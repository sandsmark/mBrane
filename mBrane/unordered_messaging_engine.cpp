//	unordered_messaging_engine.cpp
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

#include	"node_config.h"

#if	defined	UNORDERED_MESSAGING_ENGINE


#include	"unordered_messaging_engine.h"
#include	"node.h"
#include	"..\Core\control_messages.h"


using	namespace	mBrane::sdk::payloads;

namespace	mBrane{

	UnorderedMessagingEngine::UnorderedMessagingEngine():jobFeeder(NULL){
	}

	UnorderedMessagingEngine::~UnorderedMessagingEngine(){

		if(jobFeeder)
			delete	jobFeeder;
	}
	
	void	UnorderedMessagingEngine::start(){

		jobFeeder=Thread::New<Thread>(FeedJobs,this);
	}

	void	UnorderedMessagingEngine::shutdown(){
	}

	////////////////////////////////////////////////////////////////////////////////////////////////

	uint32	thread_function_call	UnorderedMessagingEngine::FeedJobs(void	*args){

		Node	*node=(Node	*)args;

		uint32	recvThread=0;
		Pipe<P<_Payload>,MESSAGE_INPUT_QUEUE_BLOCK_SIZE>	*buffer=NULL;
		P<_Payload>	*_p;
		while(!node->_shutdown){

			node->inputSync->acquire();

			if(recvThread++>=node->recvThreads.count()){

				recvThread=0;
				buffer=&node->messageInputQueue;
			}else
				buffer=&node->recvThreads[recvThread]->buffer;

			_p=buffer->pop(false);
			if(!_p)
				continue;

			if((*_p)->category()==_Payload::CONTROL)
				node->processControlMessage(*_p);

			node->pushJobs(*_p);
			*_p=NULL;
		}

		return	0;
	}
}


#endif