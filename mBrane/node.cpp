
//	node.cpp
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

#include	"node.h"
#include	"projection.tpl.cpp"
#include	"../Core/class_register.h"
#include	"../Core/module_register.h"
#include	"../Core/control_messages.h"

#include	<iostream>


using	namespace	mBrane::sdk::payloads;

namespace	mBrane{

	Node	*Node::New(const	char	*configFileName,SharedLibrary	&userLibrary,uint8	traceLevels){

		Node	*n=new	Node(traceLevels);
		if(n->loadConfig(configFileName)){

			userLibrary=*n->userLibrary;
			return	n;
		}
		return	NULL;
	}

	Node::Node(uint8	traceLevels):Networking(),Messaging(),Executing(),nodeCount(0),nodeJoined(0){

		if(!(traceLevels	&	0x01))
			Streams[0]=new	NoStream();
		if(!(traceLevels	&	0x02))
			Streams[1]=new	NoStream();
		if(!(traceLevels	&	0x04))
			Streams[2]=new	NoStream();
	}

	Node::~Node(){

		ClassRegister::Cleanup();
		ModuleRegister::Cleanup();
		if(Streams[0])
			delete	Streams[0];
		if(Streams[1])
			delete	Streams[1];
		if(Streams[2])
			delete	Streams[2];
	}

	Node	*Node::loadConfig(const	char	*configFileName){

		hostNameSize=Host::Name(hostName);

		std::cout<<std::endl<<"--------- mBrane v"<<MBRANE_VERSION<<" Node: "<<hostName<<" ---------"<<std::endl<<std::endl;

		XMLNode	mainNode=XMLNode::openFileHelper(configFileName,"NodeConfiguration");
		if(!mainNode){

			std::cout<<"> Error: NodeConfiguration corrupted"<<std::endl;
			return	NULL;
		}
		
		if(!Networking::loadConfig(mainNode))
			return	NULL;

		if(!Executing::loadConfig(mainNode))
			return	NULL;

		if(!mdaemon::Node::loadConfig(mainNode))
			return	NULL;

		uint16 entry = 0;
		XMLNode	nodeList=mainNode.getChildNode("Nodes");
		if(!nodeList)
			nodeCount=0;
		else{

			// First, add yourself to the list, in case it is not in the list
			strcpy(nodeNames[0],hostName);
			nodeStatus[0] = 2; // Do not wait for connection...
		//	printf("--- Node 0: '%s' ---\n", hostName);
			nodeCount=nodeList.nChildNode("Node");
			for(uint16	i=0;i<nodeCount;i++){

				XMLNode	n=nodeList.getChildNode(i);
				const	char	*_n=n.getAttribute("hostname");
				if(!_n){

					std::cout<<"> Error: NodeConfiguration::Nodes::node_"<<i<<"::hostname is missing"<<std::endl;
					strcpy(nodeNames[i],"");
					nodeStatus[i] = -1;
					return	NULL;
				}
				if(	!stricmp(_n, hostName) == 0) {
					entry++;
					strcpy(nodeNames[entry],_n);
				//	printf("--- Node %u: '%s' ---\n", entry, _n);
					nodeStatus[entry] = 0; // awaiting joining...
				}
			}
		}

		//	FUTURE DEVELOPMENT: load RDMA library, alloc sharedMemorySegments and pin them down. See test_node.xml

		const	char	*application_configuration_file=mainNode.getAttribute("application_configuration_file");
		if(!application_configuration_file){

			std::cout<<"> Error: NodeConfiguration::application_configuration_file is missing"<<std::endl;
			return	NULL;
		}

		if(!loadApplication(application_configuration_file))
			return	NULL;

		return	this;
	}

