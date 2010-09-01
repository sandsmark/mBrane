//	networking.cpp
//
//	Author: Eric Nivel
//
//	BSD license:
//	Copyright (c) 2008, Eric Nivel
//	All rights reserved.
//	Redistribution and use in source and binary forms, with or without
//	modification, are permitted provided that the following conditions are met:
//
//   - Redistributions of source code must retain the above copyright
//     notice, this list of conditions and the following disclaimer.
//   - Redistributions in binary form must reproduce the above copyright
//     notice, this list of conditions and the following disclaimer in the
//     documentation and/or other materials provided with the distribution.
//   - Neither the name of Eric Nivel nor the
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

#include	"networking.h"

#include	"../Core/control_messages.h"

#include	<cstring>

using	namespace	mBrane::sdk::payloads;

namespace	mBrane{


	NodeCon::NodeCon(Networking* node, uint8 sourceNID) {
		this->node = node;
		this->sourceNID = sourceNID;
		networkID = NULL;
		for(uint32	i=0;i<6;i++) {
			commChannels[i] = NULL;
			commThreads[i] = NULL;
		}
		pushThread = NULL;
	}

	NodeCon::~NodeCon() {
		disconnect();
		node = NULL;
	};

	uint32 NodeCon::getConnectionStatus() {
		uint32 status = 0;
		if ( commChannels[CONTROL_PRIMARY] )
			status |= CONTROL_PRIMARY_INITIALISED;
		if ( commChannels[CONTROL_PRIMARY]->isConnected())
			status |= CONTROL_PRIMARY_CONNECTED;
		if ( commChannels[DATA_PRIMARY] )
			status |= DATA_PRIMARY_INITIALISED;
		if ( commChannels[DATA_PRIMARY]->isConnected())
			status |= DATA_PRIMARY_CONNECTED;
		if ( commChannels[STREAM_PRIMARY] )
			status |= STREAM_PRIMARY_INITIALISED;
		if ( commChannels[STREAM_PRIMARY]->isConnected())
			status |= STREAM_PRIMARY_CONNECTED;
		if ( commChannels[CONTROL_SECONDARY] )
			status |= CONTROL_SECONDARY_INITIALISED;
		if ( commChannels[CONTROL_SECONDARY]->isConnected())
			status |= CONTROL_SECONDARY_CONNECTED;
		if ( commChannels[DATA_SECONDARY] )
			status |= DATA_SECONDARY_INITIALISED;
		if ( commChannels[DATA_SECONDARY]->isConnected())
			status |= DATA_SECONDARY_CONNECTED;
		if ( commChannels[STREAM_SECONDARY] )
			status |= STREAM_SECONDARY_INITIALISED;
		if ( commChannels[STREAM_SECONDARY]->isConnected())
			status |= STREAM_SECONDARY_CONNECTED;
		return status;
	}

	bool NodeCon::isInUse() {
		return (pushThread != NULL);
	}

	bool NodeCon::isConnected(module::Node::Network network) {
		// ######### Check for data and control???
		switch(network) {
			case module::Node::PRIMARY:
			case module::Node::BOTH:
			case module::Node::EITHER:
				if ((!commChannels[CONTROL_PRIMARY]) || (!commChannels[CONTROL_PRIMARY]->isConnected()) ||
					(!commChannels[DATA_PRIMARY]) || (!commChannels[DATA_PRIMARY]->isConnected()) ||
					(!commChannels[STREAM_PRIMARY]) || (!commChannels[STREAM_PRIMARY]->isConnected()) ) {
					if (module::Node::PRIMARY || module::Node::BOTH)
						return false;
				}
				else if (module::Node::EITHER)
					break;
			case module::Node::SECONDARY:
				if ((!commChannels[CONTROL_SECONDARY]) || (!commChannels[CONTROL_SECONDARY]->isConnected()) ||
					(!commChannels[DATA_SECONDARY]) || (!commChannels[DATA_SECONDARY]->isConnected()) ||
					(!commChannels[STREAM_SECONDARY]) || (!commChannels[STREAM_SECONDARY]->isConnected()) ) {
					return false;
				}
		}
		return true;
	}

	bool NodeCon::disconnect() {
		CommChannel* comm;
		Thread* t;
		for(uint32	i=0;i<6;i++) {
			if(t = commThreads[i]) {
				Thread::TerminateAndWait(t);
				delete	t;
			}
			if (comm = commChannels[i]) {
				comm->disconnect();
				delete(comm);
			}
			commChannels[i] = NULL;
			commThreads[i] = NULL;
		}
		if (pushThread) {
			Thread::TerminateAndWait(pushThread);
			delete	pushThread;
			pushThread = NULL;
		}

		return true;
	}

	bool NodeCon::startNetworkChannel(CommChannel* c, uint8 type, bool isCopy) {
		if (!c)
			return false;

		if (!pushThread) {
			pushThread = Thread::New<Thread>(PushJobs,this);
		}

		CommChannel* comm;
		Thread* t;
		if(t = commThreads[type]) {
			Thread::TerminateAndWait(t);
			delete	t;
		}
		if (comm = commChannels[type]) {
			comm->disconnect();
			delete(comm);
		}

		commChannels[type] = c;
		if (!isCopy) {
			ReceiveThreadInfo* info = new ReceiveThreadInfo;
			info->con = this;
			info->channel = c;
			commThreads[type] = Thread::New<Thread>(ReceiveMessages,info);
		}
		else
			commThreads[type] = NULL;

		return true;
	}

	CommChannel* NodeCon::getNetworkChannel(uint8 type) {
		return commChannels[type];
	}

