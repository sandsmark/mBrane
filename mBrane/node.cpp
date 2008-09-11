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
#include	"..\Core\class_register.h"
#include	"..\Core\module_register.h"
#include	"..\Core\control_messages.h"

#include	<iostream>


using	namespace	mBrane::sdk::payloads;

namespace	mBrane{

	const	uint16	Node::NoNode=0xFFFF;

	Node	*Node::New(const	char	*configFileName){

		Node	*n=new	Node();
		if(n->loadConfig(configFileName))
			return	n;
		delete	n;
		return	NULL;
	}

	Node::Node():Networking(),MESSAGING_CLASS(),Executing(),nodeCount(0){
	}

	Node::~Node(){

		for(uint16	i=0;i<nodeNames.count();i++)
			if(nodeNames[i])
				free((void	*)nodeNames[i]);
	}

	Node	*Node::loadConfig(const	char	*configFileName){

		hostNameSize=Host::Name(hostName);

		XMLNode	mainNode=XMLNode::openFileHelper(configFileName,"NodeConfiguration");
		if(!mainNode){

			std::cout<<"Error: NodeConfiguration corrupted\n";
			return	NULL;
		}
		
		if(!Networking::loadConfig(mainNode))
			return	NULL;

		if(!Executing::loadConfig(mainNode))
			return	NULL;

		if(!daemon::Node::loadConfig(mainNode))
			return	NULL;

		XMLNode	nodeList=mainNode.getChildNode("Nodes");
		if(!nodeList)
			nodeCount=0;
		else{

			nodeCount=nodeList.nChildNode("Node");
			nodeNames.alloc(nodeCount);
			for(uint16	i=0;i<nodeCount;i++){

				XMLNode	n=nodeList.getChildNode(i);
				const	char	*_n=n.getAttribute("hostname");
				if(!_n){

					std::cout<<"Error: NodeConfiguration::Nodes::node_"<<i<<"::hostname is missing\n";
					nodeNames[i]=NULL;
					return	NULL;
				}
				nodeNames[i]=_n;
			}
		}

		//	FUTURE DEVELOPMENT: load RDMA library, alloc sharedMemorySegments and pin them down. See test_node.xml

		const	char	*application_configuration_file=mainNode.getAttribute("application_configuration_file");
		if(!application_configuration_file){

			std::cout<<"Error: NodeConfiguration::application_configuration_file is missing\n";
			return	NULL;
		}

		if(!loadApplication(application_configuration_file))
			return	NULL;

		return	this;
	}

	bool	Node::loadApplication(const	char	*fileName){

		Space	*rootSpace=new	Space();
		rootSpace->setActivationThreshold(1.0);

		XMLNode	mainNode=XMLNode::openFileHelper(fileName,"ApplicationConfiguration");
		if(!mainNode){

			std::cout<<"Error: ApplicationConfiguration corrupted\n";
			return	false;
		}
		const	char	*ul=mainNode.getAttribute("user_library");
		if(!ul){

			std::cout<<"Error: ApplicationConfiguration::user_library is missing\n";
			return	false;
		}
		if(!(userLibrary=SharedLibrary::New(ul)))
			return	false;

		typedef	void	(*UserInitFunction)();
		UserInitFunction	userInitFunction=userLibrary->getFunction<UserInitFunction>("Init");
		if(!userInitFunction)
			return	false;
		userInitFunction();
		ClassRegister::Get(0);//	DEBUG

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

		Space::Init();
		rootSpace->activate();

		return	true;
	}

	void	Node::unloadApplication(){

		delete	userLibrary;
	}

	uint16	Node::getNID(const	char	*name){

		if(strcmp(name,"local")==0)
			return	_ID;
		for(uint16	i=0;i<nodeNames.count();i++)
			if(strcmp(nodeNames[i],networkID->name())==0)
				return	i;
		return	NoNode;
	}

	void	Node::run(){

		if(!Networking::init())
			shutdown();
	}

	void	Node::start(uint16	assignedNID,NetworkID	*networkID,bool	isTimeReference){

		Networking::start(assignedNID,networkID,isTimeReference);
		Messaging::start();

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

		daemon::Node::start();
		Executing::start();

		bootCallback();
	}

