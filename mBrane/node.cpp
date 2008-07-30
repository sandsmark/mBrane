// node.cpp
//
// Author: Eric Nivel
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
#include	"..\Core\crank.h"
#include	"..\Core\class_register.h"
#include	"..\Core\crank_register.h"
#include	"..\Core\control_messages.h"

#include	<iostream>


using	namespace	mBrane::sdk;

namespace	mBrane{

	Node	*Node::New(const	char	*configFileName){

		Node	*n=new	Node();
		if(n->loadConfig(configFileName))
			return	n;
		delete	n;
		return	NULL;
	}

	Node::Node():sdk::Node(),_shutdown(false),networkID(NULL),isTimeReference(false),timeDrift(0){

		networkDiscoveryInterface=NULL;
		networkInterfaces[CONTROL]=NULL;
		networkInterfaces[DATA]=NULL;
		networkInterfaces[STREAM]=NULL;
	}

	Node::~Node(){

		if(networkDiscoveryInterface)
			delete	networkDiscoveryInterface;
		if(networkInterfaces[CONTROL])
			delete	networkInterfaces[CONTROL];
		if(networkInterfaces[DATA])
			delete	networkInterfaces[DATA];
		if(networkInterfaces[STREAM])
			delete	networkInterfaces[STREAM];

		if(networkID)
			delete[]	networkID;
	}

	Node	*Node::loadConfig(const	char	*configFileName){

		Host::Name(hostName);

		XMLNode	mainNode=XMLNode::openFileHelper(configFileName,"NodeConfiguration");
		if(!mainNode){

			std::cout<<"Error: NodeConfiguration corrupted\n";
			return	NULL;
		}
		XMLNode	network=mainNode.getChildNode("Network");
		if(!network){

			std::cout<<"Error: NodeConfiguration::Network is missing\n";
			return	NULL;
		}
		XMLNode	interfaces=network.getChildNode("Interfaces");
		if(!interfaces){

			std::cout<<"Error: NodeConfiguration::Network::Interfaces is missing\n";
			return	NULL;
		}
		discovery=network.getChildNode("Discovery");
		if(!discovery){

			std::cout<<"Error: NodeConfiguration::Network::Discovery is missing\n";
			return	NULL;
		}
		const	char	*_di=discovery.getAttribute("interface");
		if(!_di){

			std::cout<<"Error: NodeConfiguration::Network::Discovery::interface is missing\n";
			return	NULL;
		}
		XMLNode	_ndi=interfaces.getChildNode(_di);
		if(!_ndi){

			std::cout<<"Error: NodeConfiguration::Network::Interfaces::"<<_di<<" is missing\n";
			networkDiscoveryInterface=NULL;
			return	NULL;
		}else	if(!(networkDiscoveryInterface=NetworkInterface::New<NetworkDiscoveryInterface>(_ndi)))
			return	NULL;
		parameters[CONTROL]=network.getChildNode("Control");
		if(!parameters[CONTROL]){

			std::cout<<"Error: NodeConfiguration::Network::Control is missing\n";
			return	NULL;
		}
		const	char	*ci=parameters[CONTROL].getAttribute("interface");
		if(!ci){

			std::cout<<"Error: NodeConfiguration::Network::Control::interface is missing\n";
			return	NULL;
		}
		XMLNode	nci=interfaces.getChildNode(ci);
		if(!nci){

			std::cout<<"Error: NodeConfiguration::Network::Interfaces::"<<ci<<" is missing\n";
			networkInterfaces[CONTROL]=NULL;
			return	NULL;
		}else	if(!(networkInterfaces[CONTROL]=NetworkInterface::New<NetworkCommInterface>(nci)))
			return	NULL;
		parameters[DATA]=network.getChildNode("Data");
		if(!parameters[DATA]){

			std::cout<<"Error: NodeConfiguration::Network::Data is missing\n";
			return	NULL;
		}
		const	char	*di=parameters[DATA].getAttribute("interface");
		if(!di){

			std::cout<<"Error: NodeConfiguration::Data::interface is missing\n";
			return	NULL;
		}
		XMLNode	ndi=interfaces.getChildNode(di);
		if(!ndi){

			std::cout<<"Error: NodeConfiguration::Network::Interfaces::"<<di<<" is missing\n";
			networkInterfaces[DATA]=NULL;
			return	NULL;
		}else	if(!(networkInterfaces[DATA]=NetworkInterface::New<NetworkCommInterface>(ndi)))
			return	NULL;
		parameters[STREAM]=network.getChildNode("Stream");
		if(!parameters[STREAM]){

			std::cout<<"Error: NodeConfiguration::Network::Stream is missing\n";
			return	NULL;
		}
		const	char	*si=parameters[STREAM].getAttribute("interface");
		XMLNode	nsi=interfaces.getChildNode(si);
		if(!nsi){

			std::cout<<"Error: NodeConfiguration::Interfaces::"<<si<<" is missing\n";
			networkInterfaces[STREAM]=NULL;
			return	NULL;
		}else	if(!(networkInterfaces[STREAM]=NetworkInterface::New<NetworkCommInterface>(nsi)))
			return	NULL;
		application_configuration_file=mainNode.getAttribute("application_configuration_file");
		if(!application_configuration_file){

			std::cout<<"Error: NodeConfiguration::application_configuration_file is missing\n";
			return	NULL;
		}
		const	char	*sp=network.getAttribute("sync_period");
		if(!sp){

			std::cout<<"Error: NodeConfiguration::Network::sync_period is missing\n";
			return	NULL;
		}
		syncPeriod=atoi(sp);
		const	char	*bt=network.getAttribute("bcast_timeout");
		if(!bt){

			std::cout<<"Error: NodeConfiguration::Network::bcast_timeout is missing\n";
			return	NULL;
		}
		bcastTimeout=atoi(bt);
		return	this;
	}

