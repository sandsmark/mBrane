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
	}

	bool	Executing::loadConfig(XMLNode	&n){

		XMLNode	threads=n.getChildNode("Threads");
		if(!!threads){

			const	char	*tc=n.getAttribute("thread_count");
			if(!tc){

				std::cout<<"Error: "<<n.getName()<<"::Threads::thread_count is missing\n";
				return	false;
			}
			crankThreads.alloc(atoi(tc));
		}
		return	true;
	}

	void	Executing::start(){

		//	TODO:	start thread (CEU) pool;
	}

	void	Executing::shutdown(){

		if(__shutdown)
			return;	
		__shutdown=true;
		Thread::Wait(crankThreads.data(),crankThreads.count());
	}

	////////////////////////////////////////////////////////////////////////////////////////////////

	uint32	thread_function_call	Executing::CrankExecutionUnit(void	*args){

		Executing	*node=((CrankThreadArgs	*)args)->n;
		_Crank		*crank=((CrankThreadArgs	*)args)->c;

		crank->start();

		while(!node->__shutdown){

			if(!crank->alive())
				break;

			P<_Payload>	*p;
			if(crank->run()){

				do
					p=crank->pop();
				while(!*p);	//	*p can be NULL (when preview returns true)
				(*p)->recv_ts()=Time::Get();
				crank->notify(*p);
				*p=NULL;
			}else{

loop:			p=crank->pop(false);
				if(p){

					if(!*p)	//	*p can be NULL (when preview returns true)
						goto	loop;
					(*p)->recv_ts()=Time::Get();
					crank->notify(*p);
					*p=NULL;
				}
			}
		}

		crank->stop();
		delete	crank;

		return	0;
	}
}