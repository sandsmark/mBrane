// utils.cpp
//
// Author: Eric Nivel
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

#include	"utils.h"

#include	<iostream>

#if defined	WINDOWS
	#include <sys/timeb.h>
	#include <time.h>
#elif defined LINUX
#elif defined OSX
#endif


namespace	mBrane{

	SharedLibrary	*SharedLibrary::New(const	char	*fileName){

		SharedLibrary	*sl=new	SharedLibrary();
		if(sl->load(fileName))
			return	sl;
		else{
		
			delete	sl;
			return	NULL;
		}
	}

	SharedLibrary::SharedLibrary():library(NULL){
	}

	SharedLibrary::~SharedLibrary(){
#if defined	WINDOWS
		if(library)
			FreeLibrary(library);
#elif defined LINUX
#elif defined OSX
#endif
	}

	SharedLibrary	*SharedLibrary::load(const	char	*fileName){
#if defined	WINDOWS
		library=LoadLibrary(TEXT(fileName));
		if(!library){

			DWORD	error=GetLastError();
			std::cout<<"Error: unable to load shared library "<<fileName<<" :"<<error<<std::endl;
			return	NULL;
		}
#elif defined LINUX
#elif defined OSX
#endif
		return	this;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////

	Thread	*Thread::New(thread_function	f,void	*args){

		Thread	*t=new	Thread();
#if defined	WINDOWS
		if(t->_thread=CreateThread(NULL,0,f,args,0,NULL))
#elif defined LINUX
#elif defined OSX
#endif
			return	t;
		delete	t;
		return	NULL;
	}

	void	Thread::Wait(Thread	**threads,uint32	threadCount){

		if(!threads)
			return;
#if defined	WINDOWS
		for(uint32	i=0;i<threadCount;i++)
			WaitForSingleObject(threads[i]->_thread,INFINITE);
#elif defined LINUX
#elif defined OSX
#endif
	}

	Thread::Thread(){
	}

	Thread::~Thread(){
#if defined	WINDOWS
		ExitThread(0);
#elif defined LINUX
#elif defined OSX
#endif
	}

	////////////////////////////////////////////////////////////////////////////////////////////////

	int64	Time::Get(){
#if defined	WINDOWS
		struct	_timeb	local_time;
		_ftime(&local_time);
		return	(int64)local_time.time*1000+local_time.millitm;
#elif defined LINUX
#elif defined OSX
#endif
	}

	////////////////////////////////////////////////////////////////////////////////////////////////

	void	Host::Name(char	*name){
#if defined	WINDOWS
		uint32	s=255;
		GetComputerName(name,&s);
#elif defined LINUX
#elif defined OSX
#endif
	}
}