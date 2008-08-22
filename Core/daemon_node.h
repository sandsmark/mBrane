//	daemon_node.h
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

#ifndef	mBrane_sdk_daemon_node_h
#define	mBrane_sdk_daemon_node_h

#include	"module_node.h"
#include	"xml_parser.h"
#include	"dynamic_class_loader.h"

//	Node API, as seen from the daemons
namespace	mBrane{
	class	RecvThread;
	class	XThread;
	class	OrderedMessagingEngine;
	class	UnorderedMessagingEngine;
	class	Executing;
	namespace	sdk{
		namespace	daemon{

			class	Daemon;
			class	dll	Node:
			public	module::Node{
			friend	class	RecvThread;
			friend	class	XThread;
			friend	class	UnorderedMessagingEngine;
			friend	class	OrderedMessagingEngine;
			friend	class	Executing;
			protected:
				bool	_shutdown;
				Array<DynamicClassLoader<Daemon>	*>	daemonLoaders;
				Array<Daemon	*>						daemons;
				Array<Thread	*>						daemonThreads;
				Node(uint16	ID=NO_ID);
				bool	loadConfig(XMLNode	&n);
				void	start();
				virtual	void	shutdown();
				~Node();
			public:
				bool	isRunning();
				virtual	void	dump(const	char	*fileName)=0;	//	dumps the current system state; module dump fileNames: module_class_ID.bin: ex: CR1_123.bin
				virtual	void	load(const	char	*fileName)=0;	//	initializes itself from a previously saved system state
				virtual	void	migrate(uint16	CID,uint16	ID,uint16	NID)=0;	//	identifies the module and the target node
				//	TODO:	define API as pure virtual functions
				//			-> node map (an array of mBrane::Networking::NetworkID)
				//			-> profiling data
				//				- msg throughput
				//				- routing latencies (send_ts/recv_ts, node_send_ts/node_recv_ts)
				//				- runtime per module, and its ratio wrt total node runtime, routing time consumption/module runtime
				//				- logical topology, i.e. affinities between modules; traffic between nodes in light of traffic between modules
			};

			class	dll	Daemon{
			protected:
				Node	*node;
				Daemon(Node	*node);
			public:
				typedef	Daemon	*(*Load)(XMLNode	&,Node	*);	//	function exported by the shared library
				static	uint32	thread_function_call	Run(void	*args);	//	args=this daemon
				virtual	~Daemon();
				virtual	void	init()=0;	//	called once, before looping
				virtual	uint32	run()=0;	//	called in a loop: while(!node->_shutdown); returns error code (or 0 if none)
				virtual	void	shutdown()=0;	//	called when run returns an error, and when the node shutsdown
			};
		}
	}
}


#endif
