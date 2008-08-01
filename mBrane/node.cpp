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

#define	DEFAULT_TIME_GATE_DEPTH	32
#define	DEFAULT_MESSAGE_QUEUE_SIZE	16

namespace	mBrane{

	Node	*Node::New(const	char	*configFileName){

		Node	*n=new	Node();
		if(n->loadConfig(configFileName))
			return	n;
		delete	n;
		return	NULL;
	}

	Node::Node():sdk::Node(),_shutdown(false),networkID(NULL),isTimeReference(false),timeDrift(0){

		networkDiscoveryInterfaceLoader=NULL;
		networkCommInterfaceLoaders[CONTROL]=NULL;
		networkCommInterfaceLoaders[DATA]=NULL;
		networkCommInterfaceLoaders[STREAM]=NULL;

		networkDiscoveryInterface=NULL;
		networkCommInterfaces[CONTROL]=NULL;
		networkCommInterfaces[DATA]=NULL;
		networkCommInterfaces[STREAM]=NULL;

		timeGate.init(DEFAULT_TIME_GATE_DEPTH);
		CircularBuffer<MessageBuffer>::Iterator	i;
		for(i=timeGate.begin();i!=timeGate.very_end();i++)
			for(uint32	j=0;j<MESSAGE_PRIORITY_LEVELS;j++)
				((MessageBuffer)i)[j].init(DEFAULT_MESSAGE_QUEUE_SIZE);
	}

	Node::~Node(){

		if(networkDiscoveryInterface)
			delete	networkDiscoveryInterface;
		if(networkCommInterfaces[CONTROL])
			delete	networkCommInterfaces[CONTROL];
		if(networkCommInterfaces[DATA])
			delete	networkCommInterfaces[DATA];
		if(networkCommInterfaces[STREAM])
			delete	networkCommInterfaces[STREAM];

		if(networkDiscoveryInterfaceLoader)
			delete	networkDiscoveryInterfaceLoader;
		if(networkCommInterfaceLoaders[CONTROL])
			delete	networkCommInterfaceLoaders[CONTROL];
		if(networkCommInterfaceLoaders[DATA])
			delete	networkCommInterfaceLoaders[DATA];
		if(networkCommInterfaceLoaders[STREAM])
			delete	networkCommInterfaceLoaders[STREAM];

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

		XMLNode	discovery=network.getChildNode("Discovery");
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
			return	NULL;
		}else{
		
			if(!(networkDiscoveryInterfaceLoader=NetworkInterfaceLoader::New(_ndi)))
				return	NULL;
			if(!(networkDiscoveryInterface=networkDiscoveryInterfaceLoader->getInterface(discovery)))
				return	NULL;
		}

		XMLNode	parameters[3];

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
			networkCommInterfaces[CONTROL]=NULL;
			return	NULL;
		}else{
			
			if(!(networkCommInterfaceLoaders[CONTROL]=NetworkInterfaceLoader::New(nci)))
				return	NULL;
			if(!(networkCommInterfaces[CONTROL]=networkCommInterfaceLoaders[CONTROL]->getInterface(parameters[CONTROL])))
				return	NULL;
		}

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
			networkCommInterfaces[DATA]=NULL;
			return	NULL;
		}else{
			
			if(!(networkCommInterfaceLoaders[DATA]=NetworkInterfaceLoader::New(ndi)))
				return	NULL;
			if(!(networkCommInterfaces[DATA]=networkCommInterfaceLoaders[DATA]->getInterface(parameters[DATA])))
				return	NULL;
		}

		parameters[STREAM]=network.getChildNode("Stream");
		if(!parameters[STREAM]){

			std::cout<<"Error: NodeConfiguration::Network::Stream is missing\n";
			return	NULL;
		}
		const	char	*si=parameters[STREAM].getAttribute("interface");
		XMLNode	nsi=interfaces.getChildNode(si);
		if(!nsi){

			std::cout<<"Error: NodeConfiguration::Interfaces::"<<si<<" is missing\n";
			networkCommInterfaces[STREAM]=NULL;
			return	NULL;
		}else{
			
			if(!(networkCommInterfaceLoaders[STREAM]=NetworkInterfaceLoader::New(nsi)))
				return	NULL;
			if(!(networkCommInterfaces[STREAM]=networkCommInterfaceLoaders[STREAM]->getInterface(parameters[STREAM])))
				return	NULL;
		}

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

		network_ctrl_ID_size=networkCommInterfaces[CONTROL]->getIDSize();
		network_data_ID_size=networkCommInterfaces[DATA]->getIDSize();
		network_stream_ID_size=networkCommInterfaces[STREAM]->getIDSize();
		networkID=new	uint8[network_ID_size=network_ctrl_ID_size+network_data_ID_size+network_stream_ID_size];
		networkCommInterfaces[CONTROL]->fillID(networkID);
		networkCommInterfaces[DATA]->fillID(networkID+network_ctrl_ID_size);
		networkCommInterfaces[STREAM]->fillID(networkID+network_ctrl_ID_size+network_data_ID_size);

		if(networkCommInterfaces[CONTROL]->canBroadcast()){
		
			BroadcastCommChannel	*ctrl_c;
			if(networkCommInterfaces[CONTROL]->bind(networkID,ctrl_c))
				goto	err;
			controlChannels[0]=ctrl_c;
		}

		commThreads.alloc(4);
		commThreads[0]=Thread::New(AcceptConnections,this);
		
		return;