	bool	Node::loadApplication(const	char	*fileName){

		Space::InitRoot();

		XMLNode	mainNode=XMLNode::openFileHelper(fileName,"ApplicationConfiguration");
		if(!mainNode){

			std::cout<<"> Error: ApplicationConfiguration corrupted"<<std::endl;
			return	false;
		}
		const	char	*ul=mainNode.getAttribute("user_library");
		if(!ul){

			std::cout<<"> Error: ApplicationConfiguration::user_library is missing"<<std::endl;
			return	false;
		}
		if(!(userLibrary=SharedLibrary::New(ul)))
			return	false;

		typedef	void	(*UserInitFunction)();
		UserInitFunction	userInitFunction=userLibrary->getFunction<UserInitFunction>("Init");
		if(!userInitFunction)
			return	false;
		userInitFunction();
		std::cout<<"> User library "<<ul<<" loaded"<<std::endl;

		uint16	spaceCount=mainNode.nChildNode("Space");
		for(uint16	i=0;i<spaceCount;i++){

			XMLNode	spaceNode=mainNode.getChildNode("Space",i);
			Space	*s=Space::New(spaceNode);
			if(!s)
				return	false;
		}

		uint16	moduleCount=mainNode.nChildNode("Module");
		for(uint16	i=0;i<moduleCount;i++){

			XMLNode				moduleNode=mainNode.getChildNode("Module",i);
			ModuleDescriptor	*m=ModuleDescriptor::New(moduleNode);
			if(!m)
				return	false;
		}

		return	true;
	}

	uint16	Node::getNID(const	char	*name){

		if(stricmp(name,"local")==0)
			return	_ID;
		for(uint16	i=0;i<nodeNames.count();i++)
			if(stricmp(nodeNames[i],networkID->name())==0)
				return	i;
		return	NoID;
	}

	bool	Node::allNodesJoined() {
		// don't check reference node = 0
		for(uint16	i=1;i<nodeNames.count();i++) {
			if (nodeStatus[i] < 1) {
				printf("*** Still waiting for Node '%s' (%u of %u) to join ***\n", nodeNames[i], i, nodeNames.count());
				return false;
			}
		}
		printf("All %u Nodes joined\n", nodeNames.count());
		return	true;
	}

	bool	Node::allNodesReady() {
		// don't check reference node = 0
		for(uint16	i=1;i<nodeNames.count();i++) {
			if (nodeStatus[i] != 2) {
				printf("*** Still waiting for Node '%s' (%u of %u) to get ready ***\n", nodeNames[i], i, nodeNames.count());
				return false;
			}
		}
		printf("All %u Nodes ready\n", nodeNames.count());
		return	true;
	}

	bool	Node::checkSyncProbe(uint16 syncNodeID) {
		if (nodeStatus[syncNodeID] != 2) {
			nodeStatus[syncNodeID] = 2;
			// printf("Node '%s' set to ready... \n", nodeNames[syncNodeID]);
			if (allNodesReady()) {
				// printf("SystemReady... \n");
				Node::systemReady();
			}
			else {
				// printf("Not sending SystemReady... \n");
			}
		}
		return	true;
	}

	const	char	*Node::name(){

		return	hostName;
	}

	void	Node::run(){

		if(!Networking::init()){

			shutdown();
			return;
		}

		// std::cout<<"> Running"<<std::endl;

		Thread::Sleep();
	}

	void	Node::start(uint16	assignedNID,NetworkID	*networkID,bool	isTimeReference){

		// we only need to do this once.

		static bool started = false;

		if (started)
			return;

		started = true;

		// std::cout << "Debug: node starting up ... " << std::endl;

		Networking::start(assignedNID,networkID,isTimeReference);

		uint32 recThreadCount = 0;
		if(network==PRIMARY	||	network==BOTH){

			if(networkInterfaces[CONTROL_PRIMARY]->canBroadcast()){

				recThreadCount = recvThreads.count();
				RecvThread	*t=new	RecvThread(this,controlChannels[PRIMARY][0],0);
				recvThreads[recThreadCount]=t;
				t->start(RecvThread::ReceiveMessages);
				pushThreads[recThreadCount+1]=new	PushThread((Node*)this,&t->buffer);
				pushThreads[recThreadCount+1]->start(PushThread::PushJobs);
			}
		}
		if(network==SECONDARY	||	network==BOTH){

			if(networkInterfaces[CONTROL_SECONDARY]->canBroadcast()){

				recThreadCount = recvThreads.count();
				RecvThread	*t=new	RecvThread(this,controlChannels[SECONDARY][0],0);
				recvThreads[recThreadCount]=t;
				t->start(RecvThread::ReceiveMessages);
				pushThreads[recThreadCount+1]=new	PushThread((Node*)this,&t->buffer);
				pushThreads[recThreadCount+1]->start(PushThread::PushJobs);
			}
		}

		Messaging::start();
		mdaemon::Node::start();
		Executing::start();

		if(bootCallback)
			bootCallback();

		/* if we only have a single node; kick things into gear
		 * when we have more than one, this should be handled by the
		 * other parts of the networking code
		 */
		if (nodeCount == 0) {
			Node::ready();
			Node::systemReady();
		}

	}

