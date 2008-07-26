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


using	namespace	mBrane::sdk;

namespace	mBrane{
	namespace	node{

		Node::Node(const	char	*configFileName):sdk::Node(),_shutdown(false){
			
			Singleton=this;
			XMLNode	mainNode=XMLNode::openFileHelper(configFileName,"NodeConfiguration");
			//	read node config file and load plugins etc
			//	configuration:
			//		network interfaces: 1, 2 or 3. for each: emitter/receiver (plugins): TCP/IP, UDP/IP, IB with parameters: ex: interface(s) and port(s)
			//		application config file name
			//		ID and time reference: automatically negociated
			/*
			configure interfaces and node
			*/
			application_configuration_file=mainNode.getAttribute("application_configuration_file");
		}

		Node::~Node(){
		}

		void	Node::loadUserLibrary(const	char	*fileName){
#if defined	WINDOWS
			userLibrary=LoadLibrary(TEXT(fileName));
			if(!userLibrary){

				DWORD	error=GetLastError();
				std::cout<<"GetProcAddress Error: "<<error<<std::endl;
			}
#elif defined LINUX
#elif defined OSX
#endif
		}

		void	Node::unloadUserLibrary(){
#if defined	WINDOWS
			if(userLibrary)
				FreeLibrary(userLibrary);
#elif defined LINUX
#elif defined OSX
#endif
		}

		typedef	 sdk::_Crank *(__cdecl	*CrankInstantiator)(uint16);

		void	Node::loadApplication(const	char	*fileName){

			if(fileName)
				application_configuration_file=fileName;
			XMLNode	mainNode=XMLNode::openFileHelper(application_configuration_file,"ApplicationConfiguration");
			//	read application config file and load cranks etc
			//	to load:
			//		user library
			//		entities, modules, groups, cranks
			//		initial subscriptions (per crank, group)
			//		schedulers (per crank)
			//		migrable or not (per crank)
			//		reception policy: time first or priority first (for all cranks)
			//		thread count
			//		target thread (per crank)
			loadUserLibrary(mainNode.getAttribute("user_library"));
#if defined	WINDOWS
			if(userLibrary){ 

				CrankInstantiator	instantiator=(CrankInstantiator)GetProcAddress(userLibrary,"NewCR1"); //	test
				if(instantiator){

					sdk::_Crank	*c=(instantiator)(0);
					delete	c;
				}else{

					DWORD	error=GetLastError();
					std::cout<<"GetProcAddress Error: "<<error<<std::endl;
				}
			}
#elif defined LINUX
#elif defined OSX
#endif
		}

		void	Node::unloadApplication(){
			//	to unload
			//		entities, modules, groups, cranks
			//		user library
			unloadUserLibrary();
		}

		void	Node::run(){

			//	TODO:	launch threads: senders/receivers, crank exec units
		}

		void	Node::shutdown(){

			_shutdown=true;
		}

		void	Node::dump(const	char	*fileName){
		}
			
		void	Node::load(const	char	*fileName){
		}

		void	Node::send(uint16	crankID,_Message	*m){
		}

		void	Node::send(uint16	crankID,_ControlMessage	*m){
		}

		void	Node::send(uint16	crankID,_StreamData	*m){
		}

		int64	Node::time(){

			return	0;	//	TODO: if time base, return local time; if satellite, read internal buffer (timeDrift)
		}

		void	Node::buildCrank(uint16	CID){

			uint16	cid=0;	//	TODO: allocate cid (see below)
			_Crank	*c=(CrankRegister::Get(CID)->builder())(cid);
			//	TODO: read config for c, load c on a thread, update initial subscriptions, group membership etc
		}
	}
}