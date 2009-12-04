//	messaging.h
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

#ifndef	mBrane_messaging_h
#define	mBrane_messaging_h

#include	"../Core/pipe.h"
#include	"../Core/list.h"
#include	"../Core/control_messages.h"

#include	"networking.h"
#include	"module_descriptor.h"


using	namespace	mBrane::sdk;
using	namespace	mBrane::sdk::module;

#define	MESSAGE_INPUT_BLOCK_SIZE	64
#define	MESSAGE_OUTPUT_BLOCK_SIZE	64
#define	JOBS_BLOCK_SIZE				128

namespace	mBrane{

	typedef	struct	_Job{
		P<_Payload>	p;
		_Module		*m;
		_Job(_Payload	*p=NULL,_Module		*m=NULL):m(m){this->p=p;}
	}Job;

	class	Node;

	class	RecvThread:
	public	Thread{
	public:
		static	thread_ret thread_function_call	ReceiveMessages(void	*args);
		Pipe11<P<_Payload>,MESSAGE_INPUT_BLOCK_SIZE>	buffer;	//	incoming messages from remote nodes
		Node			*node;
		CommChannel		*channel;
		uint16			entry;
		RecvThread(Node	*node,CommChannel	*channel,uint16	entry);
		~RecvThread();
	};

	class	PushThread:
	public	Thread{
	public:
		static	thread_ret thread_function_call	PushJobs(void	*args);
		Node	*node;
		Pipe11<P<_Payload>,MESSAGE_OUTPUT_BLOCK_SIZE>	*source;
		PushThread(Node	*node,Pipe11<P<_Payload>,MESSAGE_OUTPUT_BLOCK_SIZE>	*source);
		~PushThread();
	};

	class	Executing;
	class	XThread;
	class	Messaging{
	friend	class	RecvThread;
	friend	class	PushThread;
	friend	class	XThread;
	friend	class	Executing;
	private:
		CriticalSection	moduleCS;	//	control access to spaces, modules descriptors and projections in processControlMessages.
		CriticalSection	spaceCS;
		CriticalSection	projectionCS;
	protected:
		typedef	struct{
			module::Node::Network	network;
			P<_Payload>	p;
		}MessageSlot;
		Pipe11<P<_Payload>,MESSAGE_INPUT_BLOCK_SIZE>	messageInputQueue;	//	incoming local messages
		PipeN1<MessageSlot,MESSAGE_OUTPUT_BLOCK_SIZE>	messageOutputQueue;	//	outgoing messages

		PipeNN<Job,JOBS_BLOCK_SIZE>	jobs;

		Array<RecvThread	*,MESSAGE_INPUT_BLOCK_SIZE>	recvThreads;
		Thread											*sendThread;
		static	thread_ret thread_function_call			SendMessages(void	*args);
		Array<PushThread	*,MESSAGE_INPUT_BLOCK_SIZE>	pushThreads;	//	one for each message source (recvThread->buffer plus input queue); push jobs in the job pipe
		
		//TODO: add a pointer to processControlMessage plugin here.

		Messaging();
		~Messaging();
		void	start();
		void	shutdown();
		void	send(_Payload	*message,module::Node::Network	network);
		void	pushJobs(_Payload	*p,NodeEntry	&e);
		void	pushJobs(_Payload	*p);
		void	processControlMessage(_Payload	*p);
	};
}


#endif