	void	Node::ready(void) {

		static bool initialised = false;

		if (initialised)
			return;

		printf("\n\n*** NODE READY ***\n\n");
		Space::Init(_ID);
		ModuleDescriptor::Init(_ID);
		Networking::startSync();
		initialised = true;
	}

	void	Node::systemReady(void) {

		if ( isTimeReference ) {

			printf("\n\n*** SYSTEM READY ***\n\n");

			SystemReady	*m = new SystemReady();

			m->send_ts() = this->time();
			Messaging::send(m, BOTH);

		}
	}

	void	Node::notifyNodeJoined(uint16	NID,NetworkID	*networkID){

		// static	uint16	ToJoin=nodeCount; // using nodeJoined instead

		// if(nodeCount && (nodeJoined == nodeCount)){	//	a node is joining after startup
		if (allNodesJoined()) {

			//	TODO: update NodeEntries. See Node::notifyNodeLeft()
		}else {	//	a node is joining during startup

			bool alreadyJoined = false;
			for(uint16	i=0;i<nodeNames.count();i++) {
				// printf("Node join check '%s' == '%s'... \n", nodeNames[i],networkID->name());
				if(stricmp(nodeNames[i],networkID->name())==0){
				//	Node::Get()->trace(Node::NETWORK)<<"> Node join status: "<<networkID->name()<<": '"<<nodeStatus[i]<<"'"<<std::endl;
					if (nodeStatus[i] >= 1)
						alreadyJoined = true;
					else
						nodeStatus[i] = 1; // now joined
					break;
				}
			}

			if (!alreadyJoined) {
				Node::Get()->trace(Node::NETWORK)<<"> Node join init: "<<networkID->name()<<":"<<NID<<std::endl;
				Space::Init(NID);

				for(uint32	i=0;i<ModuleDescriptor::Config.count();i++)	{ //	resolve host names into NID
					for(uint32	j=0;j<ModuleDescriptor::Config[i].count();j++){

						if(stricmp(ModuleDescriptor::Config[i][j]->hostName,networkID->name())==0){

							ModuleDescriptor::Main[NID][i][j]=ModuleDescriptor::Config[i][j];
							ModuleDescriptor::Main[NID][i][j]->hostID=NID;
							ModuleDescriptor::Main[NID][i][j]->applyInitialProjections(NID);
						}
					}
				}

			}
		}

		if (allNodesJoined()) {
			Node::ready();
		}

		NodeJoined	*m=new	NodeJoined();
		m->node_id=NID;
		m->send_ts()=this->time();
		Messaging::send(m,LOCAL);

		Node::Get()->trace(Node::NETWORK)<<"> Node joined: "<<networkID->name()<<":"<<NID<<std::endl;
	}

