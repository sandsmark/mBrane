//	utils.tpl.cpp
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

#include	<iostream>


namespace	mBrane{

	template<typename	T>	T	SharedLibrary::getFunction(const	char	*functionName){
		T	function=NULL;
#if defined	WINDOWS
		if(library){ 

			function=(T)GetProcAddress(library,functionName);
			if(!function){

				DWORD	error=GetLastError();
				std::cout<<"GetProcAddress Error: "<<error<<std::endl;
			}
		}
#elif defined LINUX
#elif defined OSX
#endif
		return	function;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////

	template<class	T>	T	*Thread::New(thread_function	f,void	*args){

		T	*t=new	T();
#if defined	WINDOWS
		if(t->_thread=CreateThread(NULL,0,f,args,0,NULL))
#elif defined LINUX
#elif defined OSX
#endif
			return	t;
		delete	t;
		return	NULL;
	}
}
