//	messaging.h
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

#ifndef	mBrane_messaging_h
#define	mBrane_messaging_h

#include	"..\Core\pipe.h"

#include	"networking.h"
#include	"module_descriptor.h"


using	namespace	mBrane::sdk;
using	namespace	mBrane::sdk::module;

#define	MESSAGE_INPUT_QUEUE_BLOCK_SIZE	32
#define	MESSAGE_OUTPUT_QUEUE_BLOCK_SIZE	32
#define	JOB_QUEUE_BLOCK_SIZE			64

namespace	mBrane{

	typedef	struct	_Job{
		P<_Payload>			p;
		ModuleDescriptor	*m;
	}Job;

	class	Node;
	class	RecvThread:
	public	Thread{
	public:
		static	uint32	thread_function_call	ReceiveMessages(void	*args);
		Pipe<P<_Payload>,MESSAGE_INPUT_QUEUE_BLOCK_SIZE>	buffer;	//	incoming messages from remote nodes
		Node			*node;
		CommChannel		*channel;
		uint16			entry;
		RecvThread(Node	*node,CommChannel	*channel,uint16	entry);
		~RecvThread();
	};

	class	UnorderedMessagingEngine;
	class	OrderedMessagingEngine;
	class	Executing;
	class	XThread;
	template<class	Engine>	class	Messaging:
	public	Engine{
	friend	class	RecvThread;
	friend	class	XThread;
	friend	class	UnorderedMessagingEngine;
	friend	class	OrderedMessagingEngine;
	friend	class	Executing;
	protected:
		typedef	struct{
			module::Node::Network	network;
			P<_Payload>	p;
		}MessageSlot;
		Pipe<P<_Payload>,MESSAGE_INPUT_QUEUE_BLOCK_SIZE>	messageInputQueue;	//	incoming local messages
		Pipe<MessageSlot,MESSAGE_OUTPUT_QUEUE_BLOCK_SIZE>	messageOutputQueue;

		Pipe<Job,JOB_QUEUE_BLOCK_SIZE>	jobs;

		Array<RecvThread	*>	recvThreads;
		Thread					*sendThread;
		static	uint32	thread_function_call	SendMessages(void	*args);
		Semaphore	*inputSync;	//	sync on the input message count
				
		Messaging();
		~Messaging();
		void	send(_Payload	*message,module::Node::Network	network);
		void	processControlMessage(_Payload	*p);
		void	pushJobs(_Payload	*p,NodeEntry	&e);
		void	pushJobs(_Payload	*p);
		void	start();
		void	shutdown();
	};
}


#include	"messaging.tpl.cpp"


#endif