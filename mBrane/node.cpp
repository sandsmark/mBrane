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

		hostNameSize=Host::Name(hostName);

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

			sdk::_Crank	*c=(instantiator)(0);
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

		if(!startInterfaces())
			goto	err;

		network_ctrl_ID_size=networkCommInterfaces[CONTROL]->getIDSize();
		network_data_ID_size=networkCommInterfaces[DATA]->getIDSize();
		network_stream_ID_size=networkCommInterfaces[STREAM]->getIDSize();
		networkID=new	uint8[network_ID_size=sizeof(uint8)+hostNameSize+network_ctrl_ID_size+network_data_ID_size+network_stream_ID_size];
		networkID[0]=hostNameSize;
		strcpy(((char	*)networkID)+sizeof(uint8),hostName);
		networkCommInterfaces[CONTROL]->fillID(networkID+sizeof(uint8)+hostNameSize);
		networkCommInterfaces[DATA]->fillID(networkID+sizeof(uint8)+hostNameSize+network_ctrl_ID_size);
		networkCommInterfaces[STREAM]->fillID(networkID+sizeof(uint8)+hostNameSize+network_ctrl_ID_size+network_data_ID_size);

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
		if(networkCommInterfaces[CONTROL]->canBroadcast()){

			ReceiveThreadArgs	args;
			args.n=this;
			args.c=controlChannels[0];
			args.e=0;
			args.t=CONTROL;
			commThreads[commThreads.count()]=Thread::New(ReceiveMessages,&args);
		}
		
		commThreads[commThreads.count()]=Thread::New(NotifyMessages,this);

		if(isTimeReference)
			commThreads[commThreads.count()]=Thread::New(Sync,this);

		//	TODO:	build cranks
		//			launch CEU (-> start cranks)

		//	TODO:	if(isTimeReference) wait for nodes (listed in the config file) and send SystemReady to (local) cranks
		//			else send NID to reference node
	}

	void	Node::startReceivingThreads(uint16	NID){

		ReceiveThreadArgs	args;
		args.n=this;

		if(!networkCommInterfaces[CONTROL]->canBroadcast()){

			args.c=controlChannels[NID];
			args.e=NID;
			args.t=CONTROL;
			commThreads[commThreads.count()]=Thread::New(ReceiveMessages,&args);
		}

		if(networkCommInterfaces[DATA]!=networkCommInterfaces[CONTROL]){

			args.c=dataChannels[NID]->data;
			args.e=NID;
			args.t=DATA;
			commThreads[commThreads.count()]=Thread::New(ReceiveMessages,&args);
		}

		if(networkCommInterfaces[STREAM]!=networkCommInterfaces[DATA]){

			args.c=dataChannels[NID]->stream;
			args.e=NID;
			args.t=STREAM;
			commThreads[commThreads.count()]=Thread::New(ReceiveMessages,&args);
		}
	}

	void	Node::processError(NetworkInterfaceType	type,uint16	entry){

		m.acquire();

		notifyNodeLeft(entry);

		if(controlChannels[entry])
			delete	controlChannels[entry];
		controlChannels[entry]=NULL;
		if(dataChannels[entry]->data)
			delete	dataChannels[entry]->data;
		dataChannels[entry]->data=NULL;
		if(dataChannels[entry]->stream)
			delete	dataChannels[entry]->stream;
		dataChannels[entry]->stream=NULL;

		if(entry==referenceNID)
			setNewReference();

		m.release();
	}

	uint16	Node::addNodeEntry(){

		m.acquire();

		for(uint16	i=0;i<dataChannels.count();i++){

			if(i==_ID)
				continue;
			if(!dataChannels[i]->data){

				m.release();
				return	i;
			}
		}
		
		m.release();
		return	dataChannels.count();
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

	void	Node::notifyNodeJoined(uint16	NID,char	*name){

		static	NodeJoined	m;

		m.nid()=NID;
		m.send_ts()=Time::Get();
		sendLocal(&m);

		std::cout<<"Node joined: "<<name<<":"<<NID<<std::endl;
	}

	void	Node::notifyNodeLeft(uint16	NID){

		static	NodeLeft	m;

		if(controlChannels[NID]	||	dataChannels[NID]->data	||	dataChannels[NID]->stream){

			m.nid()=NID;
			m.send_ts()=Time::Get();
			sendLocal(&m);

			std::cout<<"Node left: "<<dataChannels[NID]->name<<":"<<NID<<std::endl;
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
		if(!networkCommInterfaces[CONTROL]->start())
			return	false;
		if(networkCommInterfaces[DATA]->operator !=(networkCommInterfaces[CONTROL]))
			if(!networkCommInterfaces[DATA]->start())
				return	false;
		if(networkCommInterfaces[STREAM]->operator !=(networkCommInterfaces[DATA]))
			if(!networkCommInterfaces[STREAM]->start())
				return	false;
		return	true;
	}

	void	Node::stopInterfaces(){

		networkDiscoveryInterface->stop();
		networkCommInterfaces[CONTROL]->stop();
		if(networkCommInterfaces[DATA]->operator !=(networkCommInterfaces[CONTROL]))
			networkCommInterfaces[DATA]->stop();
		if(networkCommInterfaces[STREAM]->operator !=(networkCommInterfaces[DATA]))
			networkCommInterfaces[STREAM]->stop();
	}

	uint16	Node::sendID(ConnectedCommChannel	*c,uint16	assignedNID){

		uint16	r;
		if(r=c->send((uint8	*)&_ID,sizeof(uint16)))
			return	r;
		if(r=c->send((uint8	*)&hostNameSize,sizeof(uint8)))
			return	r;
		if(r=c->send((uint8	*)hostName,hostNameSize))
			return	r;
		if(r=c->send((uint8	*)&assignedNID,sizeof(uint16)))
			return	r;
		return	0;
	}

	uint16	Node::recvID(ConnectedCommChannel	*c,uint16	&NID,char	*&name,uint8	&nameSize,uint16	&assignedNID){

		uint16	r;
		if(r=c->recv((uint8	*)&NID,sizeof(uint16)))
			return	r;
		if(r=c->recv((uint8	*)&nameSize,sizeof(uint8)))
			return	r;
		name=new	char[nameSize];
		if(r=c->recv((uint8	*)name,nameSize)){

			delete[]	name;
			return	r;
		}
		if(r=c->recv((uint8	*)&assignedNID,sizeof(uint16))){

			delete[]	name;
			return	r;
		}
		return	0;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////

	uint32	thread_function_call	Node::ScanIDs(void	*args){

		Node	*node=(Node	*)args;

		uint16	r;
		
		ConnectedCommChannel	*ctrl_c;
		ConnectedCommChannel	*data_c;
		ConnectedCommChannel	*stream_c;
		uint16	remoteNID;
		uint16	assignedNID;
		uint8	*remoteNetworkID=new	uint8[node->network_ID_size];
		while(!node->_shutdown){

			if(r=node->networkDiscoveryInterface->scanID(remoteNetworkID,node->network_ID_size))
				goto	err3;

			uint8	remoteNameSize=remoteNetworkID[0];

			if(node->isTimeReference)
				assignedNID=node->addNodeEntry();
			else
				assignedNID=NO_ID;

			ctrl_c=NULL;
			if(!node->networkCommInterfaces[CONTROL]->canBroadcast()){

				if(r=node->networkCommInterfaces[CONTROL]->connect(remoteNetworkID+sizeof(uint8)+remoteNameSize,ctrl_c))
					goto	err3;
				if(node->sendID(ctrl_c,assignedNID))
					goto	err2;
			}
			
			if(node->networkCommInterfaces[DATA]->operator !=(node->networkCommInterfaces[CONTROL])){

				if(r=node->networkCommInterfaces[DATA]->connect(remoteNetworkID+sizeof(uint8)+remoteNameSize+node->network_ctrl_ID_size,data_c))
					goto	err2;
				if(node->sendID(data_c,assignedNID))
					goto	err1;
			}else
				data_c=ctrl_c;

			if(node->networkCommInterfaces[STREAM]->operator !=(node->networkCommInterfaces[DATA])){

				if(r=node->networkCommInterfaces[STREAM]->connect(remoteNetworkID+sizeof(uint8)+remoteNameSize+node->network_ctrl_ID_size+node->network_data_ID_size,stream_c))
					goto	err1;
				if(node->sendID(stream_c,assignedNID))
					goto	err0;
			}else
				stream_c=data_c;

			node->dataChannels[assignedNID]->nameSize=remoteNameSize;
			node->dataChannels[assignedNID]->name=new	char[remoteNameSize];
			memcpy(node->dataChannels[assignedNID]->name,remoteNetworkID+1,remoteNameSize);
			if(ctrl_c)
				node->controlChannels[assignedNID]=ctrl_c;
			node->dataChannels[assignedNID]->data=data_c;
			node->dataChannels[assignedNID]->stream=stream_c;
			node->notifyNodeJoined(assignedNID,node->dataChannels[assignedNID]->name);
			node->startReceivingThreads(assignedNID);
		}

		delete[]	remoteNetworkID;
		return	0;
err0:	if(stream_c!=data_c)
			delete	stream_c;
err1:	if(data_c!=ctrl_c)
			delete	data_c;
err2:	if(ctrl_c)
			delete	ctrl_c;
err3:	delete[]	remoteNetworkID;
		node->shutdown();
		return	r;
	}

	uint32	thread_function_call	Node::AcceptConnections(void	*args){

		Node	*node=(Node	*)args;

		uint16	r;

		if(r=node->networkDiscoveryInterface->broadcastID(node->networkID,node->network_ID_size))
			goto	err2;

		ConnectedCommChannel	*ctrl_c;
		ConnectedCommChannel	*data_c;
		ConnectedCommChannel	*stream_c;
		int32	timeout=node->bcastTimeout;
		uint16	remoteNID;
		char	*remoteName;
		uint8	remoteNameSize;
		while(!node->_shutdown){

			bool	timedout;
			ctrl_c=NULL;
			if(!node->networkCommInterfaces[CONTROL]->canBroadcast()){

				if(r=node->networkCommInterfaces[CONTROL]->acceptConnection(ctrl_c,timeout,timedout))
					goto	err3;
				if(timedout)
					goto	ref;
				timeout=-1;
				
				uint16	assignedNID;
				if(r=node->recvID(ctrl_c,remoteNID,remoteName,remoteNameSize,assignedNID))
					goto	err2;
				if(assignedNID!=NO_ID){

					node->referenceNID=remoteNID;
					node->dataChannels[assignedNID];	//	alloc for oneself
					node->init(assignedNID);
				}

				node->dataChannels[remoteNID]->name=remoteName;
				node->dataChannels[remoteNID]->nameSize=remoteNameSize;
				node->controlChannels[remoteNID]=ctrl_c;
				if(node->dataChannels[remoteNID]->data	&&	node->dataChannels[remoteNID]->stream){
					
					node->notifyNodeJoined(remoteNID,node->dataChannels[remoteNID]->name);
					node->startReceivingThreads(remoteNID);
				}
			}

			if(node->networkCommInterfaces[DATA]->operator !=(node->networkCommInterfaces[CONTROL])){

				if(r=node->networkCommInterfaces[DATA]->acceptConnection(data_c,timeout,timedout))
					goto	err2;
				if(timedout)
					goto	ref;

				uint16	assignedNID;
				if(r=node->recvID(data_c,remoteNID,remoteName,remoteNameSize,assignedNID))
					goto	err1;
				if(assignedNID!=NO_ID){

					node->referenceNID=remoteNID;
					node->dataChannels[assignedNID];	//	alloc for oneself
					node->init(assignedNID);
				}
			}else
				data_c=ctrl_c;

			node->dataChannels[remoteNID]->name=remoteName;
			node->dataChannels[remoteNID]->nameSize=remoteNameSize;
			node->dataChannels[remoteNID]->data=data_c;
			if(node->dataChannels[remoteNID]->stream){
					
				if((!node->networkCommInterfaces[CONTROL]->canBroadcast()	&&	node->controlChannels[remoteNID])	||	node->networkCommInterfaces[CONTROL]->canBroadcast()){

					node->notifyNodeJoined(remoteNID,node->dataChannels[remoteNID]->name);
					node->startReceivingThreads(remoteNID);
				}
			}

			if(node->networkCommInterfaces[STREAM]->operator !=(node->networkCommInterfaces[DATA])){

				if(r=node->networkCommInterfaces[STREAM]->acceptConnection(stream_c,-1,timedout))
					goto	err1;
				
				uint16	assignedNID;
				if(r=node->recvID(stream_c,remoteNID,remoteName,remoteNameSize,assignedNID))
					goto	err0;
				if(assignedNID!=NO_ID){

					node->referenceNID=remoteNID;
					node->dataChannels[assignedNID];	//	alloc for oneself
					node->init(assignedNID);
				}
			}else
				stream_c=data_c;

			node->dataChannels[remoteNID]->name=remoteName;
			node->dataChannels[remoteNID]->nameSize=remoteNameSize;
			node->dataChannels[remoteNID]->stream=stream_c;
			if(node->dataChannels[remoteNID]->data){
					
				if((!node->networkCommInterfaces[CONTROL]->canBroadcast()	&&	node->controlChannels[remoteNID])	||	node->networkCommInterfaces[CONTROL]->canBroadcast()){

					node->notifyNodeJoined(remoteNID,node->dataChannels[remoteNID]->name);
					node->startReceivingThreads(remoteNID);
				}
			}
		}

		return	0;
err0:	if(stream_c!=data_c)
			delete	stream_c;
err1:	if(ctrl_c!=data_c)
			delete	data_c;
err2:	if(ctrl_c)
			delete	ctrl_c;
err3:	node->shutdown();
		return	r;
ref:	node->isTimeReference=true;
		node->referenceNID=0;
		node->dataChannels[0];	//	alloc for oneself
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
				node->timeDrift=p->node_recv_ts()-p->node_send_ts()-node->networkCommInterfaces[type]->rtt();	//	TODO:	make sure rtt() is quick to return
			
			P<_Payload>	_p=p;
			if(p->cid()!=TimeSync::CID())
				node->timeGate.push(_p);
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

				//	TODO:	send p where required (use pub-sub structure to find target remote nodes; send on data/stream channels; push in timeGate if the local node is a target)
			}
			
			if(node->isTimeReference)
				node->lastSyncTime=p->node_send_ts();
		}

		return	0;
	}

	uint32	thread_function_call	Node::NotifyMessages(void	*args){

		Node	*node=(Node	*)args;

		P<_Payload>	*p;
		while(!node->_shutdown){

			p=node->timeGate.pop();
			if((*p)->isControlMessage()){

				switch((*p)->cid()){
				//	TODO:	process p
				default:	break;
				}
			}

			//	TODO:	find receiving cranks (from pub-sub structure); push p in cranks
		}

		return	0;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////

	Node::DataCommChannel::DataCommChannel():data(NULL),stream(NULL),name(NULL),nameSize(0){
	}

	Node::DataCommChannel::~DataCommChannel(){

		if(data)
			delete	data;
		if(stream)
			delete	stream;
		if(name)
			delete[]	name;
	}
}