	thread_ret thread_function_call	NodeCon::ReceiveMessages(void	*args) {

		ReceiveThreadInfo	*info=(ReceiveThreadInfo	*)args;

		SyncEcho	*echo;
		_Payload	*p;
		while(info->con->node->isRunning()){

			uint64 t = Time::Get();
			if(info->channel	&&	info->channel->recv(&p,info->con->sourceNID)){

				info->con->node->processError(info->con->sourceNID);
				// continue;
				delete(info);
				thread_ret_val(0);
			}
			// printf("RecvThread::ReceiveMessages::recv took %uus...\n", Time::Get()-t);
			uint64 t0, t1, t2, t3;
			uint64 start = Time::Get();
			P<_Payload>	_p=p;
			p->node_recv_ts()=info->con->node->time();
			switch(p->cid()){
			case	SyncEcho_CID:	//	non-ref node, compute drift
				// Time Sync description for Non-Reference Node (receiver of SyncEcho)
				// t0: SyncProbe node_send_ts (local time)
				// t1: SyncProbe node_recv_ta (remote time)
				// t2: SyncEcho node_send_ts (remote time)
				// t3: SyncEcho node_recv_ta (local time)
				// RTT:  t3-t0 - (t2-t1) // network transit time
				// timeDrift = t0 - t1 - (RTT/2)
				//  = t0 - t1 - (t3 - t0 - t2 + t1)/2
				t0 = ((SyncEcho*)p)->t0;
				t1 = ((SyncEcho*)p)->t1;
				t2 = p->node_send_ts();
				t3 = start;
				info->con->node->timeDrift = t0 - t1 - (t3 - t0 - t2 + t1)/2;
				printf("*** timeDrift = %llu       RTT = %d (+ %d = %d)\n",
					info->con->node->timeDrift, (int32)(t3 - t0 - (t2 - t1)),
					(int32)(t2 - t1), (int32)(t3 - t0));
			//		((SyncEcho*)p)->t0 - ((SyncEcho*)p)->t1 -
			//		   (p->node_recv_ts() - ((SyncEcho*)p)->t0 - p->node_send_ts() + ((SyncEcho*)p)->t1)/2;
				
				// Time::Get()-((SyncEcho	*)p)->time-(p->node_recv_ts()-p->node_send_ts());
				break;
			case	SyncProbe_CID:	//	ref node, echo
				// Now the sending Node is definitely up and running...
				info->con->node->checkSyncProbe(((SyncProbe*)p)->node_id);
				// std::cout<<"> Info: Receiving SyncProbe from "<< ((SyncProbe*)p)->node_id <<" "<<std::endl;
				echo=new	SyncEcho();
				// echo->time=Time::Get();
				echo->t0 = p->node_send_ts();
				echo->t1 = start; // this needs local time, not adjusted time
				((_Payload*)echo)->node_send_ts()=((_Payload*)echo)->send_ts()=Time::Get();
				info->channel->send(echo,0xFF);
				delete	echo;
				break;
			default:
//				 std::cout<<"Pushing buffer["<< (unsigned int)(&info->con->buffer) <<"]: "<<_p->cid()<<std::endl;fflush(stdout);
				info->con->buffer.push(_p);
				break;
			}
			_p=NULL;
		}

		delete(info);
		thread_ret_val(0);
	}

	thread_ret thread_function_call	NodeCon::PushJobs(void	*args){

		NodeCon	*_this=(NodeCon	*)args;

//		std::cout<<"Starting to look for jobs ["<< (unsigned int)(&_this->source) <<"]..."<<std::endl;
		P<_Payload>	_p;
		while(_this->node->isRunning()){

//			std::cout<<"Looking for jobs ["<< (unsigned int)(&_this->source) <<"]..."<<std::endl;
			_p=_this->buffer.pop();
			_p->recv_ts()=_this->node->time();
//			std::cout<<"Pushing job: "<<_p->cid()<<std::endl;
			if(_p->category()==_Payload::CONTROL)
				_this->node->processControlMessage(_p);
			_this->node->pushJobs(_p);
			_p=NULL;
		}

		thread_ret_val(0);
	}
















	bool	Networking::checkSyncProbe(uint8 syncNodeID) {

		//if (nodeStatus[syncNodeID] != 2) {
		//	nodeStatus[syncNodeID] = 2;
		//	// printf("Node '%s' set to ready... \n", nodeNames[syncNodeID]);
		//	if (allNodesReady()) {
		//		// printf("SystemReady... \n");
		//		Node::systemReady();
		//	}
		//	else {
		//		printf("Not sending SystemReady - all nodes not ready yet... \n");
		//	}
		//}
		return	true;
	}

	Networking::Networking():Node(),Messaging(),isTimeReference(false),timeDrift(0),networkID(NULL),callbackLibrary(NULL),discoveryChannel(NULL),connectedNodeCount(0){

		uint8	i;
		for(i=0;i<7;i++){
			networkInterfaceLoaders[i]=NULL;
			networkInterfaces[i]=NULL;
		}

		for(uint8	i=0;i<32;i++){
			nodes[i] = NULL;
			//controlChannels[0][i] = NULL;
			//controlChannels[1][i] = NULL;
			commThreads[i] = NULL;
		}
		discoveryChannel = broadcastChannel[0] = broadcastChannel[1] = NULL;
		bootCallback=NULL;
	}

	Networking::~Networking(){

		for(uint8	i=0;i<7;i++){

			if(networkInterfaces[i])
				delete	networkInterfaces[i];
			if(networkInterfaceLoaders[i])
				delete	networkInterfaceLoaders[i];
		}
		
		if(networkID)
			delete	networkID;

		if(callbackLibrary)
			delete	callbackLibrary;

		if(discoveryChannel)
			delete	discoveryChannel;
		if(broadcastChannel[0])
			delete	broadcastChannel[0];
		if(broadcastChannel[1])
			delete	broadcastChannel[1];
		discoveryChannel = broadcastChannel[0] = broadcastChannel[1] = NULL;

		for(uint8	i=0;i<commThreads.count();i++){

			if(commThreads[i]) {
				delete	commThreads[i];
				commThreads[i] = NULL;
			}

			if (nodes[i]) {
				delete	nodes[i];
				delete nodes[i];
			}

		}
	}

