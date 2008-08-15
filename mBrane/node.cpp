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

	Node::Node():Networking(),MESSAGING_CLASS(),PublishingSubscribing(),Executing(),nodeCount(0){
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
		Messaging::start();
		
		if(networkInterfaces[CONTROL]->canBroadcast()){

			RecvThread	*t=new	RecvThread(this,controlChannels[0],0,CONTROL);
			recvThreads[recvThreads.count()]=t;
			t->start(RecvThread::ReceiveMessages);
		}

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

		if(!networkInterfaces[CONTROL]->canBroadcast()){

			RecvThread	*t=new	RecvThread(this,controlChannels[NID],NID,CONTROL);
			recvThreads[recvThreads.count()]=t;
			t->start(RecvThread::ReceiveMessages);
		}

		if(networkInterfaces[DATA]!=networkInterfaces[CONTROL]){

			RecvThread	*t=new	RecvThread(this,dataChannels[NID]->data,NID,DATA);
			recvThreads[recvThreads.count()]=t;
			t->start(RecvThread::ReceiveMessages);
		}

		if(networkInterfaces[STREAM]!=networkInterfaces[DATA]){

			RecvThread	*t=new	RecvThread(this,dataChannels[NID]->stream,NID,STREAM);
			recvThreads[recvThreads.count()]=t;
			t->start(RecvThread::ReceiveMessages);
		}
	}

	void	Node::shutdown(){

		if(_shutdown)
			return;
		_shutdown=true;
		Messaging::shutdown();
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

	inline	Array<PublishingSubscribing::NodeEntry>	*Node::getNodeEntries(uint16	messageClassID,uint32	messageContentID){

		return	*(routes[messageClassID]->get(messageContentID));
	}
}