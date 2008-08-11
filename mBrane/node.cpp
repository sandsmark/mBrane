//	node.cpp
//
//	Author: Eric Nivel
//
//	BSD license:
//	Copyright (c) 2008, Eric Nivel, Thor List
//	All rights reserved.
//	Redistribution and use in source and binary forms, with or without
//	modification, are permitted provided that the following conditions are met:
//
//   - Redistributions of source code must retain the above copyright
//     notice, this list of conditions and the following disclaimer.
//   - Redistributions in binary form must reproduce the above copyright
//     notice, this list of conditions and the following disclaimer in the
//     documentation and/or other materials provided with the distribution.
//   - Neither the name of Eric Nivel, Thor List nor the
//     names of their contributors may be used to endorse or promote products
//     derived from this software without specific prior written permission.
//
//	THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND ANY
//	EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
//	WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
//	DISCLAIMED. IN NO EVENT SHALL THE REGENTS AND CONTRIBUTORS BE LIABLE FOR ANY
//	DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
//	(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
//	LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
//	ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//	(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
//	SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include	"node.h"
#include	"..\Core\class_register.h"
#include	"..\Core\crank_register.h"
#include	"..\Core\control_messages.h"

#include	<iostream>


using	namespace	mBrane::sdk::payloads;

namespace	mBrane{

	Node	*Node::New(const	char	*configFileName){

		Node	*n=new	Node();
		if(n->loadConfig(configFileName))
			return	n;
		delete	n;
		return	NULL;
	}

	Node::Node():Networking(),Messaging(),PublishingSubscribing(),Executing(),nodeCount(0){
	}

	Node::~Node(){

		for(uint16	i=0;i<nodeNames.count();i++)
			if(nodeNames[i])
				free((void	*)nodeNames[i]);
	}

	Node	*Node::loadConfig(const	char	*configFileName){

		hostNameSize=Host::Name(hostName);

		XMLNode	mainNode=XMLNode::openFileHelper(configFileName,"NodeConfiguration");
		if(!mainNode){

			std::cout<<"Error: NodeConfiguration corrupted\n";
			return	NULL;
		}
		
		if(!Networking::loadConfig(mainNode))
			return	NULL;

		if(!Executing::loadConfig(mainNode))
			return	NULL;

		if(!daemon::Node::loadConfig(mainNode))
			return	NULL;

		XMLNode	nodeList=mainNode.getChildNode("Nodes");
		if(!nodeList)
			nodeCount=0;
		else{

			nodeCount=nodeList.nChildNode("Node");
			nodeNames.alloc(nodeCount);
			for(uint16	i=0;i<nodeCount;i++){

				XMLNode	n=nodeList.getChildNode(i);
				const	char	*_n=n.getAttribute("hostname");
				if(!_n){

					std::cout<<"Error: NodeConfiguration::Nodes::node_"<<i<<"::hostname is missing\n";
					nodeNames[i]=NULL;
					return	NULL;
				}
				nodeNames[i]=_n;
			}
		}

		application_configuration_file=mainNode.getAttribute("application_configuration_file");
		if(!application_configuration_file){

			std::cout<<"Error: NodeConfiguration::application_configuration_file is missing\n";
			return	NULL;
		}

		return	this;
	}

	typedef	 _Crank *(__cdecl	*CrankInstantiator)(uint16);	//	test

	Node	*Node::loadApplication(const	char	*fileName){

		if(fileName)
			application_configuration_file=fileName;
		XMLNode	mainNode=XMLNode::openFileHelper(application_configuration_file,"ApplicationConfiguration");
		if(!mainNode){

			std::cout<<"Error: ApplicationConfiguration corrupted\n";
			return	NULL;
		}
		const	char	*ul=mainNode.getAttribute("user_library");
		if(!ul){

			std::cout<<"Error: ApplicationConfiguration::user_library is missing\n";
			return	NULL;
		}
		if(!(userLibrary=SharedLibrary::New(ul)))
			return	NULL;
		//	TODO:	load:
		//		modules, groups, cranks: NB: module==entity
		//		initial subscriptions (per crank and per group)
		//		schedulers (per thread): 2nd step
		//		migrable or not (per crank)
		//		reception policy: time first or priority first (for all cranks): 2nd step
		//		user thread count: 2nd step
		//		target thread (per crank): 2nd step

		//	begin test
		CrankInstantiator	instantiator=userLibrary->getFunction<CrankInstantiator>("NewCR1");
		if(instantiator){

			_Crank	*c=(instantiator)(0);
			delete	c;
		}
		//	end test
		return	this;
	}