	bool	Networking::loadInterface(XMLNode	&interfaces,XMLNode	&config,const	char	*name,InterfaceType	type){

		XMLNode	node=config.getChildNode(name);
		if(!node){

			std::cout<<"> Error: NodeConfiguration::Network::"<<name<<" is missing"<<std::endl;
			return	false;
		}
		const	char	*_i=node.getAttribute("interface");
		if(!_i){

			std::cout<<"> Error: NodeConfiguration::Network::"<<name<<"::interface is missing"<<std::endl;
			return	false;
		}
		XMLNode	i=interfaces.getChildNode(_i);
		if(!i){

			std::cout<<"> Error: NodeConfiguration::Network::Interfaces::"<<_i<<" is missing"<<std::endl;
			return	false;
		}

		if(!(networkInterfaceLoaders[type]=DynamicClassLoader<NetworkInterface>::New(i)))
			return	false;
		
		XMLNode	parameters=config.getChildNode(name);
		if(!parameters){

			std::cout<<"> Error: NodeConfiguration::Network::"<<name<<" is missing"<<std::endl;
			return	false;
		}

		if(!(networkInterfaces[type]=networkInterfaceLoaders[type]->getInstance(parameters,this)))
			return	false;

		return	true;
	}

	bool	Networking::loadConfig(XMLNode	&n){

		XMLNode	network=n.getChildNode("Network");
		if(!network){

			std::cout<<"> Error: NodeConfiguration::Network is missing"<<std::endl;
			return	false;
		}
		XMLNode	interfaces=network.getChildNode("Interfaces");
		if(!interfaces){

			std::cout<<"> Error: NodeConfiguration::Network::Interfaces is missing"<<std::endl;
			return	false;
		}
		if(!loadInterface(interfaces,network,"Discovery",DISCOVERY))
			return	false;
		if(!networkInterfaces[DISCOVERY]->canBroadcast()){

			std::cout<<"> Error: discovery interface is not broadcast capable"<<std::endl;
			return	false;
		}
		XMLNode	primary=network.getChildNode("Primary");
		if(!primary){

			std::cout<<"> Error: NodeConfiguration::Network::Primary is missing"<<std::endl;
			return	false;
		}
		XMLNode	secondary=network.getChildNode("Secondary");
		if(!secondary){

			std::cout<<"> Error: NodeConfiguration::Network::Secondary is missing"<<std::endl;
			return	false;
		}
		if(primary.nChildNode()!=3)
			this->network=SECONDARY;
		else	if(secondary.nChildNode()!=3)
			this->network=PRIMARY;
		else
			this->network=BOTH;
		if(primary.nChildNode()!=3	&&	secondary.nChildNode()!=3){

			std::cout<<"> Error: no network specified"<<std::endl;
			return	false;
		}
		if(primary.nChildNode()==3){
			
			if(!loadInterface(interfaces,primary,"Control",CONTROL_PRIMARY))
				return	false;
			if(!loadInterface(interfaces,primary,"Data",DATA_PRIMARY))
				return	false;
			if(!loadInterface(interfaces,primary,"Stream",STREAM_PRIMARY))
				return	false;
		}
		if(secondary.nChildNode()==3){
			
			if(!loadInterface(interfaces,secondary,"Control",CONTROL_SECONDARY))
				return	false;
			if(!loadInterface(interfaces,secondary,"Data",DATA_SECONDARY))
				return	false;
			if(!loadInterface(interfaces,secondary,"Stream",STREAM_SECONDARY))
				return	false;
		}

		const	char	*sp=network.getAttribute("sync_period");
		if(!sp){

			std::cout<<"> Error: NodeConfiguration::Network::sync_period is missing"<<std::endl;
			return	false;
		}
		syncPeriod=atoi(sp);

		const	char	*bt=network.getAttribute("bcast_timeout");
		if(!bt){

			std::cout<<"> Error: NodeConfiguration::Network::bcast_timeout is missing"<<std::endl;
			return	false;
		}
		bcastTimeout=atoi(bt);
		if(!bcastTimeout)
			isTimeReference=true;

		const	char	*bc=network.getAttribute("boot_callback");
		if(!bc){

			std::cout<<"> Error: NodeConfiguration::Network::boot_callback is missing"<<std::endl;
			return	false;
		}
		if(stricmp(bc,"NULL")!=0){

			if(!(callbackLibrary=SharedLibrary::New(bc)))
				return	false;
			if(!(bootCallback=callbackLibrary->getFunction<BootCallback>("BootCallback")))
				return	false;
		}

		return	true;
	}

	bool	Networking::startInterfaces(){

		if(networkInterfaces[DISCOVERY]->start())
			return	false;

		if(network==PRIMARY	||	network==BOTH){

			if(networkInterfaces[CONTROL_PRIMARY]->start())
				return	false;
			if(*networkInterfaces[DATA_PRIMARY]!=*networkInterfaces[CONTROL_PRIMARY])
				if(networkInterfaces[DATA_PRIMARY]->start())
					return	false;
			if(*networkInterfaces[STREAM_PRIMARY]!=*networkInterfaces[DATA_PRIMARY])
				if(networkInterfaces[STREAM_PRIMARY]->start())
					return	false;
		}

		if(network==SECONDARY	||	network==BOTH){

			if(networkInterfaces[CONTROL_SECONDARY]->start())
				return	false;
			if(*networkInterfaces[DATA_SECONDARY]!=*networkInterfaces[CONTROL_SECONDARY])
				if(networkInterfaces[DATA_SECONDARY]->start())
					return	false;
			if(*networkInterfaces[STREAM_SECONDARY]!=*networkInterfaces[DATA_SECONDARY])
				if(networkInterfaces[STREAM_SECONDARY]->start())
					return	false;
		}

		return	true;
	}

