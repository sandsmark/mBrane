//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/ //_/_/ //_/_/  mdaemon_node.h //_/_/ //_/_/  Eric Nivel //_/_/  Center for Analysis and Design of Intelligent Agents //_/_/    Reykjavik University, Menntavegur 1, 101 Reykjavik, Iceland //_/_/    http://cadia.ru.is
//_/_/  Copyright©2012 //_/_/ //_/_/  This software was developed by the above copyright holder as part of  //_/_/  the HUMANOBS EU research project, in collaboration with the  //_/_/  following parties:
//_/_/   //_/_/  Autonomous Systems Laboratory //_/_/    Technical University of Madrid, Spain //_/_/    http://www.aslab.org/ //_/_/ //_/_/  Communicative Machines //_/_/    Edinburgh, United Kingdom //_/_/    http://www.cmlabs.com/ //_/_/ //_/_/  Istituto Dalle Molle di Studi sull'Intelligenza Artificiale //_/_/    University of Lugano and SUPSI, Switzerland //_/_/    http://www.idsia.ch/ //_/_/ //_/_/  Institute of Cognitive Sciences and Technologies //_/_/    Consiglio Nazionale delle Ricerche, Italy //_/_/    http://www.istc.cnr.it/
//_/_/
//_/_/  Dipartimento di Ingegneria Informatica
//_/_/    University of Palermo, Italy
//_/_/    http://roboticslab.dinfo.unipa.it/index.php/Main/HomePage
//_/_/ //_/_/
//_/_/  --- HUMANOBS Open-Source BSD License, with CADIA Clause v 1.0 ---
//_/_/
//_/_/  Redistribution and use in source and binary forms, with or without 
//_/_/  modification, is permitted provided that the following conditions 
//_/_/  are met:
//_/_/
//_/_/  - Redistributions of source code must retain the above copyright 
//_/_/    and collaboration notice, this list of conditions and the 
//_/_/    following disclaimer.
//_/_/
//_/_/  - Redistributions in binary form must reproduce the above copyright 
//_/_/    notice, this list of conditions and the following
//_/_/    disclaimer in the documentation and/or other materials provided 
//_/_/    with the distribution.
//_/_/
//_/_/  - Neither the name of its copyright holders nor the names of its 
//_/_/    contributors may be used to endorse or promote products 
//_/_/    derived from this software without specific prior written permission.
//_/_/
//_/_/  - CADIA Clause: The license granted in and to the software under this 
//_/_/     agreement is a limited-use license. The software may not be used in 
//_/_/     furtherance of: 
//_/_/     (i) intentionally causing bodily injury or severe emotional distress 
//_/_/         to any person; 
//_/_/     (ii) invading the personal privacy or violating the human rights of 
//_/_/         any person; or 
//_/_/     (iii) committing  or preparing for any act of war.
//_/_/
//_/_/  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
//_/_/  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
//_/_/  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR 
//_/_/  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
//_/_/  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
//_/_/  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
//_/_/  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, 
//_/_/  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY 
//_/_/  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
//_/_/  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
//_/_/  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//_/_/
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

#ifndef	mBrane_sdk_mdaemon_node_h
#define	mBrane_sdk_mdaemon_node_h

#include	"module_node.h"
#include	"xml_parser.h"
#include	"dynamic_class_loader.h"

#pragma warning( disable : 4251 )

namespace	mBrane{
	class	RecvThread;
	class	PushThread;
	class	XThread;
	class	Executing;
	namespace	sdk{
		namespace	mdaemon{

			class	Daemon;
			//	Node API, as seen from the daemons
			class	mBrane_dll	Node:
			public	module::Node{
			friend	class	RecvThread;
			friend	class	PushThread;
			friend	class	XThread;
			friend	class	Executing;
			protected:
				bool	volatile	_shutdown;
				StaticArray<DynamicClassLoader<Daemon>	*>	daemonLoaders;
				StaticArray<Daemon	*>						daemons;
				StaticArray<Thread	*>						daemonThreads;
				Node(uint8	ID=NoID);
				bool	loadConfig(XMLNode	&n);
				void	start();
				virtual	void	shutdown();
				~Node();
			public:
				bool	isRunning();
				virtual	void				dump(const	char	*fileName)=0;	//	dumps the current system state; module dump fileNames: module_class_ID.bin: ex: CR1_123.bin
				virtual	void				load(const	char	*fileName)=0;	//	initializes itself from a previously saved system state
				virtual	void				migrate(uint16	CID,uint16	ID,uint8	NID)=0;	//	identifies the module and the target node
				//virtual	Array<uint8,65535>	&sharedMemorySegment(uint8	segment)=0;	//	FUTURE DEVELOPMENT: pinned down RAM for RDMA, 16KB-1
				virtual	module::_Module		*getModule(uint8	hostID,uint16	CID,uint16	ID)=0;	//	so that the daemons can write module internals and use modules as interfaces to the pub-sub network
				//	TODO:	define API as pure virtual functions
				//			-> node map (an array of mBrane::Networking::NetworkID)
				//			-> profiling data
				//				- msg throughput
				//				- routing latencies (send_ts/recv_ts, node_send_ts/node_recv_ts)
				//				- runtime per module, and its ratio wrt total node runtime, routing time consumption/module runtime
				//				- logical topology, i.e. affinities between modules; traffic between nodes in light of traffic between modules
			};

			class	mBrane_dll	Daemon{
			protected:
				Node	*const	node;
				Daemon(Node	*node);
			public:
				typedef	Daemon	*(*Load)(XMLNode	&,Node	*);	//	function exported by the shared library
				static	thread_ret thread_function_call	Run(void	*args);	//	args=this daemon
				virtual	~Daemon();
				virtual	void	init()=0;	//	called once, before looping
				virtual	uint32	run()=0;	//	called in a loop: while(!node->_shutdown); returns error code (or 0 if none)
				virtual	void	shutdown()=0;	//	called when run returns an error, and when the node shutsdown
			};
		}
	}
}


#endif
