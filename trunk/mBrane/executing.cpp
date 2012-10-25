//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/ //_/_/ //_/_/  executing.cpp //_/_/ //_/_/  Eric Nivel //_/_/  Center for Analysis and Design of Intelligent Agents //_/_/    Reykjavik University, Menntavegur 1, 101 Reykjavik, Iceland //_/_/    http://cadia.ru.is
//_/_/  Copyright©2012 //_/_/ //_/_/  This software was developed by the above copyright holder as part of  //_/_/  the HUMANOBS EU research project, in collaboration with the  //_/_/  following parties:
//_/_/   //_/_/  Autonomous Systems Laboratory //_/_/    Technical University of Madrid, Spain //_/_/    http://www.aslab.org/ //_/_/ //_/_/  Communicative Machines //_/_/    Edinburgh, United Kingdom //_/_/    http://www.cmlabs.com/ //_/_/ //_/_/  Istituto Dalle Molle di Studi sull'Intelligenza Artificiale //_/_/    University of Lugano and SUPSI, Switzerland //_/_/    http://www.idsia.ch/ //_/_/ //_/_/  Institute of Cognitive Sciences and Technologies //_/_/    Consiglio Nazionale delle Ricerche, Italy //_/_/    http://www.istc.cnr.it/
//_/_/
//_/_/  Dipartimento di Ingegneria Informatica
//_/_/    University of Palermo, Italy
//_/_/    http://roboticslab.dinfo.unipa.it/index.php/Main/HomePage
//_/_/ //_/_/
//_/_/  --- HUMANOBS Open-Source BSD License, with CADIA Clause v 1.0 ---
//_/_/
//_/_/  Redistribution and use in source and binary forms, with or without 
//_/_/  modification, is permitted provided that the following conditions 
//_/_/  are met:
//_/_/
//_/_/  - Redistributions of source code must retain the above copyright 
//_/_/    and collaboration notice, this list of conditions and the 
//_/_/    following disclaimer.
//_/_/
//_/_/  - Redistributions in binary form must reproduce the above copyright 
//_/_/    notice, this list of conditions and the following
//_/_/    disclaimer in the documentation and/or other materials provided 
//_/_/    with the distribution.
//_/_/
//_/_/  - Neither the name of its copyright holders nor the names of its 
//_/_/    contributors may be used to endorse or promote products 
//_/_/    derived from this software without specific prior written permission.
//_/_/
//_/_/  - CADIA Clause: The license granted in and to the software under this 
//_/_/     agreement is a limited-use license. The software may not be used in 
//_/_/     furtherance of: 
//_/_/     (i) intentionally causing bodily injury or severe emotional distress 
//_/_/         to any person; 
//_/_/     (ii) invading the personal privacy or violating the human rights of 
//_/_/         any person; or 
//_/_/     (iii) committing  or preparing for any act of war.
//_/_/
//_/_/  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
//_/_/  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
//_/_/  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR 
//_/_/  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
//_/_/  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
//_/_/  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
//_/_/  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, 
//_/_/  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY 
//_/_/  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
//_/_/  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
//_/_/  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//_/_/
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

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

		xThreads.alloc(threadCount*1);	//	twice the requested amount: to keep extra threads in our sleeve when some are waiting
		for(uint32	i=0;i<xThreads.count();i++){

			XThread	*t=new	XThread((Node	*)this);
			xThreads[i]=t;
			if(i>=threadCount)
				t->wasSupporting=true;
			t->start(XThread::Xec);
		}
	}

	void	Executing::shutdown(){

		Thread::TerminateAndWait(xThreads.data(),xThreads.count());
	}

	////////////////////////////////////////////////////////////////////////////////////////////////

	thread_ret thread_function_call	XThread::Xec(void	*args){

		XThread	*_this=((XThread	*)args);

		if(_this->wasSupporting)
check_in:	_this->node->supportSync->acquire();

		while(_this->node->isRunning()){

			Job	j=_this->node->jobs.pop();
			j.m->sync->acquire();	//	forces threads processing the same module to run in the order of message arrival, until reaching a preemption point (see work).
			if (j.p == NULL){
				printf("Xec Payload NULL!!!\n");fflush(stdout);
			}else{
				//std::cout<<"xec got job: "<<j.p->cid()<<" "<<std::hex<<_this<<std::dec<<std::endl;fflush(stdout);
				_this->work(j.p,j.m);
				}
			j.p=NULL;

			if(_this->wasSupporting)
				goto	check_in;
		}
		thread_ret_val(0);
	}

	XThread::XThread(Node	*n):Thread(),FastSemaphore(0,1),node(n),wasSupporting(false){
	}

	XThread::~XThread(){
	}

	inline	void	XThread::work(_Payload	*p,_Module	*m){
//static	uint32	w=0;
		XThread	*currentProcessor=(XThread	*)m->processor;
		if(currentProcessor){

			switch(m->dispatch(p)){
			case	_Module::WAIT:
				node->supportSync->release();			// before waiting, unlock a supporting thread to run instead of this.
				if(currentProcessor)					// could be NULL if currentProcessor just finished.
					currentProcessor->acquire();		// wait for the currentProcessor to finish; will be unlocked by currentProcessor (see release() below).
				work(p,m);								// recurse to ask again what to do with p (ex: in case currentProcessor was preempting yet another one).
				return;
			case	_Module::PREEMPT:
				if(currentProcessor)					// could be NULL if currentProcessor just finished.
					currentProcessor->suspend();
				m->processor=this;
				m->sync->release();						// preemption point.
				if(p->category()==_Payload::STREAM)
					m->notify(p->as_StreamData()->sid(),p);
				else
					m->notify(p);
				if(m->processor=currentProcessor)		// could be NULL if currentProcessor just finished.
					currentProcessor->resume();
				release();								// release a potential waiting thread.
				return;
			case	_Module::DISCARD:
				return;
			}
		}
		
		m->processor=this;
		m->sync->release();	// preemption point.
		if(p->category()==_Payload::STREAM)
			m->notify(p->as_StreamData()->sid(),p);
		else{

			//std::cout<<"xec notified job: "<<p->cid()<<" "<<std::hex<<this<<std::dec<<std::endl;fflush(stdout);
			//std::cout<<"Executing job: "<<++w<<"|"<<p->cid()<<std::endl;
			m->notify(p);
		}
		m->processor=NULL;
		release();	// release a potential waiting thread.
		return;
	}
}