	void	Node::unloadApplication(){
		
		//	TODO:	unload entities, modules, groups, cranks
		delete	userLibrary;
	}

	void	Node::run(){

		if(!Networking::init())
			shutdown();
	}

	void	Node::start(uint16	assignedNID,NetworkID	*networkID,bool	isTimeReference){

		Networking::start(assignedNID,networkID,isTimeReference);

		commThreads[commThreads.count()]=Thread::New(SendMessages,this);
		
		if(networkInterfaces[CONTROL]->canBroadcast()){

			ReceiveThreadArgs	args;
			args.n=this;
			args.c=controlChannels[0];
			args.e=0;
			args.t=CONTROL;
			commThreads[commThreads.count()]=Thread::New(ReceiveMessages,&args);
		}
		
		commThreads[commThreads.count()]=Thread::New(NotifyMessages,this);

		daemon::Node::start();
		//	TODO:	build cranks
		Executing::start();
	}

	void	Node::notifyNodeJoined(uint16	NID,NetworkID	*networkID){

		static	NodeJoined	m;

		m.nid()=NID;
		m.send_ts()=Time::Get();
		Messaging::send(_ID,&m,true);

		std::cout<<"Node joined: "<<networkID->name()<<":"<<NID<<std::endl;

		if(isTimeReference	&&	nodeCount){

			for(uint16	i=0;i<nodeNames.count();i++){

				if(strcmp(nodeNames[i],networkID->name())==0){

					free((void	*)nodeNames[i]);
					nodeNames[i]=NULL;
					nodeCount--;
					break;
				}
			}
			if(!nodeCount){

				SystemReady	m;
				m.send_ts()=Time::Get();
				Messaging::send(_ID,&m,false);
			}
		}
	}

	void	Node::notifyNodeLeft(uint16	NID){

		static	NodeLeft	m;

		if(controlChannels[NID]	||	dataChannels[NID]->data	||	dataChannels[NID]->stream){

			m.nid()=NID;
			m.send_ts()=Time::Get();
			Messaging::send(_ID,&m,true);

			std::cout<<"Node left: "<<dataChannels[NID]->networkID->name()<<":"<<NID<<std::endl;
		}
	}

	void	Node::startReceivingThreads(uint16	NID){

		ReceiveThreadArgs	args;
		args.n=this;

		if(!networkInterfaces[CONTROL]->canBroadcast()){

			args.c=controlChannels[NID];
			args.e=NID;
			args.t=CONTROL;
			commThreads[commThreads.count()]=Thread::New(ReceiveMessages,&args);
		}

		if(networkInterfaces[DATA]!=networkInterfaces[CONTROL]){

			args.c=dataChannels[NID]->data;
			args.e=NID;
			args.t=DATA;
			commThreads[commThreads.count()]=Thread::New(ReceiveMessages,&args);
		}

		if(networkInterfaces[STREAM]!=networkInterfaces[DATA]){

			args.c=dataChannels[NID]->stream;
			args.e=NID;
			args.t=STREAM;
			commThreads[commThreads.count()]=Thread::New(ReceiveMessages,&args);
		}
	}

	void	Node::shutdown(){

		if(_shutdown)
			return;
		_shutdown=true;
		Executing::shutdown();
		daemon::Node::shutdown();
		Networking::shutdown();
	}

	void	Node::dump(const	char	*fileName){	//	TODO
	}
		
	void	Node::load(const	char	*fileName){	//	TODO
	}

	inline	int64	Node::time()	const{

		if(isTimeReference)
			return	Time::Get();
		else
			return	Time::Get()-timeDrift;
	}

	_Crank	*Node::buildCrank(uint16	CID){

		uint16	cid=0;	//	TODO: allocate cid (see below)
		_Crank	*c=(CrankRegister::Get(CID)->builder())(cid);
		//	TODO: read config for c, load c on a thread, update initial subscriptions, group membership etc
		return	NULL;
	}

	void	Node::start(_Crank	*c){	//	TODO
	}

	void	Node::stop(_Crank	*c){	//	TODO
	}

	void	migrate(_Crank	*c,uint16	NID){	//	TODO
	}

	void	Node::send(const	_Crank	*sender,_Payload	*message){

		Messaging::send(_ID,sender,message,false);
	}

