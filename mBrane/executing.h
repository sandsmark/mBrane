//	executing.h
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

#ifndef	mBrane_executing_h
#define	mBrane_executing_h

#include	"../Core/utils.h"
#include	"../Core/module.h"
#include	"../Core/xml_parser.h"


using	namespace	mBrane::sdk;
using	namespace	mBrane::sdk::module;

namespace	mBrane{

	class	Node;
	//	Message processing thread class.
	class	XThread:
	public	Thread,
	public	FastSemaphore{
	public:
		static	thread_ret thread_function_call	Xec(void	*args);
		Node	*node;
		bool	wasSupporting;
		
		XThread(Node	*n);
		~XThread();
		void	work(_Payload	*p,_Module	*c);
	};

	//	XThread pool.
	class	Executing{
	friend	class	XThread;
	protected:
		
		StaticArray<Thread	*>	xThreads;
		uint16		threadCount;
		Semaphore	*supportSync;

		Executing();
		~Executing();
		bool	loadConfig(XMLNode	&n);
		void	start();
		void	shutdown();
	};
}


#endif
