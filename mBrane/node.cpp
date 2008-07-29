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

#include	<iostream>

#if defined	WINDOWS
	#include <sys/timeb.h>
	#include <time.h>
#elif defined LINUX
#elif defined OSX
#endif


using	namespace	mBrane::sdk;

namespace	mBrane{

	Node	*Node::New(const	char	*configFileName){

		Node	*n=new	Node();
		if(n->init(configFileName))
			return	n;
		delete	n;
		return	NULL;
	}

	Node::Node():sdk::Node(),_shutdown(false){

		networkDiscoveryInterface=NULL;
		networkInterfaces[CONTROL]=NULL;
		networkInterfaces[DATA]=NULL;
		networkInterfaces[STREAM]=NULL;
	}

	Node::~Node(){

		if(networkDiscoveryInterface)
			delete	networkDiscoveryInterface;
		if(networkInterfaces[CONTROL])
			delete	networkInterfaces[CONTROL];
		if(networkInterfaces[DATA])
			delete	networkInterfaces[DATA];
		if(networkInterfaces[STREAM])
			delete	networkInterfaces[STREAM];
	}

	Node	*Node::init(const	char	*configFileName){
#if defined	WINDOWS
		uint32	s=255;
		GetComputerName(hostName,&s);
#elif defined LINUX
#elif defined OSX
#endif
		XMLNode	mainNode=XMLNode::openFileHelper(configFileName,"NodeConfiguration");
		if(!mainNode){

			std::cout<<"Error: NodeConfiguration corrupted\n";
			return	NULL;
		}
		XMLNode	interfaces=mainNode.getChildNode("Interfaces");
		if(!interfaces){

			std::cout<<"Error: NodeConfiguration::Interfaces is missing\n";
			return	NULL;
		}
		XMLNode	discovery=mainNode.getChildNode("Discovery");
		if(!discovery){

			std::cout<<"Error: NodeConfiguration::Discovery is missing\n";
			return	NULL;
		}
		const	char	*_di=discovery.getAttribute("interface");
		if(!_di){

			std::cout<<"Error: NodeConfiguration::Discovery::interface is missing\n";
			return	NULL;
		}
		XMLNode	_ndi=interfaces.getChildNode(_di);
		if(!_ndi){

			std::cout<<"Error: NodeConfiguration::Interfaces::"<<_di<<" is missing\n";
			networkDiscoveryInterface=NULL;
			return	NULL;
		}else	if(!(networkDiscoveryInterface=NetworkInterface::New<NetworkDiscoveryInterface>(_ndi)))
			return	NULL;
		XMLNode	control=mainNode.getChildNode("Control");
		if(!control){

			std::cout<<"Error: NodeConfiguration::Control is missing\n";
			return	NULL;
		}
		const	char	*ci=control.getAttribute("interface");
		if(!ci){

			std::cout<<"Error: NodeConfiguration::Control::interface is missing\n";
			return	NULL;
		}
		XMLNode	nci=interfaces.getChildNode(ci);
		if(!nci){

			std::cout<<"Error: NodeConfiguration::Interfaces::"<<ci<<" is missing\n";
			networkInterfaces[CONTROL]=NULL;
			return	NULL;
		}else	if(!(networkInterfaces[CONTROL]=NetworkInterface::New<NetworkCommInterface>(nci)))
			return	NULL;
		XMLNode	data=mainNode.getChildNode("Data");
		if(!data){

			std::cout<<"Error: NodeConfiguration::Data is missing\n";
			return	NULL;
		}
		const	char	*di=data.getAttribute("interface");
		if(!di){

			std::cout<<"Error: NodeConfiguration::Data::interface is missing\n";
			return	NULL;
		}
		XMLNode	ndi=interfaces.getChildNode(di);
		if(!ndi){

			std::cout<<"Error: NodeConfiguration::Interfaces::"<<di<<" is missing\n";
			networkInterfaces[DATA]=NULL;
			return	NULL;
		}else	if(!(networkInterfaces[DATA]=NetworkInterface::New<NetworkCommInterface>(ndi)))
			return	NULL;
		XMLNode	stream=mainNode.getChildNode("Stream");
		if(!stream){

			std::cout<<"Error: NodeConfiguration::Stream is missing\n";
			return	NULL;
		}
		const	char	*si=stream.getAttribute("interface");
		XMLNode	nsi=interfaces.getChildNode(si);
		if(!nsi){

			std::cout<<"Error: NodeConfiguration::Interfaces::"<<si<<" is missing\n";
			networkInterfaces[STREAM]=NULL;
			return	NULL;
		}else	if(!(networkInterfaces[STREAM]=NetworkInterface::New<NetworkCommInterface>(nsi)))
			return	NULL;
		application_configuration_file=mainNode.getAttribute("application_configuration_file");
		if(!application_configuration_file){

			std::cout<<"Error: NodeConfiguration::application_configuration_file is missing\n";
			return	NULL;
		}
		return	this;
	}

