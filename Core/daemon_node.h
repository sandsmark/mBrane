// daemon_node.h
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

#ifndef	mBrane_sdk_daemon_node_h
#define	mBrane_sdk_daemon_node_h

#include	"crank_node.h"
#include	"xml_parser.h"
#include	"dynamic_class_loader.h"


namespace	mBrane{
	namespace	sdk{
		namespace	daemon{

			class	Daemon;
			class	dll	Node:
			public	crank::Node{
			protected:
				static	const	uint16	NO_ID=0xFFFF;
				uint16	_ID;	//	max: 0xFFFE
				bool	_shutdown;
				Array<DynamicClassLoader<Daemon>	*>	daemonLoaders;
				Array<Daemon	*>						daemons;
				Array<Thread	*>						daemonThreads;
				void	start();
				virtual	void	shutdown();
				Node(uint16	ID=NO_ID);
			public:
				~Node();
				uint16	ID()	const;
				bool	isRunning();
				bool	loadConfig(XMLNode	&n);
				//	TODO:	define API as pure virtual functions
				//			-> node map
				//			-> stats
				//			-> ...
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