	typedef	 sdk::_Crank *(__cdecl	*CrankInstantiator)(uint16);	//	test

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
		//		entities, modules, groups, cranks
		//		initial subscriptions (per crank, group)
		//		schedulers (per crank)
		//		migrable or not (per crank)
		//		reception policy: time first or priority first (for all cranks)
		//		user thread count
		//		target thread (per crank)

		//	test
		CrankInstantiator	instantiator=userLibrary->getFunction<CrankInstantiator>("NewCR1");
		if(instantiator){

			sdk::_Crank	*c=(instantiator)(0);
			delete	c;
		}
		return	this;
	}

	void	Node::unloadApplication(){
		//	to unload
		//		entities, modules, groups, cranks
		//		user library
		delete	userLibrary;
	}

	void	Node::run(){

		if(!startInterfaces())
			goto	err;

		network_ctrl_ID_size=networkInterfaces[CONTROL]->getIDSize();
		network_data_ID_size=networkInterfaces[DATA]->getIDSize();
		network_stream_ID_size=networkInterfaces[STREAM]->getIDSize();
		networkID=new	uint8[network_ID_size=network_ctrl_ID_size+network_data_ID_size+network_stream_ID_size];
		networkInterfaces[CONTROL]->fillID(networkID);
		networkInterfaces[DATA]->fillID(networkID+network_ctrl_ID_size);
		networkInterfaces[STREAM]->fillID(networkID+network_ctrl_ID_size+network_data_ID_size);

		if(networkInterfaces[CONTROL]->canBroadcast()){
		
			BroadcastCommChannel	*ctrl_c;
			if(networkInterfaces[CONTROL]->bind(networkID,ctrl_c))
				goto	err;
			controlChannels[0]=ctrl_c;
		}

		_threads.alloc(4);
		_threads[0]=Thread::New(AcceptConnections,this);
		
		return;
err:	shutdown();
	}

	void	Node::init(uint16	NID){

		_ID=NID;
		_threads[_threads.count()]=Thread::New(ScanIDs,this);
		if(isTimeReference)
			_threads[_threads.count()]=Thread::New(Sync,this);
		_threads[_threads.count()]=Thread::New(SendMessages,this);
		
		ReceiveThreadArgs	args;
		args.n=this;
		for(uint16	i=0;controlChannels.count();i++){

			args.c=controlChannels[i];
			args.e=i;
			args.t=CONTROL;
			_threads[_threads.count()]=Thread::New(ReceiveMessages,&args);
		}

		for(uint16	i=0;dataChannels.count();i++){

			if(networkInterfaces[DATA]!=networkInterfaces[CONTROL]){

				args.c=dataChannels[i]->data;
				args.e=i;
				args.t=DATA;
				_threads[_threads.count()]=Thread::New(ReceiveMessages,&args);
			}

			if(networkInterfaces[STREAM]!=networkInterfaces[DATA]){

				args.c=dataChannels[i]->stream;
				args.t=STREAM;
				_threads[_threads.count()]=Thread::New(ReceiveMessages,&args);
			}
		}
		//	TODO:	build cranks and load them in CEU
		//			launch CEU
		//			wait for nodes (specified in the config file) and send SystemReady to (local) cranks
	}

	void	Node::processError(NetworkInterfaceType	type,uint16	entry){

		switch(type){
		case	CONTROL:
			//	TODO:	notify channel disappearance (ctrl msg? std::cout?)
			delete	controlChannels[entry];
			controlChannels[entry]=NULL;
			break;
		case	DATA:
			//	TODO:	notify channel disappearance (ctrl msg? std::cout?)
			delete	dataChannels[entry]->data;
			dataChannels[entry]->data=NULL;
			break;
		case	STREAM:
			//	TODO:	notify channel disappearance (ctrl msg? std::cout?)
			delete	dataChannels[entry]->stream;
			dataChannels[entry]->stream=NULL;
			break;
		}

		if(!controlChannels[entry]	&&	!dataChannels[entry]->data	&&	!dataChannels[entry]->stream){

			//	TODO:	notify node disappearance (ctrl msg? std::cout?)
		}
	}

	void	Node::shutdown(){

		if(_shutdown)
			return;
		_shutdown=true;
		Thread::Wait(_threads.data(),_threads.count());
		stopInterfaces();
	}

	void	Node::dump(const	char	*fileName){	//	TODO
	}
		
	void	Node::load(const	char	*fileName){	//	TODO
	}

	void	Node::send(sdk::_Crank	*sender,_Payload	*message){	//	TODO

		//	must be thread safe
	}

	void	Node::sendLocal(sdk::_Crank	*sender,_Payload	*message){	//	TODO
	}

	void	Node::sendTo(uint16	NID,_Payload	*message){	//	TODO

		//	must be thread safe
	}

	inline	int64	Node::time(){

		if(isTimeReference)
			return	Time::Get();
		else
			return	Time::Get()-timeDrift;
	}

	sdk::_Crank	*Node::buildCrank(uint16	CID){

		uint16	cid=0;	//	TODO: allocate cid (see below)
		_Crank	*c=(CrankRegister::Get(CID)->builder())(cid);
		//	TODO: read config for c, load c on a thread, update initial subscriptions, group membership etc
		return	NULL;
	}

	void	Node::start(sdk::_Crank	*c){	//	TODO
	}

	void	Node::stop(sdk::_Crank	*c){	//	TODO
	}

	bool	Node::startInterfaces(){

		if(!networkDiscoveryInterface->start(discovery))
			return	false;
		for(uint8	i=0;i<3;i++){

			if(!networkInterfaces[i]->start(parameters[i]))
				return	false;
		}
		return	true;
	}

	void	Node::stopInterfaces(){

		networkDiscoveryInterface->stop();
		for(uint8	i=0;i<3;i++)
			networkInterfaces[i]->stop();
	}

	////////////////////////////////////////////////////////////////////////////////////////////////

	uint32	thread_function_call	Node::ScanIDs(void	*args){

		Node	*node=(Node	*)args;

		uint16	r;
		
		ConnectedCommChannel	*data_c;
		ConnectedCommChannel	*stream_c;
		uint16	remoteNID;
		uint8	*remoteNetworkID=new	uint8[node->network_ID_size];
		while(!node->_shutdown){

			if(r=node->networkDiscoveryInterface->scanID(remoteNetworkID,node->network_ID_size))
				goto	err;
			if(!node->networkInterfaces[CONTROL]->canBroadcast()){

				ConnectedCommChannel	*ctrl_c;
				if(r=node->networkInterfaces[CONTROL]->connect(remoteNetworkID,ctrl_c))
					goto	err;

				node->controlChannels[remoteNID]=ctrl_c;
			}
			
			if(node->networkInterfaces[DATA]!=node->networkInterfaces[CONTROL]){

				if(r=node->networkInterfaces[DATA]->connect(remoteNetworkID+node->network_ctrl_ID_size,data_c))
					goto	err;
				if(node->isTimeReference)
					remoteNID=node->dataChannels.count();
				else
					remoteNID=NO_ID;
				if(r=data_c->send((uint8	*)&remoteNID,sizeof(uint16)))
					goto	err;
			}

			if(node->networkInterfaces[STREAM]!=node->networkInterfaces[DATA]){

				if(r=node->networkInterfaces[STREAM]->connect(remoteNetworkID+node->network_ctrl_ID_size+node->network_data_ID_size,stream_c))
					goto	err;
			}

			node->dataChannels[remoteNID]->data=data_c;
			node->dataChannels[remoteNID]->stream=stream_c;

			//	TODO:	notify node appearance (ctrl msg? std::cout?) => flag apparitions to notify only once (when ctrl+data+stream are present)
		}

		delete[]	remoteNetworkID;
		return	0;
err:	delete[]	remoteNetworkID;
		node->shutdown();
		return	r;
	}

	uint32	thread_function_call	Node::AcceptConnections(void	*args){

		Node	*node=(Node	*)args;

		uint16	r;

		if(r=node->networkDiscoveryInterface->broadcastID(node->networkID,node->network_ID_size))
			goto	err;

		int32	timeout=node->bcastTimeout;
		int16	remoteNID;
		while(!node->_shutdown){

			bool	timedout;
			ConnectedCommChannel	*ctrl_c;
			ConnectedCommChannel	*data_c;
			ConnectedCommChannel	*stream_c;
			
			if(!node->networkInterfaces[CONTROL]->canBroadcast()){

				if(r=node->networkInterfaces[CONTROL]->acceptConnection(ctrl_c,timeout,timedout))
					goto	err;
				if(timedout)
					goto	ref;
				timeout=-1;
				if(r=ctrl_c->recv((uint8	*)&remoteNID,sizeof(uint16)))
					goto	err;

				node->controlChannels[remoteNID]=ctrl_c;
			}

			if(node->networkInterfaces[DATA]!=node->networkInterfaces[CONTROL]){

				if(r=node->networkInterfaces[DATA]->acceptConnection(data_c,timeout,timedout))
					goto	err;
				if(timedout)
					goto	ref;
				if(r=data_c->recv((uint8	*)&remoteNID,sizeof(uint16)))
					goto	err;

				uint16	assignedNID;
				if(r=data_c->recv((uint8	*)&assignedNID,sizeof(uint16)))
					goto	err;
				if(assignedNID!=NO_ID)
					node->init(assignedNID);

				node->dataChannels[remoteNID]->data=data_c;
			}

			if(node->networkInterfaces[STREAM]!=node->networkInterfaces[DATA]){

				if(r=node->networkInterfaces[STREAM]->acceptConnection(stream_c,-1,timedout))
					goto	err;
				if(r=stream_c->recv((uint8	*)&remoteNID,sizeof(uint16)))
					goto	err;

				node->dataChannels[remoteNID]->data=stream_c;
			}
		}
		return	0;
err:	node->shutdown();
		return	r;
ref:	node->isTimeReference=true;
		node->init(0);
		return	0;
	}

	uint32	thread_function_call	Node::Sync(void	*args){

		Node	*node=(Node	*)args;

		sdk::TimeSync	sync;
		sync.senderNode_id()=node->_ID;
		while(!node->_shutdown){

			int64	t=node->time();
			if(t-node->lastSyncTime>=node->syncPeriod){

				sync.send_ts()=Time::Get();
				for(uint16	i=0;i<node->controlChannels.count();i++){

					if(node->controlChannels[i]	&&	node->controlChannels[i]->send(&sync))
						node->processError(CONTROL,i);
				}
				node->lastSyncTime=t;
			}
		}

		return	0;
	}
	
	uint32	thread_function_call	Node::CrankExecutionUnit(void	*args){

		Node	*node=(Node	*)args;

		while(!node->_shutdown){

			//	TODO:	
		}

		return	0;
	}

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
				node->timeDrift=p->node_recv_ts()-p->node_send_ts()-node->networkInterfaces[type]->rtt();	//	TODO:	check how long rtt takes
			
			if(p->cid()!=TimeSync::CID()){

				//	TODO:	inject in pub-sub structure (latched input queue: thread safe)
			}
		}

		return	0;
	}

	uint32	thread_function_call	Node::SendMessages(void	*args){

		Node	*node=(Node	*)args;

		uint16	r;

		_Payload	*p;
		while(!node->_shutdown){

			//	TODO:	extract p from pub-sub output queue (blocking call)
			if(p->isControlMessage()){

				for(uint16	i=0;i<node->controlChannels.count();i++){

					if(node->controlChannels[i]	&&	node->controlChannels[i]->send(p))
						node->processError(CONTROL,i);
				}
			}else{

				//	TODO:	send p where required (remote nodes on data or stream channels)
			}
			
			int64	t=node->time();
			if(node->isTimeReference)
				node->lastSyncTime=t;
		}

		return	0;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////

	Node::DataCommChannel::DataCommChannel():data(NULL),stream(NULL){
	}

	Node::DataCommChannel::~DataCommChannel(){

		if(data)
			delete	data;
		if(stream)
			delete	stream;
	}
}