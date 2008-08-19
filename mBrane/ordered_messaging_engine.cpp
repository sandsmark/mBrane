//	ordered_messaging_engine.cpp
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

#if	defined	ORDERED_MESSAGING_ENGINE


#include	"ordered_messaging_engine.h"
#include	"node.h"
#include	"..\Core\control_messages.h"


#define	INITIAL_MESSAGING_LATENCY	1000

using	namespace	mBrane::sdk::payloads;

namespace	mBrane{

	OrderedMessagingEngine::OrderedMessagingEngine():orderThread(NULL),latency(INITIAL_MESSAGING_LATENCY),orderedMessageUpdateThread(NULL),refCount(0),jobFeeder(NULL){

		orderedMessageSync=new	Semaphore(0,65535);
	}

	OrderedMessagingEngine::~OrderedMessagingEngine(){

		if(orderThread)
			delete	orderThread;
		if(orderedMessageUpdateThread)
			delete	orderedMessageUpdateThread;
		if(jobFeeder)
			delete	jobFeeder;
		delete	orderedMessageSync;
	}
	
	void	OrderedMessagingEngine::start(){

		orderThread=Thread::New<Thread>(OrderMessages,this);
		orderedMessageUpdateThread=Thread::New<Thread>(UpdateMessageOrdering,this);
		jobFeeder=Thread::New<Thread>(FeedJobs,this);
	}

	void	OrderedMessagingEngine::shutdown(){

		Thread::Wait(orderThread);
	}

	inline	void	OrderedMessagingEngine::insertMessage(P<_Payload>	&p){

		int64	now=Time::Get();
		uint32	count=0;
		List<P<_Payload> >::Iterator	i;

		if(!orderedMessages.elementCount()){

			orderedMessages.addElementHead(p);
			if(now-p->send_ts()<latency){

				ref=orderedMessages.begin();
				refCount=1;
			}else
				orderedMessageSync->release();
			goto	resetTimer;
		}

		for(i=orderedMessages.begin();i!=ref;i++){

			if(now-((P<_Payload>)i)->send_ts()<latency){

				if(p->send_ts()<((P<_Payload>)i)->send_ts()){	//	p older than i

					count++;
					continue;
				}else{

					i.insertBefore(p);
					for(;now-((P<_Payload>)i)->send_ts()<latency;i++)	//	find the new ref
						count++;
					goto	updateRef;
				}
			}else{	//	i is the first message over the latency: ref or before

				if(p->send_ts()>((P<_Payload>)i)->send_ts()){	//	p is younger than i.

					if(now-p->send_ts()<latency){	//	p is the last message under the latency (i.e. the new ref), 

						i.insertBefore(p);
						goto	updateRef;
					}
					//	p is the first message over the latency
					i.insertBefore(p);
					orderedMessageSync->release();
					goto	resetTimer;
				}
				//	p is older than i: late message
				//	TODO:	flag
				i.insertBefore(p);
				orderedMessageSync->release();
				goto	resetTimer;
			}
		}
updateRef:
		ref=--i;	//	insertions are "before"
		if(uint32	c=refCount-count+1)
			orderedMessageSync->release(c);
		refCount=count+1;	//	+1: there was an insertion
resetTimer:
		orderedMessageUpdateTimer.start(latency);
	}

	////////////////////////////////////////////////////////////////////////////////////////////////

	uint32	thread_function_call	OrderedMessagingEngine::UpdateMessageOrdering(void	*args){

		Node	*node=(Node	*)args;

		List<P<_Payload> >::Iterator	i;
		while(!node->_shutdown){

			node->orderedMessageUpdateTimer.wait();
			int64	now=Time::Get();
			uint32	count=0;
			for(i=node->orderedMessages.begin();now-((P<_Payload>)i)->send_ts()<node->latency;i++,count++);
			if(uint32	c=node->refCount-count)
				node->orderedMessageSync->release(c);
			node->ref=--i;
			node->refCount=count;
			node->orderedMessageUpdateTimer.start(node->latency);
		}

		return	0;
	}

	uint32	thread_function_call	OrderedMessagingEngine::OrderMessages(void	*args){

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

			node->insertMessage(*_p);
			*_p=NULL;
		}

		return	0;
	}

	uint32	thread_function_call	OrderedMessagingEngine::FeedJobs(void	*args){

		Node	*node=((Node	*)args);

		while(!node->_shutdown){

			node->orderedMessageSync->acquire();
			List<P<_Payload> >::Iterator	i=node->orderedMessages.end();
			P<_Payload>	p=(P<_Payload>	)i;
			i.removeJumpPrevious();

			if(p->isControlMessage())
				node->processControlMessage(p);

			node->pushJobs(p);
		}

		return	0;
	}
}


#endif