	void	Networking::stopInterfaces(){

		networkInterfaces[DISCOVERY]->stop();

		if(network==PRIMARY	||	network==BOTH){

			networkInterfaces[CONTROL_PRIMARY]->stop();
			if(*networkInterfaces[DATA_PRIMARY]!=*networkInterfaces[CONTROL_PRIMARY])
				networkInterfaces[DATA_PRIMARY]->stop();
			if(*networkInterfaces[STREAM_PRIMARY]!=*networkInterfaces[DATA_PRIMARY])
				networkInterfaces[STREAM_PRIMARY]->stop();
		}

		if(network==SECONDARY	||	network==BOTH){

			networkInterfaces[CONTROL_SECONDARY]->stop();
			if(*networkInterfaces[DATA_SECONDARY]!=*networkInterfaces[CONTROL_SECONDARY])
				networkInterfaces[DATA_SECONDARY]->stop();
			if(*networkInterfaces[STREAM_SECONDARY]!=*networkInterfaces[DATA_SECONDARY])
				networkInterfaces[STREAM_SECONDARY]->stop();
		}
	}

	bool	Networking::init(){

		if(!startInterfaces())
			return	false;

		NetworkID::DiscoveryIDSize=networkInterfaces[DISCOVERY]->getIDSize();

		if(network==PRIMARY	||	network==BOTH){

			NetworkID::CtrlIDSize[PRIMARY]=networkInterfaces[CONTROL_PRIMARY]->getIDSize();
			NetworkID::DataIDSize[PRIMARY]=networkInterfaces[DATA_PRIMARY]->getIDSize();
			NetworkID::StreamIDSize[PRIMARY]=networkInterfaces[STREAM_PRIMARY]->getIDSize();
		}

		if(network==SECONDARY	||	network==BOTH){

			NetworkID::CtrlIDSize[SECONDARY]=networkInterfaces[CONTROL_SECONDARY]->getIDSize();
			NetworkID::DataIDSize[SECONDARY]=networkInterfaces[DATA_SECONDARY]->getIDSize();
			NetworkID::StreamIDSize[SECONDARY]=networkInterfaces[STREAM_SECONDARY]->getIDSize();
		}

		// Include the trailing 0 in the hostName string
		networkID=new	NetworkID(_ID,network,hostNameSize+1,hostName);

		networkInterfaces[DISCOVERY]->fillID(networkID->at(DISCOVERY));
		if(networkInterfaces[DISCOVERY]->newChannel(networkID->at(DISCOVERY),&discoveryChannel))
			return	false;

		if(network==PRIMARY	||	network==BOTH){

			networkInterfaces[CONTROL_PRIMARY]->fillID(networkID->at(CONTROL_PRIMARY));
			networkInterfaces[DATA_PRIMARY]->fillID(networkID->at(DATA_PRIMARY));
			networkInterfaces[STREAM_PRIMARY]->fillID(networkID->at(STREAM_PRIMARY));
			
			if(networkInterfaces[CONTROL_PRIMARY]->canBroadcast()){
				if(networkInterfaces[CONTROL_PRIMARY]->newChannel(networkID->at(CONTROL_PRIMARY),&broadcastChannel[0])) {
					return	false;
				}
			}
			
			if(!networkInterfaces[CONTROL_PRIMARY]->canBroadcast()){

				AcceptConnectionArgs	*args=new	AcceptConnectionArgs();
				args->timeout=bcastTimeout;
				args->node=this;
				args->network=PRIMARY;
				args->category=_Payload::CONTROL;
				commThreads[commThreads.count()]=Thread::New<Thread>(AcceptConnections,args);
			}
			if(*networkInterfaces[DATA_PRIMARY]!=*networkInterfaces[CONTROL_PRIMARY]){
			
				AcceptConnectionArgs	*args=new	AcceptConnectionArgs();
				args->timeout=-1;
				args->node=this;
				args->network=PRIMARY;
				args->category=_Payload::DATA;
				commThreads[commThreads.count()]=Thread::New<Thread>(AcceptConnections,args);
			}
			if(*networkInterfaces[STREAM_PRIMARY]!=*networkInterfaces[DATA_PRIMARY]){
			
				AcceptConnectionArgs	*args=new	AcceptConnectionArgs();
				args->timeout=-1;
				args->node=this;
				args->network=PRIMARY;
				args->category=_Payload::STREAM;
				commThreads[commThreads.count()]=Thread::New<Thread>(AcceptConnections,args);
			}
		}

		if(network==SECONDARY	||	network==BOTH){

			networkInterfaces[CONTROL_SECONDARY]->fillID(networkID->at(CONTROL_SECONDARY));
			networkInterfaces[DATA_SECONDARY]->fillID(networkID->at(DATA_SECONDARY));
			networkInterfaces[STREAM_SECONDARY]->fillID(networkID->at(STREAM_SECONDARY));
			
			if(networkInterfaces[CONTROL_SECONDARY]->canBroadcast()){
		
				if(networkInterfaces[CONTROL_SECONDARY]->newChannel(networkID->at(CONTROL_SECONDARY),&broadcastChannel[0]))
					return	false;
			}
	
			if(!networkInterfaces[CONTROL_SECONDARY]->canBroadcast()){

				AcceptConnectionArgs	*args=new	AcceptConnectionArgs();
				args->timeout=bcastTimeout;
				args->node=this;
				args->network=SECONDARY;
				args->category=_Payload::CONTROL;
				commThreads[commThreads.count()]=Thread::New<Thread>(AcceptConnections,args);
			}
			if(*networkInterfaces[DATA_SECONDARY]!=*networkInterfaces[CONTROL_SECONDARY]){
			
				AcceptConnectionArgs	*args=new	AcceptConnectionArgs();
				args->timeout=-1;
				args->node=this;				
				args->network=SECONDARY;
				args->category=_Payload::DATA;
				commThreads[commThreads.count()]=Thread::New<Thread>(AcceptConnections,args);
			}
			if(*networkInterfaces[STREAM_SECONDARY]!=*networkInterfaces[DATA_SECONDARY]){
			
				AcceptConnectionArgs	*args=new	AcceptConnectionArgs();
				args->timeout=-1;
				args->node=this;
				args->network=SECONDARY;
				args->category=_Payload::STREAM;
				commThreads[commThreads.count()]=Thread::New<Thread>(AcceptConnections,args);
			}
		}

		Thread::Sleep(50);

		//std::cout<<"> Info: Sending network greeting ["<<networkID->name()<<","<<(*(uint16*)networkID->data)<<"]..."<<std::endl;
		uint16 mBraneToken = MBRANETOKEN;
		if(discoveryChannel->send((uint8*)&mBraneToken,sizeof(uint16))) {
			std::cout<<"Error: Networking Discovery Token could not be sent"<<std::endl;
			return	false;
		}
		if(discoveryChannel->send(networkID->data,NetworkID::Size+networkID->headerSize)) {
			std::cout<<"Error: Networking Discovery could not be sent"<<std::endl;
			return	false;
		}
		
		return	true;
	}

