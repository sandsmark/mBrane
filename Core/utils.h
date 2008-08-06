// utils.h
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

#ifndef mBrane_utils_h
#define mBrane_utils_h

#include	"types.h"


namespace	mBrane{

	class	dll	SharedLibrary{
	private:
		shared_object	library;
		SharedLibrary();
		SharedLibrary	*load(const	char	*fileName);
	public:
		static	SharedLibrary	*New(const	char	*fileName);
		~SharedLibrary();
		template<typename	T>	T	getFunction(const	char	*functionName);
	};

	class	dll	Thread{
	private:
		thread		_thread;
		Thread();
	public:
		static	Thread	*New(thread_function	f,void	*args);
		static	void	Wait(Thread	**threads,uint32	threadCount);
		static	void	Sleep(int64	d);
		~Thread();
	};

	class	dll	Time{
	public:
		static	int64	Get();	//	in ms since 01/01/1970
	};

	class	dll	Host{
	public:
		static	uint8	Name(char	*name);	//	name size=255; return the actual size
	};

	class	dll	Semaphore{
	private:
		semaphore	s;
		static	const	uint32	Infinite;
	public:
		Semaphore(uint32	initialCount,uint32	maxCount);
		~Semaphore();
		bool	acquire(uint32	timeout=Infinite);	//	returns true if timedout
		void	release();
		void	reset();
	};

	class	dll	Mutex{
	private:
		mutex	m;
		static	const	uint32	Infinite;
	public:
		Mutex();
		~Mutex();
		bool	acquire(uint32	timeout=Infinite);	//	returns true if timedout
		void	release();
	};

	class	dll	CriticalSection{
	private:
		critical_section	cs;
	public:
		CriticalSection();
		~CriticalSection();
		void	enter();
		void	leave();
	};

	class	dll	Timer{
	private:
		timer	t;
		static	const	uint32	Infinite;
	public:
		Timer();
		~Timer();
		void	start(uint32	deadline);
		bool	wait(uint32	timeout=Infinite);	//	returns true if timedout
	};
}


#include	"utils.tpl.cpp"


#endif