	void	Node::notifyNodeJoined(uint16	NID,NetworkID	*networkID){

		static	NodeJoined	m;

		m.node_id=NID;
		m.send_ts()=Time::Get();
		Messaging::send(&m,LOCAL);

		std::cout<<"Node joined: "<<networkID->name()<<":"<<NID<<std::endl;

		if(isTimeReference	&&	nodeCount){

			for(uint16	i=0;i<nodeNames.count();i++){

				if(strcmp(nodeNames[i],networkID->name())==0){

					free((void	*)nodeNames[i]);
					nodeNames[i]=NULL;
					nodeCount--;
					break;
				}
			}
			if(!nodeCount){

				SystemReady	m;
				m.send_ts()=Time::Get();
				Messaging::send(&m,BOTH);
			}
		}
	}

	void	Node::notifyNodeLeft(uint16	NID){

		static	NodeLeft	m;

		if(	controlChannels[PRIMARY][NID]				||
			dataChannels[NID]->channels[PRIMARY].data	||	
			dataChannels[NID]->channels[PRIMARY].stream	||
			controlChannels[SECONDARY][NID]				||
			dataChannels[NID]->channels[SECONDARY].data	||	
			dataChannels[NID]->channels[SECONDARY].stream){

			m.node_id=NID;
			m.send_ts()=Time::Get();
			Messaging::send(&m,LOCAL);

			std::cout<<"Node left: "<<dataChannels[NID]->networkID->name()<<":"<<NID<<std::endl;
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
		Messaging::shutdown();
		Executing::shutdown();
		daemon::Node::shutdown();
		Networking::shutdown();
		unloadApplication();
	}

	inline	int64	Node::time()	const{

		if(isTimeReference)
			return	Time::Get();
		else
			return	Time::Get()-timeDrift;
	}

	void	Node::send(const	_Module	*sender,_Payload	*message,Network	network){

		if(message->category()==_Payload::DATA){

			_Message	*_m=message->operator	_Message	*();
			_m->senderModule_cid()=sender->descriptor->CID;
			_m->senderModule_id()=sender->descriptor->ID;
		}
		Messaging::send(message,network);
	}

	void	Node::newSpace(const	_Module	*sender,Network	network){

		CreateSpace	cs;
		cs.sender_cid=sender->descriptor->CID;
		cs.sender_id=sender->descriptor->ID;
		Messaging::send(&cs,network);
	}

	void	Node::newModule(const	_Module	*sender,uint16	CID,Network	network,const	char	*hostName){

		CreateModule	cm;
		cm.sender_cid=sender->descriptor->CID;
		cm.sender_id=sender->descriptor->ID;
		if((cm.node_id=getNID(hostName))!=NoNode){

			cm.module_cid=CID;
			Messaging::send(&cm,network);
		}
	}

	void	Node::deleteSpace(uint16	ID,Network	network){

		DeleteSpace	ds;
		ds.space_id=ID;
		Messaging::send(&ds,network);
	}

	void	Node::deleteModule(uint16	CID,uint16	ID,Network	network){

		DeleteModule	dm;
		dm.module_cid=CID;
		dm.module_id=ID;
		Messaging::send(&dm,network);
	}

	const	char	*Node::getSpaceName(uint16	ID){
	
		return	Space::Main[ID]->getName();
	}

	const	char	*Node::getModuleName(uint16	ID){

		return	ModuleRegister::Get(ID)->name();
	}

	void	Node::dump(const	char	*fileName){	//	TODO
	}
		
	void	Node::load(const	char	*fileName){	//	TODO
	}

	void	Node::migrate(uint16	CID,uint16	ID,uint16	NID){	//	TODO
	}

	Array<uint8>	&Node::sharedMemorySegment(uint8	segment){

		return	sharedMemorySegments[segment];
	}

	_Module	*Node::getModule(uint16	CID,uint16	ID){

		return	ModuleDescriptor::Main[CID][ID]->module;
	}
}