	void	Networking::start(uint8	assignedNID,NetworkID	*networkNID,bool	isTimeReference){

		this->isTimeReference=isTimeReference;
//		dataChannels[assignedNID]->networkID=this->networkID;
		_ID=assignedNID;
		this->networkID->setNID(_ID);
		if (networkNID != NULL)
			referenceNID=networkNID->NID();
		else
			referenceNID=assignedNID;

		for(uint8	i=0;i<32;i++)
			nodes[i] = new NodeCon(this, assignedNID);

		if(isTimeReference)
			commThreads[commThreads.count()]=Thread::New<Thread>(ScanIDs,this);
		else
			commThreads[commThreads.count()]=Thread::New<Thread>(Sync,this);
	}

	bool	Networking::startSync() {
		if(!isTimeReference)
			commThreads[commThreads.count()]=Thread::New<Thread>(Sync,this);
		return true;
	}

	uint16	Networking::sendID(CommChannel	*c,NetworkID	*networkID){

		//std::cout<<"> Info: SendID network greeting ["<<networkID->name()<<","<<(*(uint16*)networkID->data)<<"]..."<<std::endl;
		uint16	r;
		uint16 mBraneToken = MBRANETOKEN;
		if(r=c->send((uint8*)&mBraneToken,sizeof(uint16)))
			return	r;
		if(r=c->send(networkID->data,NetworkID::Size+networkID->headerSize))
			return	r;
		return	0;
	}

	uint16	Networking::recvID(CommChannel	*c,NetworkID	*&networkID){

		uint16	r;
		uint8	remoteNID;
		uint16 mBraneToken;
		if(r=c->recv((uint8	*)&mBraneToken,sizeof(uint16))) {
			std::cout<<"Error: Error receiving NodeID..."<<std::endl;
			return	r;
		}
		if (mBraneToken != MBRANETOKEN) {
		//	std::cout<<"Error: mBrane Token error..."<<std::endl;
			return	1;
		}
		if(r=c->recv((uint8	*)&remoteNID,sizeof(uint8)))
			return	r;
		uint8 remoteNetwork;
		// Network	;
		if(r=c->recv((uint8	*)&remoteNetwork,sizeof(uint8)))
			return	r;
		uint8	remoteNameSize;
		if(r=c->recv((uint8	*)&remoteNameSize,sizeof(uint8)))
			return	r;
		char	*remoteName=new	char[remoteNameSize];
		if(r=c->recv((uint8	*)remoteName,remoteNameSize)){

			delete[]	remoteName;
			return	r;
		}
		networkID=new	NetworkID(remoteNID,(Network)remoteNetwork,remoteNameSize,remoteName);
		delete[]	remoteName;
		if(r=c->recv(networkID->data+networkID->headerSize,networkID->Size)){

			delete	networkID;
			return	r;
		}
		return	0;
	}

	uint16	Networking::recvMap(CommChannel	*c){

		uint16	r;
		uint16	mapElementCount;
		if(r=c->recv((uint8	*)&mapElementCount,sizeof(uint16)))
			return	r;
		std::cout<<"> Info: Receiving network map containing "<<mapElementCount<<" other nodes..."<<std::endl;
		NetworkID	*networkID;
		for(uint16	i=0;i<mapElementCount;i++){

			if(r=recvID(c,networkID))
				return	r;
			if(r=connect(networkID))
				return	r;
		}
		return	0;
	}

	uint16	Networking::sendMap(CommChannel	*c){

		uint16	r;
		uint8* data = new uint8[32*sizeof(NetworkID)+sizeof(uint16)];
		NetworkID* ids = (NetworkID*)(data+sizeof(uint16));
		uint16* mapElementCount = (uint16*)data;
		*mapElementCount = 0;
		UNORDERED_MAP<uint8, NodeCon*>::iterator it, itEnd;
		for (it = nodes.begin(), itEnd=nodes.end(); it != itEnd; it++) {
			if ( it->second && it->second->isConnected() ) {
				memcpy(ids++, &it->second->networkID, sizeof(NetworkID));
				*mapElementCount++;
			}
		}

		std::cout<<"> Info: Sending network map containing "<<*mapElementCount<<" nodes..."<<std::endl;
		if(r=c->send(data,sizeof(uint16)+*mapElementCount*sizeof(NetworkID))) {
			delete [] data;
			return	r;
		}

		delete [] data;
		return	0;
	}

