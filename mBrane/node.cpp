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

	Node	*Node::New(const	char	*configFileName,uint8	traceLevels){

		Node	*n=new	Node(traceLevels);
		if(n->loadConfig(configFileName))
			return	n;
		return	NULL;
	}

	Node::Node(uint8	traceLevels):Networking(),Messaging(),Executing(),nodeCount(0){

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

		std::cout<<"---------------- mBrane V"<<MBRANE_VERSION<<" ----------------"<<std::endl;
		std::cout<<"> Started on "<<hostName<<std::endl;

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

		XMLNode	nodeList=mainNode.getChildNode("Nodes");
		if(!nodeList)
			nodeCount=0;
		else{

			nodeCount=nodeList.nChildNode("Node");
			for(uint16	i=0;i<nodeCount;i++){

				XMLNode	n=nodeList.getChildNode(i);
				const	char	*_n=n.getAttribute("hostname");
				if(!_n){

					std::cout<<"> Error: NodeConfiguration::Nodes::node_"<<i<<"::hostname is missing"<<std::endl;
					strcpy(nodeNames[i],"");
					return	NULL;
				}
				strcpy(nodeNames[i],_n);
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

	void	Node::unloadApplication(){

		delete	userLibrary;
	}

	uint16	Node::getNID(const	char	*name){

		if(stricmp(name,"local")==0)
			return	_ID;
		for(uint16	i=0;i<nodeNames.count();i++)
			if(stricmp(nodeNames[i],networkID->name())==0)
				return	i;
		return	NoID;
	}

	const	char	*Node::name(){

		return	hostName;
	}

	void	Node::run(){

		if(!Networking::init()){

			shutdown();
			return;
		}

		std::cout<<"> Running"<<std::endl;

		Thread::Sleep();
	}

	void	Node::start(uint16	assignedNID,NetworkID	*networkID,bool	isTimeReference){

		// we only need to do this once.

		static bool started = false;

		if (started)
			return;

		started = true;

		std::cout << "Debug: node starting up ... " << std::endl;

		Networking::start(assignedNID,networkID,isTimeReference);

		if(network==PRIMARY	||	network==BOTH){

			if(networkInterfaces[CONTROL_PRIMARY]->canBroadcast()){

				RecvThread	*t=new	RecvThread(this,controlChannels[PRIMARY][0],0);
				recvThreads[recvThreads.count()]=t;
				t->start(RecvThread::ReceiveMessages);
			}
		}
		if(network==SECONDARY	||	network==BOTH){

			if(networkInterfaces[CONTROL_SECONDARY]->canBroadcast()){

				RecvThread	*t=new	RecvThread(this,controlChannels[SECONDARY][0],0);
				recvThreads[recvThreads.count()]=t;
				t->start(RecvThread::ReceiveMessages);
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
		}

	}

	void	Node::ready(void) {

		Space::Init(_ID);
		ModuleDescriptor::Init(_ID);

		if ( isTimeReference ) {

			SystemReady	*m = new SystemReady();

			m->send_ts() = Time::Get();
			Messaging::send(m, BOTH);

		}
	}

	void	Node::notifyNodeJoined(uint16	NID,NetworkID	*networkID){

		static	uint16	ToJoin=nodeCount;

		if(!ToJoin	&&	nodeCount){	//	a node is joining after startup

			//	TODO: update NodeEntries. See Node::notifyNodeLeft()
		}else	if(ToJoin){	//	a node is joining during startup

			for(uint16	i=0;i<nodeNames.count();i++)
				if(stricmp(nodeNames[i],networkID->name())==0){

					ToJoin--;
					break;
				}

			for(uint32	i=0;i<ModuleDescriptor::Config.count();i++)	//	resolve host names into NID
				for(uint32	j=0;j<ModuleDescriptor::Config[i].count();j++){

					if(stricmp(ModuleDescriptor::Config[i][j]->hostName,networkID->name())==0){

						ModuleDescriptor::Main[NID][i][j]=ModuleDescriptor::Config[i][j];
						ModuleDescriptor::Main[NID][i][j]->hostID=NID;
						ModuleDescriptor::Main[NID][i][j]->applyInitialProjections(NID);
					}
				}

			if(!ToJoin){	//	startup time initialization

				Node::ready();

			}
		}else{	//	nodeCount==0

			Node::ready();

		}

		NodeJoined	*m=new	NodeJoined();
		m->node_id=NID;
		m->send_ts()=Time::Get();
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
			m->send_ts()=Time::Get();
			Messaging::send(m,LOCAL);

			if (dataChannels[NID]->networkID)
				Node::Get()->trace(Node::NETWORK)<<"> Node left: "<<dataChannels[NID]->networkID->name()<<":"<<NID<<std::endl;
		}
	}

	void	Node::startReceivingThreads(uint16	NID){

		if(network==PRIMARY	||	network==BOTH){

			if(!networkInterfaces[CONTROL_PRIMARY]->canBroadcast()){

				RecvThread	*t=new	RecvThread(this,controlChannels[PRIMARY][NID],NID);
				recvThreads[recvThreads.count()]=t;
				t->start(RecvThread::ReceiveMessages);
			}

			if(networkInterfaces[DATA_PRIMARY]!=networkInterfaces[CONTROL_PRIMARY]){

				RecvThread	*t=new	RecvThread(this,dataChannels[NID]->channels[PRIMARY].data,NID);
				recvThreads[recvThreads.count()]=t;
				t->start(RecvThread::ReceiveMessages);
			}

			if(networkInterfaces[STREAM_PRIMARY]!=networkInterfaces[DATA_PRIMARY]){

				RecvThread	*t=new	RecvThread(this,dataChannels[NID]->channels[PRIMARY].stream,NID);
				recvThreads[recvThreads.count()]=t;
				t->start(RecvThread::ReceiveMessages);
			}
		}

		if(network==SECONDARY	||	network==BOTH){

			if(!networkInterfaces[CONTROL_SECONDARY]->canBroadcast()){

				RecvThread	*t=new	RecvThread(this,controlChannels[SECONDARY][NID],NID);
				recvThreads[recvThreads.count()]=t;
				t->start(RecvThread::ReceiveMessages);
			}

			if(networkInterfaces[DATA_SECONDARY]!=networkInterfaces[CONTROL_SECONDARY]){

				RecvThread	*t=new	RecvThread(this,dataChannels[NID]->channels[SECONDARY].data,NID);
				recvThreads[recvThreads.count()]=t;
				t->start(RecvThread::ReceiveMessages);
			}

			if(networkInterfaces[STREAM_SECONDARY]!=networkInterfaces[DATA_SECONDARY]){

				RecvThread	*t=new	RecvThread(this,dataChannels[NID]->channels[SECONDARY].stream,NID);
				recvThreads[recvThreads.count()]=t;
				t->start(RecvThread::ReceiveMessages);
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
		unloadApplication();
	}

	inline	int64	Node::time()	const{

		if(isTimeReference)
			return	Time::Get();
		else
			return	Time::Get()-timeDrift;
	}

	void	Node::send(const	_Module	*sender,_Payload	*message,Network	network){

		_Payload::Category	cc= message->category();
		if(message->category()==_Payload::DATA){

			_Message	*_m=message->as_Message();
			_m->senderModule_cid()=sender->descriptor->CID;
			_m->senderModule_id()=sender->descriptor->ID;
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
