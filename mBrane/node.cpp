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

#define	INITIAL_TIME_GATE_DEPTH	32
#define	INITIAL_OUTPUT_QUEUE_DEPTH	32
#define	INITIAL_MID_ARRAY_LENGTH	16

namespace	mBrane{

	Node	*Node::New(const	char	*configFileName){

		Node	*n=new	Node();
		if(n->loadConfig(configFileName))
			return	n;
		delete	n;
		return	NULL;
	}

	Node::Node():sdk::NodeAPI(),_shutdown(false),networkID(NULL),isTimeReference(false),timeDrift(0){

		networkInterfaceLoaders[DISCOVERY]=NULL;
		networkInterfaceLoaders[CONTROL]=NULL;
		networkInterfaceLoaders[DATA]=NULL;
		networkInterfaceLoaders[STREAM]=NULL;

		networkInterfaces[DISCOVERY]=NULL;
		networkInterfaces[CONTROL]=NULL;
		networkInterfaces[DATA]=NULL;
		networkInterfaces[STREAM]=NULL;

		timeGate.init(INITIAL_TIME_GATE_DEPTH);
		outputQueue.init(INITIAL_OUTPUT_QUEUE_DEPTH);

		routes.alloc(ClassRegister::Count());
		for(uint32	i=0;i<ClassRegister::Count();i++)
			routes[i]=new	Array<Array<NodeEntry>	*>(INITIAL_MID_ARRAY_LENGTH);
	}

	Node::~Node(){

		if(networkInterfaces[DISCOVERY])
			delete	networkInterfaces[DISCOVERY];
		if(networkInterfaces[CONTROL])
			delete	networkInterfaces[CONTROL];
		if(networkInterfaces[DATA])
			delete	networkInterfaces[DATA];
		if(networkInterfaces[STREAM])
			delete	networkInterfaces[STREAM];

		if(networkInterfaceLoaders[DISCOVERY])
			delete	networkInterfaceLoaders[DISCOVERY];
		if(networkInterfaceLoaders[CONTROL])
			delete	networkInterfaceLoaders[CONTROL];
		if(networkInterfaceLoaders[DATA])
			delete	networkInterfaceLoaders[DATA];
		if(networkInterfaceLoaders[STREAM])
			delete	networkInterfaceLoaders[STREAM];

		if(networkID)
			delete[]	networkID;

		for(uint32	i=0;i<daemonLoaders.count();i++){

			if(daemonLoaders[i])
				delete	daemonLoaders[i];
			if(daemons[i])
				delete	daemons[i];
		}

		for(uint32	i=0;i<ClassRegister::Count();i++){

			for(uint32	j=0;j<routes[i]->count();j++){

				if(*(routes[i]->get(j))){

					for(uint32	k=0;k<routes[i]->operator[](j)->count();k++){

						if(routes[i]->operator[](j)->operator[](k).cranks)
							delete	routes[i]->operator[](j)->operator[](k).cranks;
					}
					delete	routes[i]->operator[](j);
				}
			}
			delete	routes[i];
		}
	}