err:	shutdown();
	}

	void	Node::init(uint16	NID){

		_ID=NID;
		commThreads[commThreads.count()]=Thread::New(ScanIDs,this);
		commThreads[commThreads.count()]=Thread::New(SendMessages,this);
		
		ReceiveThreadArgs	args;
		args.n=this;
		for(uint16	i=0;controlChannels.count();i++){

			args.c=controlChannels[i];
			args.e=i;
			args.t=CONTROL;
			commThreads[commThreads.count()]=Thread::New(ReceiveMessages,&args);
		}

		commThreads[commThreads.count()]=Thread::New(ReceiveMessages,this);

		for(uint16	i=0;dataChannels.count();i++){

			if(networkCommInterfaces[DATA]!=networkCommInterfaces[CONTROL]){

				args.c=dataChannels[i]->data;
				args.e=i;
				args.t=DATA;
				commThreads[commThreads.count()]=Thread::New(ReceiveMessages,&args);
			}

			if(networkCommInterfaces[STREAM]!=networkCommInterfaces[DATA]){

				args.c=dataChannels[i]->stream;
				args.t=STREAM;
				commThreads[commThreads.count()]=Thread::New(ReceiveMessages,&args);
			}
		}

		if(isTimeReference)
			commThreads[commThreads.count()]=Thread::New(Sync,this);

		//	TODO:	build cranks
		//			launch CEU (-> start cranks)

		//	TODO:	if(isTimeReference) wait for nodes (listed in the config file) and send SystemReady to (local) cranks
		//			else send NID to reference node
	}

	void	Node::processError(NetworkInterfaceType	type,uint16	entry){

		notifyNodeLeft(entry);

		switch(type){
		case	CONTROL:
			delete	controlChannels[entry];
			controlChannels[entry]=NULL;
			break;
		case	DATA:
			delete	dataChannels[entry]->data;
			dataChannels[entry]->data=NULL;
			break;
		case	STREAM:
			delete	dataChannels[entry]->stream;
			dataChannels[entry]->stream=NULL;
			break;
		}

		if(entry==referenceNID)
			setNewReference();
	}

	void	Node::setNewReference(){	//	elect the first node in the list

		for(uint16	i=0;i<dataChannels.count();i++){

			if(dataChannels[i]->data	&&	i==_ID){

				isTimeReference=true;
				commThreads[commThreads.count()]=Thread::New(Sync,this);
				return;
			}
		}
	}

	void	Node::notifyNodeJoined(uint16	NID){

		static	NodeJoined	m;

		if(controlChannels[NID]	&&	dataChannels[NID]->data	&&	dataChannels[NID]->stream){

			m.nid()=NID;
			m.send_ts()=Time::Get();
			sendLocal(&m);

			std::cout<<"Node joined: "<<NID<<std::endl;
		}
	}

	void	Node::notifyNodeLeft(uint16	NID){

		static	NodeLeft	m;

		uint8	alive=3;
		if(!controlChannels[NID])
			alive--;
		if(!dataChannels[NID]->data)
			alive--;
		if(!dataChannels[NID]->stream)
			alive--;
		if(alive==2){

			m.nid()=NID;
			m.send_ts()=Time::Get();
			sendLocal(&m);

			std::cout<<"Node left: "<<NID<<std::endl;
		}
	}

	void	Node::shutdown(){

		if(_shutdown)
			return;
		_shutdown=true;
		Thread::Wait(crankThreads.data(),crankThreads.count());
		Thread::Wait(commThreads.data(),commThreads.count());
		stopInterfaces();
	}

	void	Node::dump(const	char	*fileName){	//	TODO
	}
		
	void	Node::load(const	char	*fileName){	//	TODO
	}

	void	Node::send(const	sdk::_Crank	*sender,_Payload	*message){	//	TODO

		//	must be thread safe
	}

	void	Node::sendLocal(_Payload	*message){	//	TODO


	}

	void	Node::sendLocal(const	sdk::_Crank	*sender,_Payload	*message){	//	TODO


	}

	void	Node::sendTo(uint16	NID,_Payload	*message){	//	TODO

		//	must be thread safe
	}

	inline	int64	Node::time()	const{

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

		if(!networkDiscoveryInterface->start())
			return	false;
		for(uint8	i=0;i<3;i++){

			if(!networkCommInterfaces[i]->start())
				return	false;
		}
		return	true;
	}

	void	Node::stopInterfaces(){

		networkDiscoveryInterface->stop();
		for(uint8	i=0;i<3;i++)
			networkCommInterfaces[i]->stop();
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
			if(!node->networkCommInterfaces[CONTROL]->canBroadcast()){

				ConnectedCommChannel	*ctrl_c;
				if(r=node->networkCommInterfaces[CONTROL]->connect(remoteNetworkID,ctrl_c))
					goto	err;

				node->controlChannels[remoteNID]=ctrl_c;
			}
			
			if(node->networkCommInterfaces[DATA]->operator !=(node->networkCommInterfaces[CONTROL])){

				if(r=node->networkCommInterfaces[DATA]->connect(remoteNetworkID+node->network_ctrl_ID_size,data_c))
					goto	err;
				if(node->isTimeReference)
					remoteNID=node->dataChannels.count();
				else
					remoteNID=NO_ID;
				if(r=data_c->send((uint8	*)&remoteNID,sizeof(uint16)))
					goto	err;
			}

			if(node->networkCommInterfaces[STREAM]->operator !=(node->networkCommInterfaces[DATA])){

				if(r=node->networkCommInterfaces[STREAM]->connect(remoteNetworkID+node->network_ctrl_ID_size+node->network_data_ID_size,stream_c))
					goto	err;
			}

			node->dataChannels[remoteNID]->data=data_c;
			node->dataChannels[remoteNID]->stream=stream_c;

			node->notifyNodeJoined(remoteNID);
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
			
			if(!node->networkCommInterfaces[CONTROL]->canBroadcast()){

				if(r=node->networkCommInterfaces[CONTROL]->acceptConnection(ctrl_c,timeout,timedout))
					goto	err;
				if(timedout)
					goto	ref;
				timeout=-1;
				if(r=ctrl_c->recv((uint8	*)&remoteNID,sizeof(uint16)))
					goto	err;

				node->controlChannels[remoteNID]=ctrl_c;

				std::cout<<"Connected <control> node: "<<remoteNID<<std::endl;
			}

			if(node->networkCommInterfaces[DATA]->operator !=(node->networkCommInterfaces[CONTROL])){

				if(r=node->networkCommInterfaces[DATA]->acceptConnection(data_c,timeout,timedout))
					goto	err;
				if(timedout)
					goto	ref;
				if(r=data_c->recv((uint8	*)&remoteNID,sizeof(uint16)))
					goto	err;

				uint16	assignedNID;
				if(r=data_c->recv((uint8	*)&assignedNID,sizeof(uint16)))
					goto	err;
				if(assignedNID!=NO_ID){

					node->referenceNID=remoteNID;
					node->init(assignedNID);
				}

				node->dataChannels[remoteNID]->data=data_c;

				std::cout<<"Connected <data> node: "<<remoteNID<<std::endl;
			}

			if(node->networkCommInterfaces[STREAM]->operator !=(node->networkCommInterfaces[DATA])){

				if(r=node->networkCommInterfaces[STREAM]->acceptConnection(stream_c,-1,timedout))
					goto	err;
				if(r=stream_c->recv((uint8	*)&remoteNID,sizeof(uint16)))
					goto	err;

				node->dataChannels[remoteNID]->data=stream_c;

				std::cout<<"Connected <stream> node: "<<remoteNID<<std::endl;
			}	
		}

		return	0;