	void	Node::notifyNodeLeft(uint16	NID){

		if(	controlChannels[PRIMARY][NID]				||
			dataChannels[NID]->channels[PRIMARY].data	||	
			dataChannels[NID]->channels[PRIMARY].stream	||
			controlChannels[SECONDARY][NID]				||
			dataChannels[NID]->channels[SECONDARY].data	||	
			dataChannels[NID]->channels[SECONDARY].stream){

			//	TODO: update NodeEntries. Implies defining a policy for node ressucitation.

			NodeLeft	*m=new	NodeLeft();
			m->node_id=NID;
			m->send_ts()=this->time();
			Messaging::send(m,LOCAL);

			if (dataChannels[NID]->networkID)
				Node::Get()->trace(Node::NETWORK)<<"> Node left: "<<dataChannels[NID]->networkID->name()<<":"<<NID<<std::endl;
		}
	}

	void	Node::startReceivingThreads(uint16	NID){

		uint32 recThreadCount = 0;
		if(network==PRIMARY	||	network==BOTH){

			if(!networkInterfaces[CONTROL_PRIMARY]->canBroadcast()){

				recThreadCount = recvThreads.count();
				RecvThread	*t=new	RecvThread(this,controlChannels[PRIMARY][NID],NID);
				recvThreads[recThreadCount]=t;
				t->start(RecvThread::ReceiveMessages);
				pushThreads[recThreadCount+1]=new	PushThread((Node*)this,&t->buffer);
				pushThreads[recThreadCount+1]->start(PushThread::PushJobs);
			}

			if(networkInterfaces[DATA_PRIMARY]!=networkInterfaces[CONTROL_PRIMARY]){

				recThreadCount = recvThreads.count();
				RecvThread	*t=new	RecvThread(this,dataChannels[NID]->channels[PRIMARY].data,NID);
				recvThreads[recThreadCount]=t;
				t->start(RecvThread::ReceiveMessages);
				pushThreads[recThreadCount+1]=new	PushThread((Node*)this,&t->buffer);
				pushThreads[recThreadCount+1]->start(PushThread::PushJobs);
			}

			if(networkInterfaces[STREAM_PRIMARY]!=networkInterfaces[DATA_PRIMARY]){

				recThreadCount = recvThreads.count();
				RecvThread	*t=new	RecvThread(this,dataChannels[NID]->channels[PRIMARY].stream,NID);
				recvThreads[recThreadCount]=t;
				t->start(RecvThread::ReceiveMessages);
				pushThreads[recThreadCount+1]=new	PushThread((Node*)this,&t->buffer);
				pushThreads[recThreadCount+1]->start(PushThread::PushJobs);
			}
		}

		if(network==SECONDARY	||	network==BOTH){

			if(!networkInterfaces[CONTROL_SECONDARY]->canBroadcast()){

				recThreadCount = recvThreads.count();
				RecvThread	*t=new	RecvThread(this,controlChannels[SECONDARY][NID],NID);
				recvThreads[recThreadCount]=t;
				t->start(RecvThread::ReceiveMessages);
				pushThreads[recThreadCount+1]=new	PushThread((Node*)this,&t->buffer);
				pushThreads[recThreadCount+1]->start(PushThread::PushJobs);
			}

			if(networkInterfaces[DATA_SECONDARY]!=networkInterfaces[CONTROL_SECONDARY]){

				recThreadCount = recvThreads.count();
				RecvThread	*t=new	RecvThread(this,dataChannels[NID]->channels[SECONDARY].data,NID);
				recvThreads[recThreadCount]=t;
				t->start(RecvThread::ReceiveMessages);
				pushThreads[recThreadCount+1]=new	PushThread((Node*)this,&t->buffer);
				pushThreads[recThreadCount+1]->start(PushThread::PushJobs);
			}

			if(networkInterfaces[STREAM_SECONDARY]!=networkInterfaces[DATA_SECONDARY]){

				recThreadCount = recvThreads.count();
				RecvThread	*t=new	RecvThread(this,dataChannels[NID]->channels[SECONDARY].stream,NID);
				recvThreads[recThreadCount]=t;
				t->start(RecvThread::ReceiveMessages);
				pushThreads[recThreadCount+1]=new	PushThread((Node*)this,&t->buffer);
				pushThreads[recThreadCount+1]->start(PushThread::PushJobs);
			}
		}
	}

