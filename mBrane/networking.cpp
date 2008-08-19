//	networking.cpp
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

#include	"networking.h"

#include	"..\Core\control_messages.h"


using	namespace	mBrane::sdk::payloads;

namespace	mBrane{

	Networking::Networking():Node(),isTimeReference(false),timeDrift(0),networkID(NULL),callbackLibrary(NULL),discoveryChannel(NULL),connectedNodeCount(0){

		for(uint8	i=0;i<7;i++){

			networkInterfaceLoaders[i]=NULL;
			networkInterfaces[i]=NULL;
		}
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

		for(uint32	i=0;i<commThreads.count();i++){

			if(commThreads[i])
				delete	commThreads[i];
		}
	}

	bool	Networking::loadInterface(XMLNode	&n,const	char	*name,InterfaceType	type){

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

	bool	Networking::loadConfig(XMLNode	&n){

		XMLNode	network=n.getChildNode("Network");
		if(!network){

			std::cout<<"Error: NodeConfiguration::Network is missing\n";
			return	false;
		}
		XMLNode	interfaces=network.getChildNode("Interfaces");
		if(!interfaces){

			std::cout<<"Error: NodeConfiguration::Network::Interfaces is missing\n";
			return	false;
		}
		if(!loadInterface(network,"Discovery",DISCOVERY))
			return	false;
		if(!networkInterfaces[DISCOVERY]->canBroadcast()){

			std::cout<<"Error: discovery interface is not broadcast capable\n";
			return	false;
		}
		XMLNode	primary=network.getChildNode("Primary");
		if(!primary){

			std::cout<<"Error: NodeConfiguration::Network::Primary is missing\n";
			return	false;
		}
		XMLNode	secondary=network.getChildNode("Secondary");
		if(!secondary){

			std::cout<<"Error: NodeConfiguration::Network::Secondary is missing\n";
			return	false;
		}
		if(primary.nChildNode()!=3)
			this->network=SECONDARY;
		else	if(secondary.nChildNode()!=3)
			this->network=PRIMARY;
		else
			this->network=BOTH;
		if(primary.nChildNode()!=3	&&	secondary.nChildNode()!=3){

			std::cout<<"Error: no network specified\n";
			return	false;
		}
		if(primary.nChildNode()==3){
			
			if(!loadInterface(primary,"Control",CONTROL_PRIMARY))
				return	false;
			if(!loadInterface(primary,"Data",DATA_PRIMARY))
				return	false;
			if(!loadInterface(primary,"Stream",STREAM_PRIMARY))
				return	false;
		}
		if(secondary.nChildNode()==3){
			
			if(!loadInterface(secondary,"Control",CONTROL_SECONDARY))
				return	false;
			if(!loadInterface(secondary,"Data",DATA_SECONDARY))
				return	false;
			if(!loadInterface(secondary,"Stream",STREAM_SECONDARY))
				return	false;
		}

		const	char	*sp=network.getAttribute("sync_period");
		if(!sp){

			std::cout<<"Error: NodeConfiguration::Network::sync_period is missing\n";
			return	false;
		}
		syncPeriod=atoi(sp);

		const	char	*bt=network.getAttribute("bcast_timeout");
		if(!bt){

			std::cout<<"Error: NodeConfiguration::Network::bcast_timeout is missing\n";
			return	false;
		}
		bcastTimeout=atoi(bt);
		if(!bcastTimeout)
			isTimeReference=true;

		const	char	*bc=network.getAttribute("boot_callback");
		if(!bc){

			std::cout<<"Error: NodeConfiguration::Network::boot_callback is missing\n";
			return	false;
		}
		if(strcmp(bc,"NULL")!=0){

			if(!(callbackLibrary=SharedLibrary::New(bc)))
				return	false;
			if(!(bootCallback=callbackLibrary->getFunction<BootCallback>("BootCallback")))
				return	false;
		}

		return	true;
	}

	bool	Networking::startInterfaces(){

		if(!networkInterfaces[DISCOVERY]->start())
			return	false;

		if(network==PRIMARY	||	network==BOTH){

			if(!networkInterfaces[CONTROL_PRIMARY]->start())
				return	false;
			if(*networkInterfaces[DATA_PRIMARY]!=*networkInterfaces[CONTROL_PRIMARY])
				if(!networkInterfaces[DATA_PRIMARY]->start())
					return	false;
			if(*networkInterfaces[STREAM_PRIMARY]!=*networkInterfaces[DATA_PRIMARY])
				if(!networkInterfaces[STREAM_PRIMARY]->start())
					return	false;
		}

		if(network==SECONDARY	||	network==BOTH){

			if(!networkInterfaces[CONTROL_SECONDARY]->start())
				return	false;
			if(*networkInterfaces[DATA_SECONDARY]!=*networkInterfaces[CONTROL_SECONDARY])
				if(!networkInterfaces[DATA_SECONDARY]->start())
					return	false;
			if(*networkInterfaces[STREAM_SECONDARY]!=*networkInterfaces[DATA_SECONDARY])
				if(!networkInterfaces[STREAM_SECONDARY]->start())
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

		networkID=new	NetworkID(_ID,network,hostNameSize,hostName);

		networkInterfaces[DISCOVERY]->fillID(networkID->at(DISCOVERY));
		if(networkInterfaces[DISCOVERY]->newChannel(networkID->at(DISCOVERY),&discoveryChannel))
			return	false;

		if(network==PRIMARY	||	network==BOTH){

			networkInterfaces[CONTROL_PRIMARY]->fillID(networkID->at(CONTROL_PRIMARY));
			networkInterfaces[DATA_PRIMARY]->fillID(networkID->at(DATA_PRIMARY));
			networkInterfaces[STREAM_PRIMARY]->fillID(networkID->at(STREAM_PRIMARY));
			
			if(networkInterfaces[CONTROL_PRIMARY]->canBroadcast()){
		
				if(networkInterfaces[CONTROL_PRIMARY]->newChannel(networkID->at(CONTROL_PRIMARY),&(controlChannels[PRIMARY][0])))
					return	false;
			}
			
			AcceptConnectionArgs	args;
			args.node=this;
			args.timeout=bcastTimeout;
			if(!networkInterfaces[CONTROL_PRIMARY]->canBroadcast()){

				args.network=PRIMARY;
				args.category=_Payload::CONTROL;
				commThreads[commThreads.count()]=Thread::New<Thread>(AcceptConnections,&args);
				args.timeout=-1;
			}
			if(*networkInterfaces[DATA_PRIMARY]!=*networkInterfaces[CONTROL_PRIMARY]){
			
				args.network=PRIMARY;
				args.category=_Payload::DATA;
				commThreads[commThreads.count()]=Thread::New<Thread>(AcceptConnections,&args);
				args.timeout=-1;
			}
			if(*networkInterfaces[STREAM_PRIMARY]!=*networkInterfaces[DATA_PRIMARY]){
			
				args.network=PRIMARY;
				args.category=_Payload::STREAM;
				commThreads[commThreads.count()]=Thread::New<Thread>(AcceptConnections,&args);
			}
		}

		if(network==SECONDARY	||	network==BOTH){

			networkInterfaces[CONTROL_SECONDARY]->fillID(networkID->at(CONTROL_SECONDARY));
			networkInterfaces[DATA_SECONDARY]->fillID(networkID->at(DATA_SECONDARY));
			networkInterfaces[STREAM_SECONDARY]->fillID(networkID->at(STREAM_SECONDARY));
			
			if(networkInterfaces[CONTROL_SECONDARY]->canBroadcast()){
		
				if(networkInterfaces[CONTROL_SECONDARY]->newChannel(networkID->at(CONTROL_SECONDARY),&(controlChannels[SECONDARY][0])))
					return	false;
			}

			AcceptConnectionArgs	args;
			args.node=this;
			args.timeout=bcastTimeout;
			if(!networkInterfaces[CONTROL_SECONDARY]->canBroadcast()){

				args.network=SECONDARY;
				args.category=_Payload::CONTROL;
				commThreads[commThreads.count()]=Thread::New<Thread>(AcceptConnections,&args);
				args.timeout=-1;
			}
			if(*networkInterfaces[DATA_SECONDARY]!=*networkInterfaces[CONTROL_SECONDARY]){
			
				args.network=SECONDARY;
				args.category=_Payload::DATA;
				commThreads[commThreads.count()]=Thread::New<Thread>(AcceptConnections,&args);
				args.timeout=-1;
			}
			if(*networkInterfaces[STREAM_SECONDARY]!=*networkInterfaces[DATA_SECONDARY]){
			
				args.network=SECONDARY;
				args.category=_Payload::STREAM;
				commThreads[commThreads.count()]=Thread::New<Thread>(AcceptConnections,&args);
			}
		}

		if(discoveryChannel->send(networkID->data,NetworkID::Size+networkID->headerSize))
			return	false;
		
		return	true;
	}

	void	Networking::start(uint16	assignedNID,NetworkID	*networkNID,bool	isTimeReference){

		this->isTimeReference=isTimeReference;
		dataChannels[assignedNID]->networkID=this->networkID;
		_ID=assignedNID;
		referenceNID=networkID->NID();

		if(isTimeReference){

			commThreads[commThreads.count()]=Thread::New<Thread>(ScanIDs,this);
			commThreads[commThreads.count()]=Thread::New<Thread>(Sync,this);
		}
	}

	uint16	Networking::sendID(CommChannel	*c,NetworkID	*networkID){

		uint16	r;
		if(r=c->send(networkID->data,NetworkID::Size+networkID->headerSize))
			return	r;
		return	0;
	}

	uint16	Networking::recvID(CommChannel	*c,NetworkID	*&networkID){

		uint16	r;
		uint16	remoteNID;
		if(r=c->recv((uint8	*)&remoteNID,sizeof(uint16)))
			return	r;
		Network	remoteNetwork;
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
		networkID=new	NetworkID(remoteNID,remoteNetwork,remoteNameSize,remoteName);
		delete[]	remoteName;
		if(r=c->recv(networkID->data,NetworkID::Size+networkID->headerSize)){

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
		uint16	mapElementCount;
		if(r=c->send((uint8	*)&mapElementCount,sizeof(uint16)))
			return	r;
		for(uint16	i=0;i<dataChannels.count();i++){

			if(i==_ID)
				continue;
			if(!dataChannels[i]->networkID)
				continue;
			if(r=sendID(c,dataChannels[i]->networkID))
				return	r;
		}
		return	0;
	}

	uint16	Networking::connect(Network	network,NetworkID	*networkID){

		uint8	offset=network*3;
		uint16	r;
		CommChannel	*ctrl_c=NULL;
		CommChannel	*data_c;
		CommChannel	*stream_c;
		uint16	assignedNID;

		if(isTimeReference)
			assignedNID=addNodeEntry();
		else
			assignedNID=NO_ID;

		if(!networkInterfaces[offset+_Payload::CONTROL]->canBroadcast()){

			if(r=networkInterfaces[offset+_Payload::CONTROL]->newChannel(networkID->at((InterfaceType)(offset+_Payload::CONTROL)),&ctrl_c))
				goto	err2;
			if(r=sendID(ctrl_c,this->networkID))
				goto	err1;
			if(r=ctrl_c->send((uint8	*)&assignedNID,sizeof(uint16)))
				goto	err1;
			if(isTimeReference)
				if(r=sendMap(ctrl_c))
					goto	err1;
		}
			
		if(*networkInterfaces[offset+_Payload::DATA]!=*networkInterfaces[offset+_Payload::CONTROL]){

			if(r=networkInterfaces[offset+_Payload::DATA]->newChannel(networkID->at((InterfaceType)(offset+_Payload::DATA)),&data_c))
				goto	err1;
			if(networkInterfaces[offset+_Payload::CONTROL]->canBroadcast()){

				if(r=sendID(data_c,this->networkID))
					goto	err0;
				if(r=data_c->send((uint8	*)&assignedNID,sizeof(uint16)))
					goto	err0;
				if(isTimeReference)
					if(r=sendMap(data_c))
						goto	err0;
			}
		}else
			data_c=ctrl_c;

		if(*networkInterfaces[offset+_Payload::STREAM]!=*networkInterfaces[offset+_Payload::DATA]){

			if(r=networkInterfaces[offset+_Payload::STREAM]->newChannel(networkID->at((InterfaceType)(offset+_Payload::STREAM)),&stream_c))
				goto	err0;
		}else
			stream_c=data_c;

		controlChannels[network][assignedNID]=ctrl_c;
		dataChannels[assignedNID]->channels[network].data=data_c;
		dataChannels[assignedNID]->channels[network].stream=stream_c;
		dataChannels[assignedNID]->networkID=networkID;
		connectedNodeCount++;
		startReceivingThreads(assignedNID);
		notifyNodeJoined(assignedNID,networkID);

		return	0;
err0:	if(data_c!=ctrl_c)
			delete	data_c;
err1:	if(ctrl_c)
			delete	ctrl_c;
err2:	delete[]	networkID;
		shutdown();
		return	r;
	}

	uint16	Networking::connect(NetworkID	*networkID){

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
	}

	uint16	Networking::addNodeEntry(){	//	assigns the first free slot

		channelsCS.enter();
		for(uint16	i=0;i<dataChannels.count();i++){

			if(i==_ID)
				continue;
			if(!dataChannels[i]->channels[PRIMARY].data	&&	!dataChannels[i]->channels[SECONDARY].data){

				channelsCS.leave();
				return	i;
			}
		}
		channelsCS.leave();
		return	dataChannels.count();
	}

	void	Networking::setNewReference(){	//	elect the first node in the list

		for(uint16	i=0;i<dataChannels.count();i++){

			if(dataChannels[i]->channels[PRIMARY].data	&&	i==_ID){

				isTimeReference=true;
				commThreads[commThreads.count()]=Thread::New<Thread>(ScanIDs,this);
				commThreads[commThreads.count()]=Thread::New<Thread>(Sync,this);
				return;
			}
		}
	}

	inline	void	Networking::broadcastControlMessage(Network	network,_Payload	*p){

		for(uint16	i=0;i<controlChannels[network].count();i++){

			if(controlChannels[network][i]	&&	controlChannels[network][i]->send(p))
				processError(i);
		}
	}

	void	Networking::sendData(uint16	NID,_Payload	*p){

		if(dataChannels[NID]->channels[PRIMARY].data){

			if(dataChannels[NID]->channels[PRIMARY].data->send(p))
				processError(NID);
		}else	if(dataChannels[NID]->channels[SECONDARY].data->send(p))
			processError(NID);
	}

	void	Networking::sendStreamData(uint16	NID,_Payload	*p){

		if(dataChannels[NID]->channels[PRIMARY].stream){

			if(dataChannels[NID]->channels[PRIMARY].stream->send(p))
				processError(NID);
		}else	if(dataChannels[NID]->channels[SECONDARY].stream->send(p))
			processError(NID);
	}

	inline	void	Networking::processError(uint16	entry){

		channelsCS.enter();

		notifyNodeLeft(entry);

		if(controlChannels[PRIMARY][entry])
			delete	controlChannels[PRIMARY][entry];
		controlChannels[PRIMARY][entry]=NULL;
		if(dataChannels[entry]->channels[PRIMARY].data)
			delete	dataChannels[entry]->channels[PRIMARY].data;
		dataChannels[entry]->channels[PRIMARY].data=NULL;
		if(dataChannels[entry]->channels[PRIMARY].stream)
			delete	dataChannels[entry]->channels[PRIMARY].stream;
		dataChannels[entry]->channels[PRIMARY].stream=NULL;

		if(controlChannels[SECONDARY][entry])
			delete	controlChannels[SECONDARY][entry];
		controlChannels[SECONDARY][entry]=NULL;
		if(dataChannels[entry]->channels[SECONDARY].data)
			delete	dataChannels[entry]->channels[SECONDARY].data;
		dataChannels[entry]->channels[SECONDARY].data=NULL;
		if(dataChannels[entry]->channels[SECONDARY].stream)
			delete	dataChannels[entry]->channels[SECONDARY].stream;
		dataChannels[entry]->channels[SECONDARY].stream=NULL;


		if(dataChannels[entry]->networkID)
			delete	dataChannels[entry]->networkID;
		dataChannels[entry]->networkID=NULL;

		connectedNodeCount--;

		if(entry==referenceNID)
			setNewReference();

		channelsCS.leave();
	}

	void	Networking::shutdown(){

		Thread::Wait(commThreads.data(),commThreads.count());
		stopInterfaces();
	}

	////////////////////////////////////////////////////////////////////////////////////////////////

	uint32	thread_function_call	Networking::AcceptConnections(void	*args){

		Networking			*node=((AcceptConnectionArgs	*)args)->node;
		Network				network=((AcceptConnectionArgs	*)args)->network;
		uint8	offset=network*3;
		_Payload::Category	category=((AcceptConnectionArgs	*)args)->category;
		int32				timeout=((AcceptConnectionArgs	*)args)->timeout;
		NetworkInterface	*networkInterface=node->networkInterfaces[offset+category];

		uint16	r;

		ConnectedCommChannel	*c;
		NetworkID	*networkID;
		uint16	assignedID;
		while(!node->_shutdown){

			bool	timedout;
			if(r=networkInterface->acceptConnection(&c,timeout,timedout))
				goto	err1;
			if(timedout){	//	reference node

				node->start(0,0,true);
				return	0;
			}
			
			//	non reference nodes
			if(category==_Payload::CONTROL	||	(category==_Payload::DATA	&&	node->networkInterfaces[offset+_Payload::CONTROL]->canBroadcast())){

				uint16	assignedNID;
				if(r=node->recvID(c,networkID))
					goto	err0;
				if(r=c->recv((uint8	*)&assignedID,sizeof(uint16)))
					goto	err0;
				if(assignedNID!=NO_ID){

					if(r=node->recvMap(c))
						goto	err0;
					node->start(assignedNID,networkID,false);
				}
			}

			uint16	remoteNID=((uint16	*)networkID)[0];
			node->dataChannels[remoteNID]->networkID=networkID;

			bool	start=false;
			switch(category){
			case	_Payload::CONTROL:
				node->controlChannels[network][remoteNID]=c;
				if(node->dataChannels[remoteNID]->channels[network].data	&&	node->dataChannels[remoteNID]->channels[network].stream)
					start=true;
				break;
			case	_Payload::DATA:
				node->dataChannels[remoteNID]->channels[network].data=c;
				if(node->dataChannels[remoteNID]->channels[network].stream	&&	(node->networkInterfaces[offset+_Payload::CONTROL]->canBroadcast()	||	(!node->networkInterfaces[offset+_Payload::CONTROL]->canBroadcast()	&&	node->controlChannels[network][remoteNID])))
					start=true;
				break;
			case	_Payload::STREAM:
				node->dataChannels[remoteNID]->channels[network].stream=c;
				if(node->dataChannels[remoteNID]->channels[network].data	&&	(node->networkInterfaces[offset+_Payload::CONTROL]->canBroadcast()	||	(!node->networkInterfaces[offset+_Payload::CONTROL]->canBroadcast()	&&	node->controlChannels[network][remoteNID])))
					start=true;
				break;
			default:
				break;
			}
			if(start){

				node->connectedNodeCount++;
				node->startReceivingThreads(remoteNID);
				node->notifyNodeJoined(remoteNID,node->dataChannels[remoteNID]->networkID);
			}
		}
		return	0;
err0:	delete	c;
err1:	node->shutdown();
		return	r;
	}

	uint32	thread_function_call	Networking::ScanIDs(void	*args){

		Networking	*node=(Networking	*)args;

		uint16	r;
		NetworkID	*networkID;
		while(!node->_shutdown){

			if(r=node->recvID(node->discoveryChannel,networkID)){

				node->shutdown();
				return	r;
			}
			node->connect(networkID);
		}
		return	0;
	}

	uint32	thread_function_call	Networking::Sync(void	*args){

		Networking	*node=(Networking	*)args;

		TimeSync	sync;
		while(!node->_shutdown){

			Thread::Sleep(node->syncPeriod);

			int64	t=node->time();
			if(t-node->lastSyncTime>=node->syncPeriod){

				if(node->network==PRIMARY	||	node->network==BOTH){

					sync.send_ts()=Time::Get();
					node->broadcastControlMessage(PRIMARY,&sync);
				}else	if(node->network==SECONDARY	||	node->network==BOTH){

					sync.send_ts()=Time::Get();
					node->broadcastControlMessage(SECONDARY,&sync);
				}
				node->lastSyncTime=t;
			}
		}
		return	0;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////

	Networking::DataCommChannel::DataCommChannel():networkID(NULL){

		channels[PRIMARY].data=channels[PRIMARY].stream=NULL;
		channels[SECONDARY].data=channels[SECONDARY].stream=NULL;
	}

	Networking::DataCommChannel::~DataCommChannel(){

		if(channels[PRIMARY].data)
			delete	channels[PRIMARY].data;
		if(channels[PRIMARY].stream)
			delete	channels[PRIMARY].stream;
		if(channels[SECONDARY].data)
			delete	channels[SECONDARY].data;
		if(channels[SECONDARY].stream)
			delete	channels[SECONDARY].stream;
		if(networkID)
			delete	networkID;
	}
}