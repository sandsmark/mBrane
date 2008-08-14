//	executing.cpp
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

#include	"node.h"


namespace	mBrane{

	Executing::Executing():jobFeeder(NULL){
	}

	Executing::~Executing(){

		for(uint32	i=0;i<xThreads.count();i++)
			delete	xThreads[i];
		for(uint32	i=0;i<sThreads.count();i++)
			delete	sThreads[i];

		if(jobFeeder)
			delete	jobFeeder;
	}

	bool	Executing::loadConfig(XMLNode	&n){

		XMLNode	threads=n.getChildNode("Threads");
		if(!!threads){

			const	char	*tc=n.getAttribute("thread_count");
			if(!tc){

				std::cout<<"Error: "<<n.getName()<<"::Threads::thread_count is missing\n";
				return	false;
			}
			threadCount=atoi(tc);
			if(threadCount>0	&&	threadCount<=512)
				xThreads.alloc(threadCount);
			else{
				
				std::cout<<"Error: thread count must be in [1,512]\n";
				return	false;
			}
		}
		return	true;
	}

	void	Executing::start(){

		xThreads.alloc(threadCount);
		for(uint32	i=0;i<xThreads.count();i++)
			xThreads[i]=Thread::New<Thread>(Xec,this);
		sThreads.alloc(threadCount);
		for(uint32	i=0;i<sThreads.count();i++)
			sThreads[i]=Thread::New<Thread>(Xec,this);

		jobFeeder=Thread::New<Thread>(FeedJobs,this);
	}

	void	Executing::shutdown(){

		Thread::Wait(xThreads.data(),xThreads.count());
		Thread::Wait(sThreads.data(),sThreads.count());
		Thread::Wait(jobFeeder);
	}

	////////////////////////////////////////////////////////////////////////////////////////////////

	uint32	thread_function_call	Executing::Xec(void	*args){

		Node	*node=((Node	*)args);

		while(!node->_shutdown){

			//	TODO:	block on CS, pop job, peek at next, get priority, assign to next waiting xThread
			//			if last unlock a support and when return block on support sem (i.e. become  a sThread)
			//			push job in another xThread if already processing for the same crank and no preemption: in that case, pop next job
			//			preempt as required
		}

		return	0;
	}

	uint32	thread_function_call	Executing::Support(void	*args){

		Node	*node=((Node	*)args);

		while(!node->_shutdown){

			//	TODO: block on sem
		}

		return	0;
	}

	uint32	thread_function_call	Executing::FeedJobs(void	*args){

		Node	*node=((Node	*)args);

		while(!node->_shutdown){

			//	TODO:	then block Messaging::orderedMessageSync, pop msg from Messaging::orderedMessage,
			//			if ctrl msg, process it
			//			read pub-sub and push new job
			//			set the priority of the first blocked xThread to the first job's crank's
		}

		return	0;
	}

/*

			//	find local receiving cranks (from pub-sub structure); insert {p,crank} pairs in pipeline
			Array<PublishingSubscribing::NodeEntry>	*nodeEntries=node->getNodeEntries((*_p)->cid(),((_ControlMessage	*)*_p)->mid());
			if(nodeEntries){	//	else: mid has never been subscribed for before

				if(nodeEntries->operator[](node->_ID).activationCount){

					List<_Crank	*>	*l=nodeEntries->get(node->_ID)->cranks;
					if(l){

						List<_Crank	*>::Iterator	i;
						for(i=l->begin();i!=l->end();i++){

							if(((_Crank	*)i)->active())
								node->insertMessage(*_p);
						}
					}
				}
			}
*/
}