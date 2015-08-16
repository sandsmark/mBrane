/*
* HUMANOBS - mBrane
*
* Eric Nivel
* Center for Analysis and Design of Intelligent Agents
*   Reykjavik University, Menntavegur 1, 101 Reykjavik, Iceland
*   http://cadia.ru.is
* Copyright(c)2012
*
* This software was developed by the above copyright holder as part of
* the HUMANOBS EU research project, in collaboration with the
* following parties:
*
* Autonomous Systems Laboratory
*   Technical University of Madrid, Spain
*   http://www.aslab.org/
*
* Communicative Machines
*   Edinburgh, United Kingdom
*   http://www.cmlabs.com/
*
* Istituto Dalle Molle di Studi sull'Intelligenza Artificiale
*   University of Lugano and SUPSI, Switzerland
*   http://www.idsia.ch/
*
* Institute of Cognitive Sciences and Technologies
*   Consiglio Nazionale delle Ricerche, Italy
*   http://www.istc.cnr.it/
*
* Dipartimento di Ingegneria Informatica
*   University of Palermo, Italy
*   http://roboticslab.dinfo.unipa.it/index.php/Main/HomePage
*
*
* --- HUMANOBS Open-Source BSD License, with CADIA Clause v 1.0 ---
*
* Redistribution and use in source and binary forms, with or without
* modification, is permitted provided that the following conditions
* are met:
*
* - Redistributions of source code must retain the above copyright
* and collaboration notice, this list of conditions and the
* following disclaimer.
*
* - Redistributions in binary form must reproduce the above copyright
* notice, this list of conditions and the following
* disclaimer in the documentation and/or other materials provided
* with the distribution.
*
* - Neither the name of its copyright holders nor the names of its
* contributors may be used to endorse or promote products
* derived from this software without specific prior written permission.
*
* - CADIA Clause: The license granted in and to the software under this
* agreement is a limited-use license. The software may not be used in
* furtherance of:
* (i) intentionally causing bodily injury or severe emotional distress
* to any person;
* (ii) invading the personal privacy or violating the human rights of
* any person; or
* (iii) committing or preparing for any act of war.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
* "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
* LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
* A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
* OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
* SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
* LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
* DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
* THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
* OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include	"node.h"
#include	"../Core/control_messages.h"


namespace	mBrane
{

Executing::Executing()
{
    supportSync = new	Semaphore(0, 65535);
}

Executing::~Executing()
{
    for (uint32_t	i = 0; i < xThreads.count(); i++) {
        delete	xThreads[i];
    }

    if (supportSync) {
        delete	supportSync;
    }
}

bool	Executing::loadConfig(XMLNode	&n)
{
    XMLNode	threads = n.getChildNode("Threads");

    if (!!threads) {
        const	char	*tc = threads.getAttribute("thread_count");

        if (!tc) {
            std::cout << "> Error: " << n.getName() << "::Threads::thread_count is missing" << std::endl;
            return	false;
        }

        threadCount = atoi(tc);

        if (threadCount == 0	||	threadCount > 512) {
            std::cout << "> Error: thread count must be in [1,512]" << std::endl;
            return	false;
        }
    }

    return	true;
}

void	Executing::start()
{
    xThreads.alloc(threadCount * 1);	//	twice the requested amount: to keep extra threads in our sleeve when some are waiting

    for (uint32_t	i = 0; i < xThreads.count(); i++) {
        XThread	*t = new	XThread((Node *)this);
        xThreads[i] = t;

        if (i >= threadCount) {
            t->wasSupporting = true;
        }

        t->start(XThread::Xec);
    }
}

void	Executing::shutdown()
{
    Thread::TerminateAndWait(xThreads.data(), xThreads.count());
}

////////////////////////////////////////////////////////////////////////////////////////////////

thread_ret thread_function_call	XThread::Xec(void	*args)
{
    XThread	*_this = ((XThread *)args);

    if (_this->wasSupporting) {
check_in:
        _this->node->supportSync->acquire();
    }

    while (_this->node->isRunning()) {
        Job	j = _this->node->jobs.pop();
        j.m->sync->acquire();	//	forces threads processing the same module to run in the order of message arrival, until reaching a preemption point (see work).

        if (j.p == NULL) {
            printf("Xec Payload NULL!!!\n");
            fflush(stdout);
        } else {
            //std::cout<<"xec got job: "<<j.p->cid()<<" "<<std::hex<<_this<<std::dec<<std::endl;fflush(stdout);
            _this->work(j.p, j.m);
        }

        j.p = NULL;

        if (_this->wasSupporting) {
            goto	check_in;
        }
    }

    thread_ret_val(0);
}

XThread::XThread(Node	*n): Thread(), FastSemaphore(0, 1), node(n), wasSupporting(false)
{
}

XThread::~XThread()
{
}

inline	void	XThread::work(_Payload	*p, _Module	*m)
{
//static	uint32_t	w=0;
    XThread	*currentProcessor = (XThread *)m->processor;

    if (currentProcessor) {
        switch (m->dispatch(p)) {
        case	_Module::WAIT:
            node->supportSync->release();			// before waiting, unlock a supporting thread to run instead of this.

            if (currentProcessor) {				// could be NULL if currentProcessor just finished.
                currentProcessor->acquire();    // wait for the currentProcessor to finish; will be unlocked by currentProcessor (see release() below).
            }

            work(p, m);								// recurse to ask again what to do with p (ex: in case currentProcessor was preempting yet another one).
            return;

        case	_Module::PREEMPT:
            if (currentProcessor) {				// could be NULL if currentProcessor just finished.
                currentProcessor->suspend();
            }

            m->processor = this;
            m->sync->release();						// preemption point.

            if (p->category() == _Payload::STREAM) {
                m->notify(p->as_StreamData()->sid(), p);
            } else {
                m->notify(p);
            }

            if (m->processor = currentProcessor) {	// could be NULL if currentProcessor just finished.
                currentProcessor->resume();
            }

            release();								// release a potential waiting thread.
            return;

        case	_Module::DISCARD:
            return;
        }
    }

    m->processor = this;
    m->sync->release();	// preemption point.

    if (p->category() == _Payload::STREAM) {
        m->notify(p->as_StreamData()->sid(), p);
    } else {
        //std::cout<<"xec notified job: "<<p->cid()<<" "<<std::hex<<this<<std::dec<<std::endl;fflush(stdout);
        //std::cout<<"Executing job: "<<++w<<"|"<<p->cid()<<std::endl;
        m->notify(p);
    }

    m->processor = NULL;
    release();	// release a potential waiting thread.
    return;
}
}
