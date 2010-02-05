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

	Networking::Networking():Node(),isTimeReference(false),timeDrift(0),networkID(NULL),callbackLibrary(NULL),discoveryChannel(NULL),connectedNodeCount(0){

		uint8	i;
		for(i=0;i<7;i++){
			networkInterfaceLoaders[i]=NULL;
			networkInterfaces[i]=NULL;
		}

		for(uint32	i=0;i<32;i++){
			controlChannels[0][i] = NULL;
			controlChannels[1][i] = NULL;
			commThreads[i] = NULL;
		}

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

	// has already been deleted above
	//	if(discoveryChannel)
	//		delete	discoveryChannel;

		for(uint32	i=0;i<commThreads.count();i++){

			if(commThreads[i])
				delete	commThreads[i];
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
		
				if(networkInterfaces[CONTROL_PRIMARY]->newChannel(networkID->at(CONTROL_PRIMARY),&(controlChannels[PRIMARY][0])))
					return	false;
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
		
				if(networkInterfaces[CONTROL_SECONDARY]->newChannel(networkID->at(CONTROL_SECONDARY),&(controlChannels[SECONDARY][0])))
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

		std::cout<<"> Info: Sending network greeting ["<<networkID->name()<<","<<(*(uint16*)networkID->data)<<"]..."<<std::endl;
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

	void	Networking::start(uint16	assignedNID,NetworkID	*networkNID,bool	isTimeReference){

		this->isTimeReference=isTimeReference;
		dataChannels[assignedNID]->networkID=this->networkID;
		_ID=assignedNID;
		this->networkID->setNID(_ID);
		if (networkNID != NULL)
			referenceNID=networkNID->NID();
		else
			referenceNID=assignedNID;

		if(isTimeReference)
			commThreads[commThreads.count()]=Thread::New<Thread>(ScanIDs,this);
	//	else
	//		commThreads[commThreads.count()]=Thread::New<Thread>(Sync,this);
	}

	bool	Networking::startSync() {
		if(!isTimeReference)
			commThreads[commThreads.count()]=Thread::New<Thread>(Sync,this);
		return true;
	}

	uint16	Networking::sendID(CommChannel	*c,NetworkID	*networkID){

		std::cout<<"> Info: SendID network greeting ["<<networkID->name()<<","<<(*(uint16*)networkID->data)<<"]..."<<std::endl;
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
		uint16	remoteNID;
		uint16 mBraneToken;
		if(r=c->recv((uint8	*)&mBraneToken,sizeof(uint16))) {
			std::cout<<"Error: Error receiving NodeID..."<<std::endl;
			return	r;
		}
		if (mBraneToken != MBRANETOKEN) {
		//	std::cout<<"Error: mBrane Token error..."<<std::endl;
			return	1;
		}
		if(r=c->recv((uint8	*)&remoteNID,sizeof(uint16)))
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
		uint16	i;
		uint16	mapElementCount = 0;
		for(i=0;i<dataChannels.count();i++){
		//	if ( (i!=_ID) && (dataChannels[i]->networkID) )
			if (dataChannels[i]->networkID)
				mapElementCount++;
		}

		std::cout<<"> Info: Sending network map containing "<<mapElementCount<<" nodes..."<<std::endl;
		if(r=c->send((uint8	*)&mapElementCount,sizeof(uint16)))
			return	r;

		for(uint16	i=0;i<dataChannels.count();i++){
		//	if(i==_ID)
		//		continue;
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

		assignedNID = networkID->NID();
		if (assignedNID == NoID) {
			if(isTimeReference)
				assignedNID=addNodeEntry();
			else
				assignedNID=NoID;
		}

		if(!networkInterfaces[offset+_Payload::CONTROL]->canBroadcast()){

			if(r=networkInterfaces[offset+_Payload::CONTROL]->newChannel(networkID->at((InterfaceType)(offset+_Payload::CONTROL)),&ctrl_c)) {
				std::cout<<"Error: Failed to open control connection to '"<< networkID->name() <<"'..."<<std::endl;
				goto	err2;
			}
			if(r=sendID(ctrl_c,this->networkID))
				goto	err1;
			if(r=ctrl_c->send((uint8	*)&assignedNID,sizeof(uint16)))
				goto	err1;
			if(isTimeReference)
				if(r=sendMap(ctrl_c))
					goto	err1;
			// std::cout<<"Info: Opened control connection to '"<< networkID->name() <<"'..."<<std::endl;
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
				if(r=data_c->send((uint8	*)&assignedNID,sizeof(uint16)))
					goto	err0;
				if(isTimeReference)
					if(r=sendMap(data_c))
						goto	err0;
			}
			// std::cout<<"Info: Opened data connection to '"<< networkID->name() <<"'..."<<std::endl;
		}else {
		//	std::cout<<"Info: Reusing control connection for data connection to '"<< networkID->name() <<"'..."<<std::endl;
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
			// std::cout<<"Info: Opened stream connection to '"<< networkID->name() <<"'..."<<std::endl;
		}else {
		//	std::cout<<"Info: Reusing control connection for stream connection to '"<< networkID->name() <<"'..."<<std::endl;
			stream_c=data_c;
		}

		controlChannels[network][assignedNID]=ctrl_c;
		dataChannels[assignedNID]->channels[network].data=data_c;
		dataChannels[assignedNID]->channels[network].stream=stream_c;
		dataChannels[assignedNID]->networkID=networkID;
		//connectedNodeCount++;
		startReceivingThreads(assignedNID);
		notifyNodeJoined(assignedNID,networkID);

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
		return	(uint16)dataChannels.count();
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

	inline	void	Networking::_broadcastControlMessage(_Payload	*p,Network	network){

		for(uint32	i=0;i<controlChannels[network].count();i++)
			if((i != _ID) && controlChannels[network][i]	&&	controlChannels[network][i]->send(p))
				processError((uint16)i);
	}

	void	Networking::broadcastControlMessage(_Payload	*p,Network	network){

		if(network==PRIMARY	||	network==BOTH)
			_broadcastControlMessage(p,PRIMARY);
		if(network==SECONDARY	||	network==BOTH)
			_broadcastControlMessage(p,SECONDARY);
	}

	void	Networking::sendData(uint16	NID,_Payload	*p,Network	network){

		if((network==PRIMARY	||	network==BOTH)	&&	dataChannels[NID]->channels[PRIMARY].data){

			if(dataChannels[NID]->channels[PRIMARY].data->send(p))
				processError(NID);
		}else	if((network==SECONDARY	||	network==BOTH)	&&	dataChannels[NID]->channels[SECONDARY].data){
			
			if(dataChannels[NID]->channels[SECONDARY].data->send(p))
				processError(NID);
		}
	}

	void	Networking::sendStreamData(uint16	NID,_Payload	*p,Network	network){

		if((network==PRIMARY	||	network==BOTH)	&&	dataChannels[NID]->channels[PRIMARY].stream){

			if(dataChannels[NID]->channels[PRIMARY].stream->send(p))
				processError(NID);
		}else	if((network==SECONDARY	||	network==BOTH)	&&	dataChannels[NID]->channels[SECONDARY].stream){
			
			if(dataChannels[NID]->channels[SECONDARY].stream->send(p))
				processError(NID);
		}
	}

	inline	void	Networking::processError(uint16	entry){

		channelsCS.enter();

		notifyNodeLeft(entry);

		if (controlChannels[PRIMARY].count() > entry) {

			if(controlChannels[PRIMARY][entry])
				delete	controlChannels[PRIMARY][entry];
			controlChannels[PRIMARY][entry]=NULL;
		}
		if(dataChannels[entry]->channels[PRIMARY].data) {
			delete	dataChannels[entry]->channels[PRIMARY].data;
			dataChannels[entry]->channels[PRIMARY].data = NULL;
		}
		dataChannels[entry]->channels[PRIMARY].data=NULL;
		if(dataChannels[entry]->channels[PRIMARY].stream) {
			delete	dataChannels[entry]->channels[PRIMARY].stream;
			dataChannels[entry]->channels[PRIMARY].stream = NULL;
		}
		dataChannels[entry]->channels[PRIMARY].stream=NULL;

		if (controlChannels[SECONDARY].count() > entry) {

			if(controlChannels[SECONDARY][entry])
				delete	controlChannels[SECONDARY][entry];
			controlChannels[SECONDARY][entry]=NULL;
		}
		if(dataChannels[entry]->channels[SECONDARY].data) {
			delete	dataChannels[entry]->channels[SECONDARY].data;
			dataChannels[entry]->channels[SECONDARY].data = NULL;
		}
		dataChannels[entry]->channels[SECONDARY].data=NULL;
		if(dataChannels[entry]->channels[SECONDARY].stream) {
			delete	dataChannels[entry]->channels[SECONDARY].stream;
			dataChannels[entry]->channels[SECONDARY].stream = NULL;
		}
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

		for(uint32	i=0;i<commThreads.count();i++)
			Thread::TerminateAndWait(*commThreads.get(i));
		stopInterfaces();
	}

	////////////////////////////////////////////////////////////////////////////////////////////////

	thread_ret thread_function_call	Networking::AcceptConnections(void	*args){

		AcceptConnectionArgs		*acargs = (AcceptConnectionArgs *)args;
		Networking			*node=acargs->node;
		Network				network=acargs->network;
		uint8	offset=network*3;
		_Payload::Category	category=acargs->category;
		int32				timeout=acargs->timeout;
		NetworkInterface	*networkInterface=node->networkInterfaces[offset+category];

		uint16	r;

	//	if (category == _Payload::CONTROL)
	//		std::cout<<"Info: Waiting for "<< (uint32)networkInterface->protocol()<<" control connection..."<<std::endl;
	//	else if (category == _Payload::DATA)
	//		std::cout<<"Info: Waiting for "<< (uint32)networkInterface->protocol()<<" data connection..."<<std::endl;
	//	else if (category == _Payload::STREAM)
	//		std::cout<<"Info: Waiting for "<< (uint32)networkInterface->protocol()<<" stream connection..."<<std::endl;

		ConnectedCommChannel	*c;
		NetworkID	*networkID;
		uint16	assignedNID;
		bool	timedout;
		bool decidedRefNode = false;
		while(!node->_shutdown){

			if(r=networkInterface->acceptConnection(&c,timeout,timedout)) {
				std::cout<<"Error: Networking Interface acceptConnection"<<std::endl;
				goto	err1;
			}

			// ######### if we already have received a connection, don't startup as ref node

			node->acceptConnectionCS.enter();

			if ( (!decidedRefNode) && (category==_Payload::CONTROL) && (timedout) ) {	//	reference node
				std::cout<<"> Info: Starting up as Reference Node..."<<std::endl;
				node->start(0,0,true);
				node->acceptConnectionCS.leave();
				thread_ret_val(0);
			}
			//std::cout<<"Info: Processing acceptConnection for protocol "<< (uint32)networkInterface->protocol()<<"..."<<std::endl;
			
			//	non reference nodes
			decidedRefNode = true;
			if(category==_Payload::CONTROL	||	(category==_Payload::DATA	&&	node->networkInterfaces[offset+_Payload::CONTROL]->canBroadcast())){

				// uint16	assignedNID;
			//	std::cout<<"Info: Receiving incoming control connection..."<<std::endl;
				if(r=node->recvID(c,networkID))
					goto	err0;
			//	std::cout<<"Info: Preparing to receive assigned NodeID..."<<std::endl;
				if(r=c->recv((uint8	*)&assignedNID,sizeof(uint16)))
					goto	err0;
				if(assignedNID!=NoID){

			//		std::cout<<"Info: Got assigned NodeID ["<<assignedNID<<"]..."<<std::endl;
					node->start(assignedNID,networkID,false);
					std::cout<<"> Info: My NodeID is now ["<<networkID->NID()<<"]..."<<std::endl;
					if(r=node->recvMap(c))
						goto	err0;
				}
			}
			else if ( (category==_Payload::DATA) || (category==_Payload::STREAM) ) {
				// Just receive the remote NetworkID
				if(r=node->recvID(c,networkID))
					goto	err0;
			}

//			uint16	remoteNID=((uint16	*)networkID)[0];
			uint16	remoteNID=networkID->NID();
			node->dataChannels[remoteNID]->networkID=networkID;

			bool	start=false;
			switch(category){
			case	_Payload::CONTROL:
				node->controlChannels[network][remoteNID]=c;
				if(node->dataChannels[remoteNID]->channels[network].data	&&	node->dataChannels[remoteNID]->channels[network].stream) {
					start=true;
				}
				break;
			case	_Payload::DATA:
				node->dataChannels[remoteNID]->channels[network].data=c;
				if(node->dataChannels[remoteNID]->channels[network].stream	&&	(node->networkInterfaces[offset+_Payload::CONTROL]->canBroadcast()	||	(!node->networkInterfaces[offset+_Payload::CONTROL]->canBroadcast()	&&	node->controlChannels[network][remoteNID]))) {
					start=true;
				}
				break;
			case	_Payload::STREAM:
				node->dataChannels[remoteNID]->channels[network].stream=c;
				if(node->dataChannels[remoteNID]->channels[network].data	&&	(node->networkInterfaces[offset+_Payload::CONTROL]->canBroadcast()	||	(!node->networkInterfaces[offset+_Payload::CONTROL]->canBroadcast()	&&	node->controlChannels[network][remoteNID]))) {
					//start=true;
				}
				break;
			default:
				break;
			}
			if(start){
				node->connectedNodeCount++;
				node->startReceivingThreads(remoteNID);
				node->notifyNodeJoined(remoteNID,node->dataChannels[remoteNID]->networkID);
			}
			node->acceptConnectionCS.leave();
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
					std::cout<<"> Info: Received invalid network greeting with empty name, ignoring..."<<std::endl;
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

		Networking	*node=(Networking	*)args;

		SyncProbe	*probe;
		while(!node->_shutdown){

			if (node->dataChannels[node->referenceNID] != NULL) {
				probe=new	SyncProbe();
				probe->node_id=node->networkID->NID();
				std::cout<<"> Info: Sending SyncProbe type '"<<probe->CID()<<"' ("<<probe->node_id<<")..."<<std::endl;
				((_Payload*)probe)->node_send_ts() = Time::Get(); // this needs local time, not adjusted time
				switch(node->network){
				case	PRIMARY:
				case	BOTH:
					if (node->dataChannels[node->referenceNID]->channels[PRIMARY].data)
						node->dataChannels[node->referenceNID]->channels[PRIMARY].data->send(probe);
					break;
				case	SECONDARY:
					if (node->dataChannels[node->referenceNID]->channels[SECONDARY].data)
						node->dataChannels[node->referenceNID]->channels[SECONDARY].data->send(probe);
					break;
				}
			}
			Thread::Sleep(node->syncPeriod);
		}
		return	0;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////

	Networking::DataCommChannel::DataCommChannel():networkID(NULL){

		channels[PRIMARY].data=channels[PRIMARY].stream=NULL;
		channels[SECONDARY].data=channels[SECONDARY].stream=NULL;
	}

	Networking::DataCommChannel::~DataCommChannel(){

	// Deleted elsewhere
		//if(channels[PRIMARY].data) {
		//	delete	channels[PRIMARY].data;
		//	channels[PRIMARY].data = NULL;
		//}
		//if(channels[PRIMARY].stream) {
		//	delete	channels[PRIMARY].stream;
		//	channels[PRIMARY].stream = NULL;
		//}
		//if(channels[SECONDARY].data) {
		//	delete	channels[SECONDARY].data;
		//	channels[SECONDARY].data = NULL;
		//}
		//if(channels[SECONDARY].stream) {
		//	delete	channels[SECONDARY].stream;
		//	channels[SECONDARY].stream = NULL;
		//}
	//	if(networkID)
	//		delete	networkID;
	}
}
