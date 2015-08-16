/*
* HUMANOBS - mBrane
*
* Eric Nivel
* Center for Analysis and Design of Intelligent Agents
*   Reykjavik University, Menntavegur 1, 101 Reykjavik, Iceland
*   http://cadia.ru.is
* Copyright(c)2012
*
* This software was developed by the above copyright holder as part of 
* the HUMANOBS EU research project, in collaboration with the 
* following parties:
* 
* Autonomous Systems Laboratory
*   Technical University of Madrid, Spain
*   http://www.aslab.org/
*
* Communicative Machines
*   Edinburgh, United Kingdom
*   http://www.cmlabs.com/
*
* Istituto Dalle Molle di Studi sull'Intelligenza Artificiale
*   University of Lugano and SUPSI, Switzerland
*   http://www.idsia.ch/
*
* Institute of Cognitive Sciences and Technologies
*   Consiglio Nazionale delle Ricerche, Italy
*   http://www.istc.cnr.it/
*
* Dipartimento di Ingegneria Informatica
*   University of Palermo, Italy
*   http://roboticslab.dinfo.unipa.it/index.php/Main/HomePage
*
*
* --- HUMANOBS Open-Source BSD License, with CADIA Clause v 1.0 ---
*
* Redistribution and use in source and binary forms, with or without 
* modification, is permitted provided that the following conditions 
* are met:
*
* - Redistributions of source code must retain the above copyright 
* and collaboration notice, this list of conditions and the 
* following disclaimer.
*
* - Redistributions in binary form must reproduce the above copyright 
* notice, this list of conditions and the following
* disclaimer in the documentation and/or other materials provided 
* with the distribution.
*
* - Neither the name of its copyright holders nor the names of its 
* contributors may be used to endorse or promote products 
* derived from this software without specific prior written permission.
*
* - CADIA Clause: The license granted in and to the software under this 
* agreement is a limited-use license. The software may not be used in 
* furtherance of: 
* (i) intentionally causing bodily injury or severe emotional distress 
* to any person; 
* (ii) invading the personal privacy or violating the human rights of 
* any person; or 
* (iii) committing or preparing for any act of war.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
* "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
* LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR 
* A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
* OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
* SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
* LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, 
* DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY 
* THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
* OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include	"mdaemon_node.h"


namespace	mBrane{
	namespace	sdk{
		namespace	mdaemon{

			Node::Node(uint8_t	ID):module::Node(ID),_shutdown(false){
			}

			Node::~Node(){

				for(uint32_t	i=0;i<daemonLoaders.count();i++){

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

					uint32_t	daemonCount=daemons.nChildNode("Daemon");
					daemonLoaders.alloc(daemonCount);
					this->daemons.alloc(daemonCount);
					daemonThreads.alloc(daemonCount);

					for(uint32_t	i=0;i<daemonCount;i++){

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

				for(uint32_t	i=0;i<daemons.count();i++)
					daemonThreads[i]=Thread::New<Thread>(Daemon::Run,daemons[i]);
			}

			void	Node::shutdown(){

				Thread::Wait(daemonThreads.data(),daemonThreads.count());
			}

			////////////////////////////////////////////////////////////////////////////////////////////////

			thread_ret thread_function_call	Daemon::Run(void	*args){

				uint32_t	r;
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