	void	Node::shutdown(){

		if(_shutdown)
			return;
		_shutdown=true;
		std::cout<<"> Shutting down..."<<std::endl;
		std::cout<<"> Shutting down Messaging..."<<std::endl;
		Messaging::shutdown();
		std::cout<<"> Shutting down Executing..."<<std::endl;
		Executing::shutdown();
		std::cout<<"> Shutting down Node..."<<std::endl;
		mdaemon::Node::shutdown();
		std::cout<<"> Shutting down Networking..."<<std::endl;
		Networking::shutdown();
		std::cout<<"> Shutting down Application..."<<std::endl;
//		unloadApplication();
	}

	inline	int64	Node::time()	const{

		if(isTimeReference)
			return	Time::Get();
		else
			return	Time::Get()-timeDrift;
	}

	void	Node::send(const	_Module	*sender,_Payload	*message, Array<uint16, 128>	*nodeIDs,Network	network){

		for (uint16 n=0; n<nodeIDs->count(); n++) {
			send(sender, message, *nodeIDs->get(n), network);
		}
	}

	void	Node::send(const	_Module	*sender,_Payload	*message, uint16	nodeID,Network	network){

		message->send_ts()=this->time();
		_Payload::Category	cat= message->category();

		MessageSlot	out;
		out.p=message;
		out.network=network;

		if(out.network==module::Node::LOCAL){
				
			message->node_recv_ts()=this->time();
			this->messageInputQueue.push(out.p);
		}else{

			switch(cat){
				case	_Payload::CONTROL:
					this->broadcastControlMessage(message,out.network);
					message->node_recv_ts()=this->time();
					this->messageInputQueue.push(out.p);
					break;
				case	_Payload::STREAM:	//	find target remote nodes; send on data/stream channels; push in messageInputQueue if the local node is a target
					{
					if(nodeID==this->_ID) {
					//	printf("Sending message (%u) as stream locally...\n", p->cid());
						message->node_recv_ts()=this->time();
						this->messageInputQueue.push(out.p);
					}
					else {
					//	printf("Sending message (%u) as stream network...\n", p->cid());
						this->sendStreamData(nodeID,message,out.network);
					}
					}break;
				case	_Payload::DATA:
					{
					_Message	*_m=message->as_Message();
					_m->senderModule_cid()=sender->descriptor->CID;
					_m->senderModule_id()=sender->descriptor->ID;
					_m->senderNodeID()=sender->descriptor->hostID;
					if(nodeID==this->_ID) {
					//	printf("Sending message (%u) as data locally...\n", p->cid());
						message->node_recv_ts()=this->time();
						this->messageInputQueue.push(out.p);
					}
					else {
					//	printf("Sending message (%u) as data network...\n", p->cid());
						this->sendData(nodeID,message,out.network);
					}
					}
			}
		}


	}

	void	Node::send(const	_Module	*sender,_Payload	*message,Network	network){

		message->send_ts()=this->time();
		_Payload::Category	cc= message->category();
		if(message->category()==_Payload::DATA){

			_Message	*_m=message->as_Message();
			_m->senderModule_cid()=sender->descriptor->CID;
			_m->senderModule_id()=sender->descriptor->ID;
			_m->senderNodeID()=sender->descriptor->hostID;
		}
		Messaging::send(message,network);
	}

	void	Node::newSpace(const	_Module	*sender,Network	network){

		CreateSpace	*cs=new	CreateSpace();
		cs->sender_cid=sender->descriptor->CID;
		cs->sender_id=sender->descriptor->ID;
		cs->host_id=_ID;
		Messaging::send(cs,network);
	}

	void	Node::newModule(const	_Module	*sender,uint16	CID,Network	network,const	char	*hostName){

		CreateModule	*cm=new	CreateModule();
		cm->sender_cid=sender->descriptor->CID;
		cm->sender_id=sender->descriptor->ID;
		cm->host_id=_ID;
		cm->module_cid=CID;
		Messaging::send(cm,network);
	}