	bool	Node::loadInterface(XMLNode	&n,const	char	*name,NetworkInterfaceType	type){

		XMLNode	node=n.getChildNode(name);
		if(!node){

			std::cout<<"Error: NodeConfiguration::Network::"<<name<<" is missing\n";
			return	false;
		}
		const	char	*_i=node.getAttribute("interface");
		if(!_i){

			std::cout<<"Error: NodeConfiguration::Network::"<<name<<"::interface is missing\n";
			return	false;
		}
		XMLNode	interfaces=node.getChildNode("Interfaces");
		XMLNode	i=interfaces.getChildNode(_i);
		if(!i){

			std::cout<<"Error: NodeConfiguration::Network::Interfaces::"<<_i<<" is missing\n";
			return	false;
		}else{
		
			if(!(networkInterfaceLoaders[type]=DynamicClassLoader<NetworkInterface>::New(i)))
				return	false;
			if(!(networkInterfaces[type]=networkInterfaceLoaders[type]->getInstance(i,this)))
				return	false;
		}
		return	true;
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

		if(!loadInterface(network,"Discovery",DISCOVERY))
			return	NULL;
		if(!loadInterface(network,"Control",CONTROL))
			return	NULL;
		if(!loadInterface(network,"Data",DATA))
			return	NULL;
		if(!loadInterface(network,"Stream",STREAM))
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
		if(!bcastTimeout)
			isTimeReference=true;

		XMLNode	daemons=mainNode.getChildNode("Daemons");
		if(!!daemons){

			int32	daemonCount=daemons.nChildNode("Daemon");
			for(uint32	i=0;i<daemonCount;i++){

				XMLNode	n=daemons.getChildNode(i);
				DynamicClassLoader<Daemon>	*dl;
				if(!(dl=DynamicClassLoader<Daemon>::New(n)))
					return	false;
				daemonLoaders[i]=dl;
				Daemon	*d;
				if(!(d=dl->getInstance(n,this)))
					return	false;
				this->daemons[i]=d;
			}
		}

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

		network_ctrl_ID_size=networkInterfaces[CONTROL]->getIDSize();
		network_data_ID_size=networkInterfaces[DATA]->getIDSize();
		network_stream_ID_size=networkInterfaces[STREAM]->getIDSize();
		networkID=new	uint8[network_ID_size=sizeof(uint8)+hostNameSize+network_ctrl_ID_size+network_data_ID_size+network_stream_ID_size];
		networkID[0]=hostNameSize;
		strcpy(((char	*)networkID)+sizeof(uint8),hostName);
		networkInterfaces[CONTROL]->fillID(networkID+sizeof(uint8)+hostNameSize);
		networkInterfaces[DATA]->fillID(networkID+sizeof(uint8)+hostNameSize+network_ctrl_ID_size);
		networkInterfaces[STREAM]->fillID(networkID+sizeof(uint8)+hostNameSize+network_ctrl_ID_size+network_data_ID_size);

		if(networkInterfaces[CONTROL]->canBroadcast()){
		
			BroadcastCommChannel	*ctrl_c;
			if(networkInterfaces[CONTROL]->bind(networkID,ctrl_c))
				goto	err;
			controlChannels[0]=ctrl_c;
		}

		if(networkInterfaces[DISCOVERY]->broadcastID(networkID,network_ID_size))
			goto	err;

		if(isTimeReference){

			init(0,0,true);
			return;
		}

		AcceptConnectionArgs	args;
		args.n=this;
		args.t=bcastTimeout;
		if(!networkInterfaces[CONTROL]->canBroadcast()){

			args.type=CONTROL;
			commThreads[commThreads.count()]=Thread::New(AcceptConnections,&args);
			args.t=-1;
		}
		if(networkInterfaces[DATA]->operator !=(networkInterfaces[CONTROL])){
		
			args.type=DATA;
			commThreads[commThreads.count()]=Thread::New(AcceptConnections,&args);
			args.t=-1;
		}
		if(networkInterfaces[STREAM]->operator !=(networkInterfaces[DATA])){
		
			args.type=STREAM;
			commThreads[commThreads.count()]=Thread::New(AcceptConnections,&args);
		}
		
		return;
err:	shutdown();
	}

	void	Node::init(uint16	assignedNID,uint16	remoteNID,bool	isTimeReference){

		this->isTimeReference=isTimeReference;
		dataChannels[assignedNID];	//	alloc and reserve for oneself
		_ID=assignedNID;
		referenceNID=remoteNID;

		commThreads[commThreads.count()]=Thread::New(ScanIDs,this);
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

		if(isTimeReference)
			commThreads[commThreads.count()]=Thread::New(Sync,this);

		//	TODO:	build cranks
		//			launch CEU (-> start cranks)

		//	TODO:	if(isTimeReference) wait for nodes (listed in the config file) and send SystemReady to (local) cranks
		//			else send NID to reference node

		for(uint32	i=0;i<daemons.count();i++)
			daemonThreads[i]=Thread::New(Daemon::Run,daemons[i]);
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
		Thread::Wait(daemonThreads.data(),daemonThreads.count());
		Thread::Wait(crankThreads.data(),crankThreads.count());
		Thread::Wait(commThreads.data(),commThreads.count());
		stopInterfaces();
	}