	////////////////////////////////////////////////////////////////////////////////////////////////

	uint32	thread_function_call	Node::ReceiveMessages(void	*args){

		Node					*node=((ReceiveThreadArgs	*)args)->n;
		CommChannel				*channel=((ReceiveThreadArgs	*)args)->c;
		uint16					entry=((ReceiveThreadArgs	*)args)->e;
		NetworkInterfaceType	type=((ReceiveThreadArgs	*)args)->t;

		_Payload	*p;
		while(!node->_shutdown){

			if(channel	&&	channel->recv(&p)){

				node->processError(type,entry);
				continue;
			}

			if(!node->isTimeReference)
				node->timeDrift=p->node_recv_ts()-p->node_send_ts()-channel->rtt();	//	TODO:	make sure rtt() is quick to return
			
			P<_Payload>	_p=p;
			if(p->cid()!=TimeSync::CID())
				node->messageInputQueue.push(_p);
		}

		return	0;
	}

	uint32	thread_function_call	Node::SendMessages(void	*args){

		Node	*node=(Node	*)args;

		uint16	r;

		OutputSlot	*o;
		_Payload	*p;
		while(!node->_shutdown){

			o=node->messageOutputQueue.pop();
			p=o->p;
			if(o->local){

				Array<NodeEntry>	*nodeEntries=node->getNodeEntries(p->cid(),((_ControlMessage	*)p)->mid());
				if(nodeEntries){	//	else: mid has never been subscribed for before

					P<_Payload>	_p=p;
					node->routesCS.enter();
					uint32	act=nodeEntries->get(node->_ID)->activationCount;
					node->routesCS.leave();
					if(act)
						node->messageInputQueue.push(_p);
				}
			}else	if(p->isControlMessage()){

				for(uint16	i=0;i<node->controlChannels.count();i++){

					if(node->controlChannels[i]	&&	node->controlChannels[i]->send(p))
						node->processError(CONTROL,i);
				}
			}else{

				//	find target remote nodes; send on data/stream channels; push in messageInputQueue if the local node is a target
				Array<NodeEntry>	*nodeEntries=node->getNodeEntries(p->cid(),((_ControlMessage	*)p)->mid());
				if(nodeEntries){	//	else: mid has never been subscribed for before

					P<_Payload>	_p=p;
					node->routesCS.enter();
					if(p->isMessage()){

						for(uint32	i=0;i<nodeEntries->count();i++){

							if(nodeEntries->get(i)->activationCount){

								if(i==node->_ID)
									node->messageInputQueue.push(_p);
								else	if(r=node->dataChannels[i]->data->send(p))
									node->processError(DATA,i);
							}
						}
					}else{

						for(uint32	i=0;i<nodeEntries->count();i++){

							if(nodeEntries->get(i)->activationCount){

								if(i==node->_ID)
									node->messageInputQueue.push(_p);
								else	if(r=node->dataChannels[i]->stream->send(p))
									node->processError(STREAM,i);
							}
						}
					}
					node->routesCS.leave();
				}
			}

			if(node->isTimeReference)
				node->lastSyncTime=p->node_send_ts();

			o->p=NULL;
		}

		return	0;
	}

	uint32	thread_function_call	Node::NotifyMessages(void	*args){

		Node	*node=(Node	*)args;

		P<_Payload>	*_p;
		_Payload	*p;
		while(!node->_shutdown){

			_p=node->messageInputQueue.pop();
			p=*_p;
			if(p->isControlMessage()){

				switch(p->cid()){
				//	TODO:	process p; subscriptions/activations: routesCS.enter()/leave()
				default:	break;
				}
			}

			//	find local receiving cranks (from pub-sub structure); push p in crank input queues
			Array<NodeEntry>	*nodeEntries=node->getNodeEntries(p->cid(),((_ControlMessage	*)p)->mid());
			if(nodeEntries){	//	else: mid has never been subscribed for before

				if(nodeEntries->operator[](node->_ID).activationCount){

					P<_Payload>	_p=p;
					List<CrankEntry>	*l=nodeEntries->get(node->_ID)->cranks;
					if(l){

						List<CrankEntry>::Iterator	i;
						for(i=l->begin();i!=l->end();i++){

							if(((CrankEntry)i).activationCount)
								((CrankEntry)i).inputQueue->push(_p);
						}
					}
				}
			}
		}

		return	0;
	}
}