	void	Node::deleteSpace(uint16	ID,Network	network){

		DeleteSpace	*ds=new	DeleteSpace();
		ds->space_id=ID;
		ds->host_id=_ID;
		Messaging::send(ds,network);
	}

	void	Node::deleteModule(uint16	CID,uint16	ID,Network	network){

		DeleteModule	*dm=new	DeleteModule();
		dm->module_cid=CID;
		dm->module_id=ID;
		dm->host_id=_ID;
		Messaging::send(dm,network);
	}

	void	Node::activateModule(const	_Module	*sender,uint16	module_cid,uint16	module_id,uint16	space_id,float32	activationLevel,Network	network){

		ActivateModule	*a=new	ActivateModule();
		a->host_id=_ID;
		a->module_cid=module_cid;
		a->module_id=module_id;
		a->space_id=space_id;
		a->activationLevel=activationLevel;
		send(sender,a,network);
	}

	void	Node::activateSpace(const	_Module	*sender,uint16	space_id,uint16	target_sid,float32	activationLevel,Network	network){

		ActivateSpace	*a=new	ActivateSpace();
		a->host_id=_ID;
		a->target_sid=target_sid;
		a->space_id=space_id;
		a->activationLevel=activationLevel;
		send(sender,a,network);
	}

	void	Node::setSpaceThreshold(const	_Module	*sender,uint16	space_id,float32	threshold,Network	network){

		SetThreshold	*s=new	SetThreshold();
		s->host_id=_ID;
		s->space_id=space_id;
		s->threshold=threshold;
		send(sender,s,network);
	}

	void	Node::subscribeMessage(const	_Module	*sender,uint16	module_cid,uint16	module_id,uint16	space_id,uint16	message_cid,Network	network){

		SubscribeMessage	*s=new SubscribeMessage();
		s->message_cid=message_cid;
		s->space_id=space_id;
		s->host_id=_ID;
		s->module_cid=module_cid;
		s->module_id=module_id;
		send(sender,s,network);
	}

	void	Node::unsubscribeMessage(const	_Module	*sender,uint16	module_cid,uint16	module_id,uint16	space_id,uint16	message_cid,Network	network){

		UnsubscribeMessage	*s=new UnsubscribeMessage();
		s->message_cid=message_cid;
		s->space_id=space_id;
		s->host_id=_ID;
		s->module_cid=module_cid;
		s->module_id=module_id;
		send(sender,s,network);
	}

	void	Node::subscribeStream(const	_Module	*sender,uint16	module_cid,uint16	module_id,uint16	space_id,uint16	stream_id,Network	network){

		SubscribeStream	*s=new SubscribeStream();
		s->host_id=_ID;
		s->module_cid=module_cid;
		s->space_id=space_id;
		s->stream_id=stream_id;
		s->module_id=module_id;
		send(sender,s,network);
	}

	void	Node::unsubscribeStream(const	_Module	*sender,uint16	module_cid,uint16	module_id,uint16	space_id,uint16	stream_id,Network	network){

		UnsubscribeStream	*s=new UnsubscribeStream();
		s->host_id=_ID;
		s->module_cid=module_cid;
		s->space_id=space_id;
		s->stream_id=stream_id;
		s->module_id=module_id;
		send(sender,s,network);
	}

	const	char	*Node::getSpaceName(uint16	hostID,uint16	ID){
	
		return	Space::Main[hostID][ID]->getName();
	}

	const	char	*Node::getModuleName(uint16	CID){

		return	ModuleRegister::Get(CID)->name();
	}

	void	Node::dump(const	char	*fileName){	//	TODO
	}
		
	void	Node::load(const	char	*fileName){	//	TODO
	}

	void	Node::migrate(uint16	CID,uint16	ID,uint16	NID){	//	TODO
	}

//	Array<uint8>	&Node::sharedMemorySegment(uint8	segment){
//
//		return	sharedMemorySegments[segment];
//	}

	_Module	*Node::getModule(uint16	hostID,uint16	CID,uint16	ID){

		return	ModuleDescriptor::Main[hostID][CID][ID]->module;
	}
}