	void	Node::dump(const	char	*fileName){	//	TODO
	}
		
	void	Node::load(const	char	*fileName){	//	TODO
	}

	void	Node::send(const	sdk::_Crank	*sender,_Payload	*message){

		message->send_ts()=time();
		((_ControlMessage	*)message)->senderNode_id()=sender->id();
		P<_Payload>	p=message;
		outputQueue.push(p);
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

	void	migrate(sdk::_Crank	*c,uint16	NID){	//	TODO
	}

	bool	Node::startInterfaces(){

		if(!networkInterfaces[DISCOVERY]->start())
			return	false;
		if(!networkInterfaces[CONTROL]->start())
			return	false;
		if(networkInterfaces[DATA]->operator !=(networkInterfaces[CONTROL]))
			if(!networkInterfaces[DATA]->start())
				return	false;
		if(networkInterfaces[STREAM]->operator !=(networkInterfaces[DATA]))
			if(!networkInterfaces[STREAM]->start())
				return	false;
		return	true;
	}

	void	Node::stopInterfaces(){

		networkInterfaces[DISCOVERY]->stop();
		networkInterfaces[CONTROL]->stop();
		if(networkInterfaces[DATA]->operator !=(networkInterfaces[CONTROL]))
			networkInterfaces[DATA]->stop();
		if(networkInterfaces[STREAM]->operator !=(networkInterfaces[DATA]))
			networkInterfaces[STREAM]->stop();
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

	Array<Node::NodeEntry>	*Node::getNodeEntries(uint16	messageClassID,uint32	messageContentID){

		return	*(routes[messageClassID]->get(messageContentID));
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

			if(r=node->networkInterfaces[DISCOVERY]->scanID(remoteNetworkID,node->network_ID_size))
				goto	err3;

			uint8	remoteNameSize=remoteNetworkID[0];

			if(node->isTimeReference)
				assignedNID=node->addNodeEntry();
			else
				assignedNID=NO_ID;

			ctrl_c=NULL;
			if(!node->networkInterfaces[CONTROL]->canBroadcast()){

				if(r=node->networkInterfaces[CONTROL]->connect(remoteNetworkID+sizeof(uint8)+remoteNameSize,ctrl_c))
					goto	err3;
				if(node->sendID(ctrl_c,assignedNID))
					goto	err2;
			}
			
			if(node->networkInterfaces[DATA]->operator !=(node->networkInterfaces[CONTROL])){

				if(r=node->networkInterfaces[DATA]->connect(remoteNetworkID+sizeof(uint8)+remoteNameSize+node->network_ctrl_ID_size,data_c))
					goto	err2;
				if(node->sendID(data_c,assignedNID))
					goto	err1;
			}else
				data_c=ctrl_c;

			if(node->networkInterfaces[STREAM]->operator !=(node->networkInterfaces[DATA])){

				if(r=node->networkInterfaces[STREAM]->connect(remoteNetworkID+sizeof(uint8)+remoteNameSize+node->network_ctrl_ID_size+node->network_data_ID_size,stream_c))
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

		Node					*node=((AcceptConnectionArgs	*)args)->n;
		NetworkInterfaceType	type=((AcceptConnectionArgs	*)args)->type;
		int32					timeout=((AcceptConnectionArgs	*)args)->t;
		NetworkInterface		*networkInterface=node->networkInterfaces[type];

		uint16	r;

		ConnectedCommChannel	*c;
		uint16	remoteNID;
		char	*remoteName;
		uint8	remoteNameSize;
		while(!node->_shutdown){

			bool	timedout;
			if(r=networkInterface->acceptConnection(c,timeout,timedout))
				goto	err1;
			if(timedout){

				node->init(0,0,true);
				return	0;
			}
			
			uint16	assignedNID;
			if(r=node->recvID(c,remoteNID,remoteName,remoteNameSize,assignedNID))
				goto	err0;
			if(assignedNID!=NO_ID)
				node->init(assignedNID,remoteNID,false);

			node->dataChannels[remoteNID]->name=remoteName;
			node->dataChannels[remoteNID]->nameSize=remoteNameSize;

			bool	start=false;
			switch(type){
			case	CONTROL:
				node->controlChannels[remoteNID]=c;
				if(node->dataChannels[remoteNID]->data	&&	node->dataChannels[remoteNID]->stream)
					start=true;
				break;
			case	DATA:
				node->dataChannels[remoteNID]->data=c;
				if(node->dataChannels[remoteNID]->stream	&&	(node->networkInterfaces[CONTROL]->canBroadcast()	||	(!node->networkInterfaces[CONTROL]->canBroadcast()	&&	node->controlChannels[remoteNID])))
					start=true;
				break;
			case	STREAM:
				node->dataChannels[remoteNID]->stream=c;
				if(node->dataChannels[remoteNID]->data	&&	(node->networkInterfaces[CONTROL]->canBroadcast()	||	(!node->networkInterfaces[CONTROL]->canBroadcast()	&&	node->controlChannels[remoteNID])))
					start=true;
				break;
			}
			if(start){

				node->notifyNodeJoined(remoteNID,node->dataChannels[remoteNID]->name);
				node->startReceivingThreads(remoteNID);
			}
		}

		return	0;
err0:	delete	c;
err1:	node->shutdown();
		return	r;
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
				node->timeDrift=p->node_recv_ts()-p->node_send_ts()-channel->rtt();	//	TODO:	make sure rtt() is quick to return
			
			P<_Payload>	_p=p;
			if(p->cid()!=TimeSync::CID())
				node->timeGate.push(_p);
		}

		return	0;
	}

	uint32	thread_function_call	Node::SendMessages(void	*args){

		Node	*node=(Node	*)args;

		uint16	r;

		P<_Payload>	*_p;
		_Payload	*p;
		while(!node->_shutdown){

			_p=node->outputQueue.pop();
			p=*_p;
			if(p->isControlMessage()){

				for(uint16	i=0;i<node->controlChannels.count();i++){

					if(node->controlChannels[i]	&&	node->controlChannels[i]->send(p))
						node->processError(CONTROL,i);
				}
			}else{

				//	find target remote nodes; send on data/stream channels; push in timeGate if the local node is a target
				Array<NodeEntry>	*nodeEntries=node->getNodeEntries(p->cid(),((_ControlMessage	*)p)->mid());
				if(nodeEntries){	//	else: mid has never been subscribed for before

					P<_Payload>	_p=p;
					uint16	r;
					if(p->isMessage()){

						for(uint32	i=0;i<nodeEntries->count();i++){

							if(nodeEntries->get(i)->activationCount){

								if(i==node->_ID)
									node->timeGate.push(_p);
								else	if(r=node->dataChannels[i]->data->send(p))
									node->processError(DATA,i);
							}
						}
					}else{

						for(uint32	i=0;i<nodeEntries->count();i++){

							if(nodeEntries->get(i)->activationCount){

								if(i==node->_ID)
									node->timeGate.push(_p);
								else	if(r=node->dataChannels[i]->stream->send(p))
									node->processError(STREAM,i);
							}
						}
					}
				}
			}

			if(node->isTimeReference)
				node->lastSyncTime=p->node_send_ts();

			(*_p)=NULL;
		}

		return	0;
	}

	uint32	thread_function_call	Node::NotifyMessages(void	*args){

		Node	*node=(Node	*)args;

		P<_Payload>	*_p;
		_Payload	*p;
		while(!node->_shutdown){

			_p=node->timeGate.pop();
			p=*_p;
			if(p->isControlMessage()){

				switch(p->cid()){
				//	TODO:	process p
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