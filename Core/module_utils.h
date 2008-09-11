//	module_utils.h
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

#ifndef mBrane_sdk_module_utils_h
#define mBrane_sdk_module_utils_h

#include	"module.h"


namespace	mBrane{
	class	XThread;
	namespace	sdk{
		namespace	module{

			class	dll	ModuleUtils{
			protected:
				_Module	*module;
				ModuleUtils(_Module	*c);
			};

			class	dll	Semaphore:
			public	ModuleUtils,
			protected	mBrane::Semaphore{
			public:
				Semaphore(_Module	*c,uint32	initialCount,uint32	maxCount);
				~Semaphore();
				bool	acquire(uint32	timeout=Infinite);	//	returns true if timedout
				void	release(uint32	count=1);
				void	reset();
			};

			class	dll	Mutex:
			public	ModuleUtils,
			protected	mBrane::Mutex{
			public:
				Mutex(_Module	*c);
				~Mutex();
				bool	acquire(uint32	timeout=Infinite);	//	returns true if timedout
				void	release();
			};

			class	dll	CriticalSection:
			public	ModuleUtils,
			protected	mBrane::CriticalSection{
			public:
				CriticalSection(_Module	*c);
				~CriticalSection();
				void	enter();
				void	leave();
			};

			class	dll	Timer:
			public	ModuleUtils,
			protected	mBrane::Timer{
			public:
				Timer(_Module	*c);
				~Timer();
				void	start(uint32	deadline,uint32	period=0);	//	in ms
				bool	wait(uint32	timeout=Infinite);	//	returns true if timedout
				bool	wait(uint64	&us,uint32	timeout=Infinite);	//	idem; updates the us actually spent
			};
		}
	}
}


#endif
