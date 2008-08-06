// networking.cpp
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

#include	"networking.h"

#include	"..\Core\control_messages.h"


namespace	mBrane{

	Networking::Networking():Node(),isTimeReference(false),timeDrift(0),networkID(NULL){

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
			delete[]	networkID;
	}

	bool	Networking::loadInterface(XMLNode	&n,const	char	*name,NetworkInterfaceType	type){

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
		if(!loadInterface(network,"Control",CONTROL))
			return	false;
		if(!loadInterface(network,"Data",DATA))
			return	false;
		if(!loadInterface(network,"Stream",STREAM))
			return	false;

		const	char	*sp=network.getAttribute("sync_period");
		if(!sp){

			std::cout<<"Error: NodeConfiguration::Network::sync_period is missing\n";
			return	NULL;
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

		return	true;
	}

	bool	Networking::init(){

		if(!startInterfaces())
			return	false;

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
				return	false;
			controlChannels[0]=ctrl_c;
		}

		if(networkInterfaces[DISCOVERY]->broadcastID(networkID,network_ID_size))
			return	false;

		if(isTimeReference){

			start(0,0,true);
			return	true;
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
		
		return	true;
	}

	void	Networking::start(uint16	assignedNID,uint16	remoteNID,bool	isTimeReference){

		this->isTimeReference=isTimeReference;
		dataChannels[assignedNID];	//	alloc and reserve for oneself
		_ID=assignedNID;
		referenceNID=remoteNID;

		commThreads[commThreads.count()]=Thread::New(ScanIDs,this);

		if(isTimeReference)
			commThreads[commThreads.count()]=Thread::New(Sync,this);
	}

	bool	Networking::startInterfaces(){

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

	void	Networking::stopInterfaces(){

		networkInterfaces[DISCOVERY]->stop();
		networkInterfaces[CONTROL]->stop();
		if(networkInterfaces[DATA]->operator !=(networkInterfaces[CONTROL]))
			networkInterfaces[DATA]->stop();
		if(networkInterfaces[STREAM]->operator !=(networkInterfaces[DATA]))
			networkInterfaces[STREAM]->stop();
	}

	uint16	Networking::sendID(ConnectedCommChannel	*c,uint16	assignedNID){

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

	uint16	Networking::recvID(ConnectedCommChannel	*c,uint16	&NID,char	*&name,uint8	&nameSize,uint16	&assignedNID){

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
				commThreads[commThreads.count()]=Thread::New(Sync,this);
				return;
			}
		}
	}

	void	Networking::processError(NetworkInterfaceType	type,uint16	entry){

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

		if(entry==referenceNID)
			setNewReference();

		channelsCS.leave();
	}

	void	Networking::shutdown(){

		Thread::Wait(commThreads.data(),commThreads.count());
		stopInterfaces();
	}

	////////////////////////////////////////////////////////////////////////////////////////////////

	uint32	thread_function_call	Networking::ScanIDs(void	*args){

		Networking	*node=(Networking	*)args;

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

	uint32	thread_function_call	Networking::AcceptConnections(void	*args){

		Networking				*node=((AcceptConnectionArgs	*)args)->n;
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

				node->start(0,0,true);
				return	0;
			}
			
			uint16	assignedNID;
			if(r=node->recvID(c,remoteNID,remoteName,remoteNameSize,assignedNID))
				goto	err0;
			if(assignedNID!=NO_ID)
				node->start(assignedNID,remoteNID,false);

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

					if(node->controlChannels[i]	&&	node->controlChannels[i]->send(&sync))
						node->processError(CONTROL,i);
				}
				node->lastSyncTime=t;
			}
		}

		return	0;
	}
}