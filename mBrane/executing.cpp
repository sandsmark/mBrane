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

				std::cout<<"Error: "<<n.getName()<<"::Threads::thread_count is missing\n";
				return	false;
			}
			threadCount=atoi(tc);
			if(threadCount==0	||	threadCount>512){
				
				std::cout<<"Error: thread count must be in [1,512]\n";
				return	false;
			}
		}
		return	true;
	}

	void	Executing::start(){

		xThreads.alloc(threadCount);
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

	uint32	thread_function_call	XThread::Xec(void	*args){

		XThread	*_this=((XThread	*)args);

		static	uint32	xCount=0;
		if(++xCount<=_this->node->xThreads.count())
			goto	xec;
sleep:
		_this->node->supportSync->acquire();
xec:
		while(!_this->node->_shutdown){

			_this->node->jobCS.enter();
			Job	*j=_this->node->jobs.pop();
			_this->node->jobCS.leave();

			P<_Payload>	p=j->p;
			j->p=NULL;
			_Module	*module=j->m->module;

			switch(_this->work(p,module)){
			case	DONE:
			case	CONTINUE:	continue;
			case	BLOCKED:	goto	sleep;
			}
		}

		return	0;
	}

	XThread::XThread(Node	*n):Thread(),node(n),module(NULL),wasBlocked(false){

		sync=new	Semaphore(0,1);
	}

	XThread::~XThread(){

		delete	sync;
	}

	inline	void	XThread::block(){

		if(!wasBlocked){

			wasBlocked=true;
			node->supportSync->release();
		}
	}

	inline	XThread::Status	XThread::work(_Payload	*p,_Module	*m){

		if(m->processor){

			switch(m->dispatch(p)){
			case	_Module::WAIT:	//	wait for an exiting xThread to finish; recurse (in case that thread was preempting yet another one)
				block();
				((XThread	*)m->processor)->sync->acquire();
				return	work(p,m);
			case	_Module::PREEMPT:{
				sync->acquire();
				XThread	*preempted=(XThread	*)module->processor;
				preempted->suspend();
				preempted->block();
				m->processor=this;
				module=m;
				if(p->category()==_Payload::STREAM)
					m->notify(p->operator	payloads::_StreamData	*()->sid(),p);
				else
					m->notify(p);
				module=NULL;
				m->processor=preempted;
				preempted->module=m;
				preempted->resume();
				sync->release();
				goto	exit;
			}case	_Module::DISCARD:
				return	CONTINUE;
			}
		}
		
		sync->acquire();
		(module=m)->processor=this;
		if(p->category()==_Payload::STREAM)
			module->notify(p->operator	payloads::_StreamData	*()->sid(),p);
		else
			module->notify(p);
		module->processor=NULL;
		module=NULL;
		sync->release();
exit:
		if(wasBlocked){

			wasBlocked=false;	
			return	BLOCKED;
		}
		return	DONE;
	}
}