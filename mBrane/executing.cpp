//	executing.cpp
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

#include	"node.h"
#include	"../Core/control_messages.h"


namespace	mBrane{

	Executing::Executing(){

		supportSync=new	Semaphore(0,65535);
	}

	Executing::~Executing(){

		for(uint32	i=0;i<xThreads.count();i++)
			delete	xThreads[i];
		if(supportSync)
			delete	supportSync;
	}

	bool	Executing::loadConfig(XMLNode	&n){

		XMLNode	threads=n.getChildNode("Threads");
		if(!!threads){

			const	char	*tc=threads.getAttribute("thread_count");
			if(!tc){

				std::cout<<"> Error: "<<n.getName()<<"::Threads::thread_count is missing"<<std::endl;
				return	false;
			}
			threadCount=atoi(tc);
			if(threadCount==0	||	threadCount>512){
				
				std::cout<<"> Error: thread count must be in [1,512]"<<std::endl;
				return	false;
			}
		}
		return	true;
	}

	void	Executing::start(){

		xThreads.alloc(threadCount*2);	//	twice the requested amount: to keep extra threads in our sleeve when some are waiting
		for(uint32	i=0;i<xThreads.count();i++){

			XThread	*t=new	XThread((Node	*)this);
			xThreads[i]=t;
			t->start(XThread::Xec);
		}
	}

	void	Executing::shutdown(){

		Thread::Wait(xThreads.data(),xThreads.count());
	}

	////////////////////////////////////////////////////////////////////////////////////////////////

	thread_ret thread_function_call	XThread::Xec(void	*args){

		XThread	*_this=((XThread	*)args);

		static	uint32	xCount=0;
		if(++xCount>_this->node->threadCount){

check_in:	_this->node->supportSync->acquire();
			_this->wasSupporting=true;
		}

		while(_this->node->isRunning()){

			Job	*j=_this->node->jobs.pop();
			j->m->sync->acquire();	//	forces threads processing the same module to run in the order of message arrival, until reaching a preemption point (see work).
			_this->work(j->p,j->m);
			j->p=NULL;

			if(_this->wasSupporting)
				goto	check_in;
		}
		thread_ret_val(0);
	}

	XThread::XThread(Node	*n):Thread(),node(n),wasSupporting(false){

		sync=new	Semaphore(0,1);
	}

	XThread::~XThread(){

		delete	sync;
	}

	inline	void	XThread::work(_Payload	*p,_Module	*m){
		
		XThread	*currentProcessor=(XThread	*)m->processor;
		if(currentProcessor){

			switch(m->dispatch(p)){
			case	_Module::WAIT:
				node->supportSync->release();			// before waiting, unlock a supporting thread to run instead of this.
				if(currentProcessor)					// could be NULL if currentProcessor just finished.
					currentProcessor->sync->acquire();	// wait for the currentProcessor to finish; will be unlocked by currentProcessor (see sync->release() below).
				work(p,m);								// recurse to ask again what to do with p (ex: in case currentProcessor was preempting yet another one).
				return;
			case	_Module::PREEMPT:
				if(currentProcessor)					// could be NULL if currentProcessor just finished.
					currentProcessor->suspend();
				m->processor=this;
				m->sync->release();						// preemption point.
				if(p->category()==_Payload::STREAM)
					m->notify(((_StreamData	*)p)->sid(),p);
				else
					m->notify(p);
				if(m->processor=currentProcessor)		// could be NULL if currentProcessor just finished.
					currentProcessor->resume();
				sync->release();						// release a potential waiting thread.
				return;
			case	_Module::DISCARD:
				return;
			}
		}
		
		m->processor=this;
		m->sync->release();	// preemption point.
		if(p->category()==_Payload::STREAM)
			m->notify(((_StreamData	*)p)->sid(),p);
		else
			m->notify(p);
		m->processor=NULL;
		sync->release();	// release a potential waiting thread.
		return;
	}
}