	uint16	Networking::connect(Network	network,NetworkID	*networkID){

		uint8	offset=network*3;
		uint16	r;
		CommChannel	*ctrl_c=NULL;
		CommChannel	*data_c=NULL;
		CommChannel	*stream_c=NULL;
		NodeCon* nodeCon = NULL;

		uint8	assignedNID = NoID;
		uint8	nid = networkID->NID();
	//	assignedNID = networkID->NID();
		if (nid == NoID) {
			if(isTimeReference) {
				assignedNID=addNodeEntry();
				networkID->setNID(assignedNID);
				nid = assignedNID;
			}
			else {
				std::cout<<"Warning: Allowing NoID for '"<< networkID->name() <<"'..."<<std::endl;
				assignedNID=NoID;
			}
		}

		if (nodes[nid] && nodes[nid]->isConnected(network))
			return	0;

		if(!networkInterfaces[offset+_Payload::CONTROL]->canBroadcast()){

			if(r=networkInterfaces[offset+_Payload::CONTROL]->newChannel(networkID->at((InterfaceType)(offset+_Payload::CONTROL)),&ctrl_c)) {
				std::cout<<"Error: Failed to open control connection to '"<< networkID->name() <<"'..."<<std::endl;
				goto	err2;
			}
			if(r=sendID(ctrl_c,this->networkID))
				goto	err1;
			if(r=ctrl_c->send((uint8	*)&assignedNID,sizeof(uint8)))
				goto	err1;
			if(isTimeReference)
				if(r=sendMap(ctrl_c))
					goto	err1;
			std::cout<<"> Info: Opened control connection to '"<< networkID->name() <<"'..."<<std::endl;
		}
			
		if(*networkInterfaces[offset+_Payload::DATA]!=*networkInterfaces[offset+_Payload::CONTROL]){

			if(r=networkInterfaces[offset+_Payload::DATA]->newChannel(networkID->at((InterfaceType)(offset+_Payload::DATA)),&data_c)) {
				std::cout<<"Error: Failed to open data connection to '"<< networkID->name() <<"'..."<<std::endl;
				goto	err1;
			}
			// In all cases we need to send our networkID
			if(r=sendID(data_c,this->networkID))
				goto	err0;
			if(networkInterfaces[offset+_Payload::CONTROL]->canBroadcast()){
				if(r=data_c->send((uint8	*)&assignedNID,sizeof(uint8)))
					goto	err0;
				if(isTimeReference)
					if(r=sendMap(data_c))
						goto	err0;
			}
			std::cout<<"> Info: Opened data connection to '"<< networkID->name() <<"'..."<<std::endl;
		}else {
			std::cout<<"> Info: Reusing control connection for data connection to '"<< networkID->name() <<"'..."<<std::endl;
			data_c=ctrl_c;
		}

		if(*networkInterfaces[offset+_Payload::STREAM]!=*networkInterfaces[offset+_Payload::DATA]){

			if(r=networkInterfaces[offset+_Payload::STREAM]->newChannel(networkID->at((InterfaceType)(offset+_Payload::STREAM)),&stream_c)) {
				std::cout<<"Error: Failed to open stream connection to '"<< networkID->name() <<"'..."<<std::endl;
				goto	err0;
			}
			// In all cases we need to send our networkID
			if(r=sendID(stream_c,this->networkID))
				goto	err0;
			std::cout<<"> Info: Opened stream connection to '"<< networkID->name() <<"'..."<<std::endl;
		}else {
			std::cout<<"> Info: Reusing control connection for stream connection to '"<< networkID->name() <<"'..."<<std::endl;
			stream_c=data_c;
		}

		nodeCon = nodes[nid];
		nodeCon->startNetworkChannel(ctrl_c, CONTROL_PRIMARY+offset);
		nodeCon->startNetworkChannel(data_c, DATA_PRIMARY+offset, data_c == ctrl_c);
		nodeCon->startNetworkChannel(data_c, STREAM_PRIMARY+offset, stream_c == data_c);
		nodeCon->networkID = networkID;
		
		//connectedNodeCount++;
		notifyNodeJoined(nid,networkID);

		return	0;
err0:	if(data_c!=ctrl_c)
			delete	data_c;
err1:	if(ctrl_c)
			delete	ctrl_c;
err2:	delete	networkID;
//		shutdown();
		return	r;
	}

	uint16	Networking::connect(NetworkID	*networkID){

		std::cout<<"> Info: From map connecting to "<<(unsigned int)networkID->NID()<<"..."<<std::endl;
		switch(network){
		case	BOTH:
		case	PRIMARY:
			switch(networkID->network()){
			case	BOTH:	
			case	PRIMARY:
				return	connect(PRIMARY,networkID);
			case	SECONDARY:
				return	0;
			}
		case	SECONDARY:
			switch(networkID->network()){
			case	PRIMARY:
				return	0;
			case	SECONDARY:
			case	BOTH:
				return	connect(SECONDARY,networkID);
			}
		}
		return	0;
	}

	uint8	Networking::addNodeEntry(){	//	assigns the first free slot

		channelsCS.enter();
		for(uint8	i=0;i<nodes.size();i++){

			if(i==_ID)
				continue;
			if ( (nodes[i] == NULL) || (!nodes[i]->isInUse()) ) {
				channelsCS.leave();
				return	i;
			}
		}
		channelsCS.leave();
		return	(uint8)nodes.size();
	}

	void	Networking::setNewReference(){	//	elect the first node in the list

		for(uint8	i=0;i<nodes.size();i++){
			// ############ to be done
		}

		//for(uint16	i=0;i<dataChannels.count();i++){

		//	if(dataChannels[i]->channels[PRIMARY].data	&&	i==_ID){

		//		isTimeReference=true;
		//		commThreads[commThreads.count()]=Thread::New<Thread>(ScanIDs,this);
		//		commThreads[commThreads.count()]=Thread::New<Thread>(Sync,this);
		//		return;
		//	}
		//}
	}

	inline	void	Networking::_broadcastControlMessage(_Payload	*p,Network	network){

		CommChannel* c;
		uint8 type = network == PRIMARY ? CONTROL_PRIMARY : CONTROL_SECONDARY;
		for(uint8	i=0;i<nodes.size();i++) {
			if((i != _ID) && (c = nodes[i]->getNetworkChannel(type)) && c->send(p,0xFF))
				processError(i);
		}
	}

