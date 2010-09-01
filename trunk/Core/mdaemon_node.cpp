//	mdaemon_node.cpp
//
//	Author: Eric Nivel
//
//	BSD license:
//	Copyright (c) 2010, Eric Nivel
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

#include	"mdaemon_node.h"


namespace	mBrane{
	namespace	sdk{
		namespace	mdaemon{

			Node::Node(uint8	ID):module::Node(ID),_shutdown(false){
			}

			Node::~Node(){

				for(uint32	i=0;i<daemonLoaders.count();i++){

					if(daemonLoaders[i])
						delete	daemonLoaders[i];
					if(daemons[i])
						delete	daemons[i];
				}
			}

			inline	bool	Node::isRunning(){

				return	!_shutdown;
			}

			bool	Node::loadConfig(XMLNode	&n){

				XMLNode	daemons=n.getChildNode("Daemons");
				if(!!daemons){

					uint32	daemonCount=daemons.nChildNode("Daemon");
					daemonLoaders.alloc(daemonCount);
					this->daemons.alloc(daemonCount);
					daemonThreads.alloc(daemonCount);

					for(uint32	i=0;i<daemonCount;i++){

						XMLNode	n=daemons.getChildNode(i);
						DynamicClassLoader<Daemon>	*dl;
						if(!(dl=DynamicClassLoader<Daemon>::New(n)))
							return	false;
						daemonLoaders[i]=dl;
						Daemon	*d;
						if(!(d=dl->getInstance(n,this)))
							return	false;
						this->daemons[i]=d;
					}
				}

				return	true;
			}

			void	Node::start(){

				for(uint32	i=0;i<daemons.count();i++)
					daemonThreads[i]=Thread::New<Thread>(Daemon::Run,daemons[i]);
			}

			void	Node::shutdown(){

				Thread::Wait(daemonThreads.data(),daemonThreads.count());
			}

			////////////////////////////////////////////////////////////////////////////////////////////////

			thread_ret thread_function_call	Daemon::Run(void	*args){

				uint32	r;
				((Daemon	*)args)->init();
				while(((Daemon	*)args)->node->isRunning())
					if(r=((Daemon	*)args)->run()){

						((Daemon	*)args)->shutdown();
						thread_ret_val(r);
					}
				((Daemon	*)args)->shutdown();
				thread_ret_val(0);
			}

			Daemon::Daemon(Node	*node):node(node){
			}

			Daemon::~Daemon(){
			}
		}
	}
}
