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

#include	"executing.h"


namespace	mBrane{

	Executing::Executing():__shutdown(false){
	}

	Executing::~Executing(){

		for(uint32	i=0;i<crankThreads.count();i++)
			delete	crankThreads[i];
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
			if(threadCount>0)
				crankThreads.alloc(threadCount);
		}
		return	true;
	}

	void	Executing::start(){
	}

	void	Executing::shutdown(){

		if(__shutdown)
			return;	
		__shutdown=true;
		Thread::Wait(crankThreads.data(),crankThreads.count());
	}

	void	Executing::addCrank(_Crank	*c){

		crankCS.enter();
		cranks.addElementTail(c);
		crankCS.leave();

		c->start();

		if(threadCount==0){

			if(crankThreads.count()<cranks.elementCount())
				goto	addThread;
		}else	if(crankThreads.count()<cranks.elementCount()	&&	crankThreads.count()<threadCount)
			goto	addThread;
		return;
addThread:
		CrankExecutionUnitArgs	args;
		args.node=this;
		args.threadID=crankThreads.count();
		crankThreads[crankThreads.count()]=Thread::New(CrankExecutionUnit,&args);
	}

	void	Executing::removeCrank(_Crank	*c){

		crankCS.enter();
		cranks.removeElement(c);
		crankCS.leave();

		c->stop();
	}

	_Crank	*Executing::getCrank(uint32	threadID){

		uint32	crankCount;
		crankCS.enter();
		crankCount=cranks.elementCount();
		
		if(threadID>=crankCount){	//	more threads than cranks

			crankCS.leave();
			return	NULL;
		}

		//	TODO:	add scheduling data in _Crank (msg count and next msg's priority, or next msg itself), implement the crank election here
		crankCS.leave();
	}

	////////////////////////////////////////////////////////////////////////////////////////////////

	uint32	thread_function_call	Executing::CrankExecutionUnit(void	*args){

		Executing	*node=((CrankExecutionUnitArgs	*)args)->node;
		uint32		ID=((CrankExecutionUnitArgs	*)args)->threadID;

		_Crank		*crank;
		P<_Payload>	*p;
		while(!node->__shutdown){

			if(!(crank=node->getCrank(ID)))
				return	0;
			do
				p=crank->pop(false);
			while(!*p);	//	*p can be NULL (when preview returns true)
			(*p)->recv_ts()=Time::Get();
			crank->notify(*p);
			*p=NULL;
		}

		return	0;
	}
}