	void	Networking::broadcastControlMessage(_Payload	*p,Network	network){

		if(network==PRIMARY	||	network==BOTH)
			_broadcastControlMessage(p,PRIMARY);
		if(network==SECONDARY	||	network==BOTH)
			_broadcastControlMessage(p,SECONDARY);
	}

	inline	void	Networking::_sendControlMessage(_Payload	*p,uint8	destinationNID,Network	network){

		CommChannel* c;
		uint8 type = network == PRIMARY ? CONTROL_PRIMARY : CONTROL_SECONDARY;
		if((destinationNID != _ID) && nodes[destinationNID] && (c = nodes[destinationNID]->getNetworkChannel(type))	&&	c->send(p,destinationNID))
			processError(destinationNID);
	}

	void	Networking::sendControlMessage(_Payload	*p,uint8	destinationNID,Network	network){

		if(network==PRIMARY	||	network==BOTH)
			sendControlMessage(p,destinationNID,PRIMARY);
		if(network==SECONDARY	||	network==BOTH)
			sendControlMessage(p,destinationNID,SECONDARY);
	}

	void	Networking::sendData(uint8	NID,_Payload	*p,Network	network){

		CommChannel* c;

		if((network==PRIMARY	||	network==BOTH)	&&  nodes[NID]  &&	(c = nodes[NID]->getNetworkChannel(DATA_PRIMARY)) && c->send(p,NID)){
			processError(NID);
		}
		else if((network==SECONDARY	||	network==BOTH)	&&  nodes[NID]  &&	(c = nodes[NID]->getNetworkChannel(DATA_SECONDARY)) && c->send(p,NID)){
			processError(NID);
		}
	}

	void	Networking::sendStreamData(uint8	NID,_Payload	*p,Network	network){

		CommChannel* c;

		if((network==PRIMARY	||	network==BOTH)	&&  nodes[NID]  &&	(c = nodes[NID]->getNetworkChannel(STREAM_PRIMARY)) && c->send(p,NID)){
			processError(NID);
		}
		else if((network==SECONDARY	||	network==BOTH)	&&  nodes[NID]  &&	(c = nodes[NID]->getNetworkChannel(STREAM_SECONDARY)) && c->send(p,NID)){
			processError(NID);
		}
	}

	inline	void	Networking::processError(uint8	entry){

		channelsCS.enter();

		notifyNodeLeft(entry);

		NodeCon* con = nodes[entry];
		if (con)
			con->disconnect();

		connectedNodeCount--;

		if(entry==referenceNID)
			setNewReference();

		channelsCS.leave();
	}

	void	Networking::shutdown(){

		for(uint32	i=0;i<commThreads.count();i++)
			Thread::TerminateAndWait(*commThreads.get(i));
		stopInterfaces();
	}

	////////////////////////////////////////////////////////////////////////////////////////////////