	typedef	 sdk::_Crank *(__cdecl	*CrankInstantiator)(uint16);

	Node	*Node::loadApplication(const	char	*fileName){

		if(fileName)
			application_configuration_file=fileName;
		XMLNode	mainNode=XMLNode::openFileHelper(application_configuration_file,"ApplicationConfiguration");
		if(!mainNode){

			std::cout<<"Error: ApplicationConfiguration corrupted\n";
			return	NULL;
		}
		//	read application config file and load cranks etc
		//	to load:
		//		user library
		//		entities, modules, groups, cranks
		//		initial subscriptions (per crank, group)
		//		schedulers (per crank)
		//		migrable or not (per crank)
		//		reception policy: time first or priority first (for all cranks)
		//		user thread count
		//		target thread (per crank)
		const	char	*ul=mainNode.getAttribute("user_library");
		if(!ul){

			std::cout<<"Error: ApplicationConfiguration::user_library is missing\n";
			return	NULL;
		}
		if(!(userLibrary=SharedLibrary::New(ul)))
			return	NULL;
		//	test
		CrankInstantiator	instantiator=userLibrary->getFunction<CrankInstantiator>("NewCR1");
		if(instantiator){

			sdk::_Crank	*c=(instantiator)(0);
			delete	c;
		}
		return	this;
	}

	void	Node::unloadApplication(){
		//	to unload
		//		entities, modules, groups, cranks
		//		user library
		delete	userLibrary;
	}

	void	Node::run(){

		_threads.alloc();
		_threads[0]=Thread::New(ScanIDs,this);
		_threads.alloc();
		_threads[1]=Thread::New(AcceptConnections,this);
		//	TODO:	launch senders/receivers, time sender
		//	TODO:	build cranks and load them in CEU
		//	TODO:	launch CEU
		//	TODO:	wait for nodes and send SystemReady to (local) cranks
	}

	void	Node::shutdown(){

		_shutdown=true;
		Thread::Wait(_threads,_threads.count());
	}

	void	Node::dump(const	char	*fileName){	//	TODO
	}
		
	void	Node::load(const	char	*fileName){	//	TODO
	}

	void	Node::send(sdk::_Crank	*sender,_Payload	*message){	//	TODO
	}

	inline	int64	Node::localTime(){

		struct	_timeb	local_time;
		_ftime(&local_time);
		return	(int64)local_time.time*1000+local_time.millitm;
	}

	inline	int64	Node::time(){

		if(_isTimeReference)
			return	localTime();
		else
			return	localTime()-timeDrift;
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

	////////////////////////////////////////////////////////////////////////////////////////////////

	uint32	thread_function_call	Node::ScanIDs(void	*args){	//	TODO

		return	0;
	}

	uint32	thread_function_call	Node::AcceptConnections(void	*args){	//	TODO

		return	0;
	}

	uint32	thread_function_call	Node::SendTime(void	*args){	//	TODO

		return	0;
	}
	
	uint32	thread_function_call	Node::CrankExecutionUnit(void	*args){	//	TODO

		return	0;
	}

	uint32	thread_function_call	Node::ReceiveMessages(void	*args){	//	TODO

		return	0;
	}

	uint32	thread_function_call	Node::SendMessages(void	*args){	//	TODO

		return	0;
	}
}