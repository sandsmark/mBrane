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

		networkInterfaceLoaders[DISCOVERY]=NULL;
		networkInterfaceLoaders[CONTROL]=NULL;
		networkInterfaceLoaders[DATA]=NULL;
		networkInterfaceLoaders[STREAM]=NULL;

		networkInterfaces[DISCOVERY]=NULL;
		networkInterfaces[CONTROL]=NULL;
		networkInterfaces[DATA]=NULL;
		networkInterfaces[STREAM]=NULL;
	}

	Networking::~Networking(){

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
		if(!loadInterface(network,"Control",CONTROL))
			return	false;
		if(!loadInterface(network,"Data",DATA))
			return	false;
		if(!loadInterface(network,"Stream",STREAM))
			return	false;

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
		if(!networkInterfaces[CONTROL]->start())
			return	false;
		if(*networkInterfaces[DATA]!=*networkInterfaces[CONTROL])
			if(!networkInterfaces[DATA]->start())
				return	false;
		if(*networkInterfaces[STREAM]!=*networkInterfaces[DATA])
			if(!networkInterfaces[STREAM]->start())
				return	false;
		return	true;
	}

	void	Networking::stopInterfaces(){

		networkInterfaces[DISCOVERY]->stop();
		networkInterfaces[CONTROL]->stop();
		if(*networkInterfaces[DATA]!=*networkInterfaces[CONTROL])
			networkInterfaces[DATA]->stop();
		if(*networkInterfaces[STREAM]!=*networkInterfaces[DATA])
			networkInterfaces[STREAM]->stop();
	}

	bool	Networking::init(){

		if(!startInterfaces())
			return	false;

		NetworkID::CtrlIDSize=networkInterfaces[CONTROL]->getIDSize();
		NetworkID::DataIDSize=networkInterfaces[DATA]->getIDSize();
		NetworkID::StreamIDSize=networkInterfaces[STREAM]->getIDSize();
		NetworkID::DiscoveryIDSize=networkInterfaces[DISCOVERY]->getIDSize();
		NetworkID::Size=NetworkID::CtrlIDSize+NetworkID::DataIDSize+NetworkID::StreamIDSize+NetworkID::DiscoveryIDSize;

		networkID=new	NetworkID(_ID,hostNameSize,hostName);

		networkInterfaces[CONTROL]->fillID(networkID->at(CONTROL));
		networkInterfaces[DATA]->fillID(networkID->at(DATA));
		networkInterfaces[STREAM]->fillID(networkID->at(STREAM));
		networkInterfaces[DISCOVERY]->fillID(networkID->at(DISCOVERY));

		if(networkInterfaces[DISCOVERY]->newChannel(networkID->at(DISCOVERY),&discoveryChannel))
			return	false;

		if(networkInterfaces[CONTROL]->canBroadcast()){
		
			if(networkInterfaces[CONTROL]->newChannel(networkID->at(CONTROL),&(controlChannels[0])))
				return	false;
		}

		AcceptConnectionArgs	args;
		args.node=this;
		args.timeout=bcastTimeout;
		if(!networkInterfaces[CONTROL]->canBroadcast()){

			args.type=CONTROL;
			commThreads[commThreads.count()]=Thread::New<Thread>(AcceptConnections,&args);
			args.timeout=-1;
		}
		if(*networkInterfaces[DATA]!=*networkInterfaces[CONTROL]){
		
			args.type=DATA;
			commThreads[commThreads.count()]=Thread::New<Thread>(AcceptConnections,&args);
			args.timeout=-1;
		}
		if(*networkInterfaces[STREAM]!=*networkInterfaces[DATA]){
		
			args.type=STREAM;
			commThreads[commThreads.count()]=Thread::New<Thread>(AcceptConnections,&args);
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
		uint8	remoteNameSize;
		if(r=c->recv((uint8	*)&remoteNameSize,sizeof(uint8)))
			return	r;
		char	*remoteName=new	char[remoteNameSize];
		if(r=c->recv((uint8	*)remoteName,remoteNameSize)){

			delete[]	remoteName;
			return	r;
		}
		networkID=new	NetworkID(remoteNID,remoteNameSize,remoteName);
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

	uint16	Networking::connect(NetworkID	*networkID){

		uint16	r;
		CommChannel	*ctrl_c=NULL;
		CommChannel	*data_c;
		CommChannel	*stream_c;
		uint16	assignedNID;

		if(isTimeReference)
			assignedNID=addNodeEntry();
		else
			assignedNID=NO_ID;

		if(!networkInterfaces[CONTROL]->canBroadcast()){

			if(r=networkInterfaces[CONTROL]->newChannel(networkID->at(CONTROL),&ctrl_c))
				goto	err2;
			if(r=sendID(ctrl_c,this->networkID))
				goto	err1;
			if(r=ctrl_c->send((uint8	*)&assignedNID,sizeof(uint16)))
				goto	err1;
			if(isTimeReference)
				if(r=sendMap(ctrl_c))
					goto	err1;
		}
			
		if(*networkInterfaces[DATA]!=*networkInterfaces[CONTROL]){

			if(r=networkInterfaces[DATA]->newChannel(networkID->at(DATA),&data_c))
				goto	err1;
			if(networkInterfaces[CONTROL]->canBroadcast()){

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

		if(*networkInterfaces[STREAM]!=*networkInterfaces[DATA]){

			if(r=networkInterfaces[STREAM]->newChannel(networkID->at(STREAM),&stream_c))
				goto	err0;
		}else
			stream_c=data_c;

		controlChannels[assignedNID]=ctrl_c;
		dataChannels[assignedNID]->data=data_c;
		dataChannels[assignedNID]->stream=stream_c;
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

	uint16	Networking::addNodeEntry(){	//	assigns the first free slot

		channelsCS.enter();
		for(uint16	i=0;i<dataChannels.count();i++){

			if(i==_ID)
				continue;
			if(!dataChannels[i]->data){

				channelsCS.leave();
				return	i;
			}
		}
		channelsCS.leave();
		return	dataChannels.count();
	}

	void	Networking::setNewReference(){	//	elect the first node in the list

		for(uint16	i=0;i<dataChannels.count();i++){

			if(dataChannels[i]->data	&&	i==_ID){

				isTimeReference=true;
				commThreads[commThreads.count()]=Thread::New<Thread>(ScanIDs,this);
				commThreads[commThreads.count()]=Thread::New<Thread>(Sync,this);
				return;
			}
		}
	}

	inline	void	Networking::processError(InterfaceType	type,uint16	entry){

		channelsCS.enter();

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
		InterfaceType		type=((AcceptConnectionArgs	*)args)->type;
		int32				timeout=((AcceptConnectionArgs	*)args)->timeout;
		NetworkInterface	*networkInterface=node->networkInterfaces[type];

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
			if(type==CONTROL	||	(type==DATA	&&	node->networkInterfaces[CONTROL]->canBroadcast())){

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
		sync.senderNode_id()=node->_ID;
		while(!node->_shutdown){

			Thread::Sleep(node->syncPeriod);

			int64	t=node->time();
			if(t-node->lastSyncTime>=node->syncPeriod){

				sync.send_ts()=Time::Get();
				for(uint16	i=0;i<node->controlChannels.count();i++){

					if(node->controlChannels[i]	&&	node->controlChannels[i]->send((&sync)->operator	_Payload	*()))
						node->processError(CONTROL,i);
				}
				node->lastSyncTime=t;
			}
		}
		return	0;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////

	Networking::DataCommChannel::DataCommChannel():data(NULL),stream(NULL),networkID(NULL){
	}

	Networking::DataCommChannel::~DataCommChannel(){

		if(data)
			delete	data;
		if(stream)
			delete	stream;
		if(networkID)
			delete	networkID;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////

	uint16	Networking::NetworkID::Size;
	
	uint16	Networking::NetworkID::CtrlIDSize;
	
	uint16	Networking::NetworkID::DataIDSize;
	
	uint16	Networking::NetworkID::StreamIDSize;

	uint16	Networking::NetworkID::DiscoveryIDSize;
	
	Networking::NetworkID::NetworkID():data(NULL){
	}

	Networking::NetworkID::NetworkID(uint16	NID,uint8	nameSize,char	*name){

		headerSize=sizeof(uint16)+sizeof(uint8)+nameSize;
		data=new	uint8[headerSize+Size];
		((uint16	*)data)[0]=NID;
		data[1]=nameSize;
		memcpy(data+sizeof(uint16)+sizeof(uint8),name,data[1]);
	}

	Networking::NetworkID::~NetworkID(){

		if(data)
			delete	data;
	}

	uint16	Networking::NetworkID::NID()	const{

		return	((uint16	*)data)[0];
	}
	
	char	*Networking::NetworkID::name()	const{

		return	(char	*)(data+sizeof(uint16)+sizeof(uint8));
	}

	uint8	*Networking::NetworkID::at(InterfaceType	t)	const{

		switch(t){
		case	CONTROL:	return	data+headerSize;
		case	DATA:		return	data+headerSize+CtrlIDSize;
		case	STREAM:		return	data+headerSize+CtrlIDSize+DataIDSize;
		case	DISCOVERY:	return	data+headerSize+CtrlIDSize+DataIDSize+StreamIDSize;
		}
	}
}