	thread_ret thread_function_call	Networking::AcceptConnections(void	*args){

		AcceptConnectionArgs	*acargs = (AcceptConnectionArgs *)args;
		Networking				*node=acargs->node;
		Network					network=acargs->network;
		uint8					offset=network*3;
		_Payload::Category		category=acargs->category;
		int32					timeout=acargs->timeout;
		NetworkInterface		*networkInterface=node->networkInterfaces[offset+category];

		uint16	r;

	//	if (category == _Payload::CONTROL)
	//		std::cout<<"Info: Waiting for "<< (uint32)networkInterface->protocol()<<" control connection..."<<std::endl;
	//	else if (category == _Payload::DATA)
	//		std::cout<<"Info: Waiting for "<< (uint32)networkInterface->protocol()<<" data connection..."<<std::endl;
	//	else if (category == _Payload::STREAM)
	//		std::cout<<"Info: Waiting for "<< (uint32)networkInterface->protocol()<<" stream connection..."<<std::endl;

		ConnectedCommChannel	*c;
		NetworkID	*networkID;
		uint8	assignedNID;
		bool	timedout;
		bool decidedRefNode = false;
		while(!node->_shutdown){

		//	std::cout<<"   ---- 0 ---- AcceptConnection "<< (uint32)networkInterface->protocol()<<"..."<<std::endl;

			if(r=networkInterface->acceptConnection(&c,timeout,timedout)) {
				std::cout<<"Error: Networking Interface acceptConnection"<<std::endl;
				goto	err1;
			}

		//	std::cout<<"   ---- 1 ---- AcceptConnection "<< (uint32)networkInterface->protocol()<<"..."<<std::endl;
			// ######### if we already have received a connection, don't startup as ref node

			node->acceptConnectionCS.enter();
		//	std::cout<<"   ---- 2 ---- AcceptConnection "<< (uint32)networkInterface->protocol()<<"..."<<std::endl;

			if ( (!decidedRefNode) && (category==_Payload::CONTROL) && (timedout) ) {	//	reference node
				std::cout<<"> Info: *** Starting up as Reference Node ***"<<std::endl;
				node->start(0,0,true);
				decidedRefNode = true;
				node->acceptConnectionCS.leave();
				//thread_ret_val(0);
				continue;
			}

			if (timedout) {
				node->acceptConnectionCS.leave();
				continue;
			}

			// std::cout<<"Info: Processing acceptConnection for protocol "<< (uint32)networkInterface->protocol()<<" ["<<category<<"]..."<<std::endl;

			//	non reference nodes
			decidedRefNode = true;
			if(category==_Payload::CONTROL	||	(category==_Payload::DATA	&&	node->networkInterfaces[offset+_Payload::CONTROL]->canBroadcast())){

				// uint16	assignedNID;
			//	std::cout<<"Info: Receiving incoming control connection..."<<std::endl;
				//std::cout<<"   ---- 3.1 ---- AcceptConnection "<< (uint32)networkInterface->protocol()<<"..."<<std::endl;
				if(r=node->recvID(c,networkID))
					goto	err0;
				//std::cout<<"   ---- 3.2 ---- AcceptConnection "<< (uint32)networkInterface->protocol()<<"...ID:"<< networkID->NID()<<"..."<<std::endl;
			//	std::cout<<"Info: Preparing to receive assigned NodeID..."<<std::endl;
				if(r=c->recv((uint8	*)&assignedNID,sizeof(uint8)))
					goto	err0;
				//std::cout<<"   ---- 3.3 ---- AcceptConnection "<< (uint32)networkInterface->protocol()<<"...Assign:"<< assignedNID<<"..."<<std::endl;
				if((assignedNID!=NoID) && (!node->isTimeReference)){

			//		std::cout<<"Info: Got assigned NodeID ["<<assignedNID<<"]..."<<std::endl;
					node->start(assignedNID,networkID,false);
					std::cout<<"> Info: My NodeID is now ["<<assignedNID<<"] assigned by ["<<networkID->NID()<<"]..."<<std::endl;
					if(r=node->recvMap(c))
						goto	err0;
				}
			}
			else if ( (category==_Payload::DATA) || (category==_Payload::STREAM) ) {
				// Just receive the remote NetworkID
				//std::cout<<"   ---- 3.5 ---- AcceptConnection "<< (uint32)networkInterface->protocol()<<"..."<<std::endl;
				if(r=node->recvID(c,networkID))
					goto	err0;
				//std::cout<<"   ---- 3.6 ---- AcceptConnection "<< (uint32)networkInterface->protocol()<<"...ID:"<< networkID->NID()<<"..."<<std::endl;
			}

//			uint16	remoteNID=((uint16	*)networkID)[0];
			uint8	remoteNID=networkID->NID();
			//std::cout<<"   ---- 4 ---- AcceptConnection "<< (uint32)networkInterface->protocol()<<" remoteID: "<<remoteNID<<"..."<<std::endl;
			NodeCon* con = node->nodes[remoteNID];
			if (!con) {
				con = node->nodes[remoteNID] = new NodeCon(node, node->_ID);
			}
			con->networkID = networkID;

			uint8 type = 0;
			if (network == PRIMARY) {
				switch(category){
					case _Payload::CONTROL:
						type = CONTROL_PRIMARY;
						break;
					case _Payload::DATA:
						type = DATA_PRIMARY;
						break;
					case _Payload::STREAM:
						type = STREAM_PRIMARY;
						break;
				}
			}
			else if (network == SECONDARY) {
				switch(category){
					case _Payload::CONTROL:
						type = CONTROL_SECONDARY;
						break;
					case _Payload::DATA:
						type = DATA_SECONDARY;
						break;
					case _Payload::STREAM:
						type = STREAM_SECONDARY;
						break;
				}
			}

			con->startNetworkChannel(c, type);

			if (con->isConnected()) {
				node->connectedNodeCount++;
				node->notifyNodeJoined(remoteNID,con->networkID);
			}
			else {
			//	std::cout<<"> Info: Not starting connection from ["<<remoteNID<<"]..."<<std::endl;
			}
			//std::cout<<"   ---- 6 ---- AcceptConnection "<< (uint32)networkInterface->protocol()<<"..."<<std::endl;
			node->acceptConnectionCS.leave();
			// std::cout<<"   ---- 7 ---- AcceptConnection "<< (uint32)networkInterface->protocol()<<"..."<<std::endl;
		}
		delete	acargs;
		node->acceptConnectionCS.leave();
		thread_ret_val(0);
err0:	delete	c;
err1:	node->shutdown();
		delete	acargs;
		node->acceptConnectionCS.leave();
		thread_ret_val(r);
	}

	thread_ret	thread_function_call	Networking::ScanIDs(void	*args){

		Networking	*node=(Networking	*)args;

		uint16	r;
		NetworkID	*networkID;
		while(!node->_shutdown){

			if(r=node->recvID(node->discoveryChannel,networkID)){
			//	std::cout<<"Info: Received non-mBrane traffic on network, ignoring..."<<std::endl;
			//	Thread::Sleep(10);
			//	node->shutdown();
			//	thread_ret_val(r);
			}
			else {
				char* recName = networkID->name();
				// Did we just receive our own broadcast
				if (stricmp(recName, node->hostName) == 0) {
					// ignore...
				//	std::cout<<"Info: Received my own network greeting, ignoring..."<<std::endl;
				}
				else if (strlen(recName) == 0) {
					// ignore...
					std::cout<<"> Warning: Received invalid network greeting with empty name, ignoring..."<<std::endl;
				}
				else {
					std::cout<<"> Info: Received network greeting from ["<<recName<<","<<*(uint16*)networkID->data<<"], connecting..."<<std::endl;
					node->connect(networkID);
				}
			}
			
		}
		thread_ret_val(0);
	}

	thread_ret	thread_function_call	Networking::Sync(void	*args){	//	executed by non-time ref nodes

		printf("Starting Network Sync...\n");
		Networking	*node=(Networking	*)args;

		CommChannel		*c;
		SyncProbe	*probe;
		while(!node->_shutdown){

			if (node->nodes[node->referenceNID] && node->nodes[node->referenceNID]->isConnected()) {
				probe=new	SyncProbe();
				probe->node_id=node->networkID->NID();
				// std::cout<<"> Info: Sending SyncProbe type '"<<probe->CID()<<"' ("<<probe->node_id<<")..."<<std::endl;
				((_Payload*)probe)->node_send_ts() = Time::Get(); // this needs local time, not adjusted time
				switch(node->network){
				case	PRIMARY:
				case	BOTH:
					if (c = node->nodes[node->referenceNID]->getNetworkChannel(DATA_PRIMARY))
						c->send(probe,0xFF);
					break;
				case	SECONDARY:
					if (c = node->nodes[node->referenceNID]->getNetworkChannel(DATA_SECONDARY))
						c->send(probe,0xFF);
					break;
				}
				Thread::Sleep(node->syncPeriod);
				//Thread::Sleep(1000000);
			}
			else {
				Thread::Sleep(100);
			}
		}
		return	0;
	}

}