err:	node->shutdown();
		return	r;
ref:	node->isTimeReference=true;
		node->referenceNID=0;
		node->init(0);
		return	0;
	}

	uint32	thread_function_call	Node::Sync(void	*args){

		Node	*node=(Node	*)args;

		sdk::TimeSync	sync;
		sync.senderNode_id()=node->_ID;
		while(!node->_shutdown){

			Thread::Sleep(node->syncPeriod);

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

		Node	*node=((CrankThreadArgs	*)args)->n;
		_Crank	*crank=((CrankThreadArgs	*)args)->c;

		crank->start();

		while(!node->_shutdown){

			if(!crank->alive())
				break;

			P<_Payload>	*p;
			if(crank->run()){

				do
					p=crank->pop();
				while(!*p);	//	*p can be NULL (when preview returns true)
				(*p)->recv_ts()=Time::Get();
				crank->notify(*p);
				*p=NULL;
			}else{

loop:			p=crank->pop(false);
				if(p){

					if(!*p)	//	*p can be NULL (when preview returns true)
						goto	loop;
					(*p)->recv_ts()=Time::Get();
					crank->notify(*p);
					*p=NULL;
				}
			}
		}

		crank->stop();
		delete	crank;

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
				node->timeDrift=p->node_recv_ts()-p->node_send_ts()-node->networkCommInterfaces[type]->rtt();	//	TODO:	check how long rtt takes
			
			if(p->cid()!=TimeSync::CID()){

				//	TODO:	inject in timeGate
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
			
			if(node->isTimeReference)
				node->lastSyncTime=p->node_send_ts();
		}

		return	0;
	}

	uint32	thread_function_call	Node::NotifyMessages(void	*args){

		Node	*node=(Node	*)args;
/*
		while(!node->_shutdown){

			//	TODO:	pop MessageBuffer from timeGate; for increasing priroities, pop message from buffer; find receiving cranks (from pub-sub structure); push message in cranks
		}
		*/
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