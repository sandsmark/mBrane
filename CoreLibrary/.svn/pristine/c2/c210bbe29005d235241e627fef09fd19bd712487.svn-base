//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
//_/_/
//_/_/ HUMANOBS - CoreLibrary
//_/_/
//_/_/ Eric Nivel, Thor List
//_/_/ Center for Analysis and Design of Intelligent Agents
//_/_/   Reykjavik University, Menntavegur 1, 101 Reykjavik, Iceland
//_/_/   http://cadia.ru.is
//_/_/ Copyright(c)2012
//_/_/
//_/_/ This software was developed by the above copyright holder as part of 
//_/_/ the HUMANOBS EU research project, in collaboration with the 
//_/_/ following parties:
//_/_/ 
//_/_/ Autonomous Systems Laboratory
//_/_/   Technical University of Madrid, Spain
//_/_/   http://www.aslab.org/
//_/_/
//_/_/ Communicative Machines
//_/_/   Edinburgh, United Kingdom
//_/_/   http://www.cmlabs.com/
//_/_/
//_/_/ Istituto Dalle Molle di Studi sull'Intelligenza Artificiale
//_/_/   University of Lugano and SUPSI, Switzerland
//_/_/   http://www.idsia.ch/
//_/_/
//_/_/ Institute of Cognitive Sciences and Technologies
//_/_/   Consiglio Nazionale delle Ricerche, Italy
//_/_/   http://www.istc.cnr.it/
//_/_/
//_/_/ Dipartimento di Ingegneria Informatica
//_/_/   University of Palermo, Italy
//_/_/   http://roboticslab.dinfo.unipa.it/index.php/Main/HomePage
//_/_/
//_/_/
//_/_/ --- HUMANOBS Open-Source BSD License, with CADIA Clause v 1.0 ---
//_/_/
//_/_/ Redistribution and use in source and binary forms, with or without 
//_/_/ modification, is permitted provided that the following conditions 
//_/_/ are met:
//_/_/
//_/_/ - Redistributions of source code must retain the above copyright 
//_/_/ and collaboration notice, this list of conditions and the 
//_/_/ following disclaimer.
//_/_/
//_/_/ - Redistributions in binary form must reproduce the above copyright 
//_/_/ notice, this list of conditions and the following
//_/_/ disclaimer in the documentation and/or other materials provided 
//_/_/ with the distribution.
//_/_/
//_/_/ - Neither the name of its copyright holders nor the names of its 
//_/_/ contributors may be used to endorse or promote products 
//_/_/ derived from this software without specific prior written permission.
//_/_/
//_/_/ - CADIA Clause: The license granted in and to the software under this 
//_/_/ agreement is a limited-use license. The software may not be used in 
//_/_/ furtherance of: 
//_/_/ (i) intentionally causing bodily injury or severe emotional distress 
//_/_/ to any person; 
//_/_/ (ii) invading the personal privacy or violating the human rights of 
//_/_/ any person; or 
//_/_/ (iii) committing or preparing for any act of war.
//_/_/
//_/_/ THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
//_/_/ "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
//_/_/ LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR 
//_/_/ A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
//_/_/ OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
//_/_/ SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
//_/_/ LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, 
//_/_/ DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY 
//_/_/ THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
//_/_/ (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
//_/_/ OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//_/_/
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

#ifndef core_utils_h
#define core_utils_h

#include	"types.h"
#include	<stdio.h>

#include	<iostream>
#include	<string>

#if defined	WINDOWS
	#include <sys/timeb.h>
	#include <time.h>
#elif defined LINUX
	#include <dlfcn.h>
	#include <errno.h>
	#include <sys/utsname.h>
	#include <sys/time.h>
	#include <cstring>
	#include <cstdlib>
	#include <pthread.h>
	#include <signal.h>
	#include <sys/time.h>
	#include <unistd.h>
	#include <time.h>
//	#undef HANDLE
//	#define HANDLE pthread_cond_t*
#endif

#ifdef WINDOWS
	#define SOCKETWOULDBLOCK WSAEWOULDBLOCK
#else
	#define SOCKETWOULDBLOCK EWOULDBLOCK
#endif

#ifndef SD_BOTH
	#define SD_BOTH 2
#endif

#define	R250_LEN	250
#define	R521_LEN	521

//	Wrapping of OS-dependent functions
namespace	core{

	bool core_dll	WaitForSocketReadability(socket s, int32 timeout);
	bool core_dll	WaitForSocketWriteability(socket s, int32 timeout);

	class	core_dll	Error{
	public:
		static	int32	GetLastOSErrorNumber();
		static	bool	GetOSErrorMessage(char* buffer, uint32 buflen, int32 err = -1);
		static	bool	PrintLastOSErrorMessage(const char* title);
		static	void	PrintBinary(void* p, uint32 size, bool asInt, const char* title = NULL);
	};

	#if defined	WINDOWS
	#elif defined LINUX
		struct SemaTex {
			pthread_mutex_t mutex;
			pthread_cond_t semaphore;
		};
	#endif

	class	core_dll	SharedLibrary{
	private:
		shared_object	library;
	public:
		static	SharedLibrary	*New(const	char	*fileName);
		SharedLibrary();
		~SharedLibrary();
		SharedLibrary	*load(const	char	*fileName);
		template<typename	T>	T	getFunction(const	char	*functionName);
	};

	class	core_dll	Thread{
	private:
		thread	_thread;
		bool	is_meaningful;
	protected:
		Thread();
	public:
		template<class	T>	static	T	*New(thread_function	f,void	*args);
		static	void	TerminateAndWait(Thread	**threads,uint32	threadCount);
		static	void	TerminateAndWait(Thread	*_thread);
		static	void	Wait(Thread	**threads,uint32	threadCount);
		static	void	Wait(Thread	*_thread);
		static	void	Sleep(int64	ms);
		static	void	Sleep();	//	inifnite
		virtual	~Thread();
		void	start(thread_function	f);
		void	suspend();
		void	resume();
		void	terminate();
	};

	class	core_dll	TimeProbe{	//	requires Time::Init()
	private:
		int64	cpu_counts;
		int64	getCounts();
	public:
		void	set();		//	initialize
		void	check();	//	gets the cpu count elapsed between set() and check()
		uint64	us();		//	converts cpu counts in us
	};

	class	core_dll	Time{	//	TODO:	make sure time stamps are consistent when computed by different cores
	friend	class	TimeProbe;
	private:
		static	float64	Period;
		static	int64	InitTime;
	public:
		static	void	Init(uint32	r);	//	detects the hardware timing capabilities; r: time resolution in us (on windows xp: max ~1000; use 1000, 2000, 5000 or 10000)
		static	uint64	Get();			//	in us since 01/01/1970

		static	std::string	ToString_seconds(uint64	t);	//	seconds:milliseconds:microseconds since 01/01/1970.
		static	std::string	ToString_year(uint64	t);	//	day_name day_number month year hour:minutes:seconds:milliseconds:microseconds GMT since 01/01/1970.
	};

	class	core_dll	Host{
	public:
		typedef	char	host_name[255];
		static	uint8	Name(char	*name);	//	name size=255; return the actual size
	};

	class	core_dll	Semaphore{
	private:
		semaphore	s;
	protected:
		static	const	uint32	Infinite;
	public:
		Semaphore(uint32	initialCount,uint32	maxCount);
		~Semaphore();
		bool	acquire(uint32	timeout=Infinite);	//	returns true if timedout
		void	release(uint32	count=1);
		void	reset();
	};

	class	core_dll	Mutex{
	private:
		mutex	m;
	protected:
		static	const	uint32	Infinite;
	public:
		Mutex();
		~Mutex();
		bool	acquire(uint32	timeout=Infinite);	//	returns true if timedout
		void	release();
	};

	class	core_dll	CriticalSection{
	private:
		critical_section	cs;
	public:
		CriticalSection();
		~CriticalSection();
		void	enter();
		void	leave();
	};

	class	core_dll	Timer{
	private:
		#if defined WINDOWS
			timer	t;
		#elif defined LINUX
			timer_t timer;
			struct SemaTex sematex;
		#endif
	protected:
		static	const	uint32	Infinite;
	public:
		Timer();
		~Timer();
		void	start(uint64	deadline,uint32	period=0);	//	deadline in us, period in ms.
		bool	wait(uint32	timeout=Infinite);				//	timeout in ms; returns true if timedout.
		bool	wait(uint64	&us,uint32	timeout=Infinite);	//	idem; updates the us actually spent.
	};

	class	core_dll	Event{
	private:
		#if defined WINDOWS
			event	e;
		#elif defined LINUX
			//	TODO.
		#endif
	public:
		Event();
		~Event();
		void	wait();
		void	fire();
		void	reset();
	};

	class	core_dll	SignalHandler{
	public:
		static	void	Add(signal_handler	h);
		static	void	Remove(signal_handler	h);
	};

	class	core_dll	Atomic{
	public:
		static	int32	Increment32(int32	volatile	*v);									//	return the final value of *v
		static	int32	Decrement32(int32	volatile	*v);									//	return the final value of *v
		static	int32	CompareAndSwap32(int32	volatile	*target,int32	v1,int32	v2);	//	compares *target with v1, if equal, replace it with v2; return the initial value of *target
		static	int64	CompareAndSwap64(int64	volatile	*target,int64	v1,int64	v2);
//		static	word	CompareAndSwap(word	volatile	*target,word	v1,word	v2);			//	uses the right version according to ARCH_xx
		static	int32	Swap32(int32	volatile	*target,int32	v);							//	writes v at target; return the initial value of *target
		static	int64	Swap64(int64	volatile	*target,int64	v);							//	ifndef ARCH_64, calls CompareAndSwap64(target,v,v)
//		static	word	Swap(word	volatile	*target,word	v);								//	uses the right version according to ARCH_xx
#if defined	ARCH_64
		static	int64	Increment64(int64	volatile	*v);
		static	int64	Decrement64(int64	volatile	*v);
		static	int32	Add32(int32	volatile	*target,int32	v);	//	adds v to *target; return the final value of *target
		static	int64	Add64(int64	volatile	*target,int64	v);
#endif
	};

	uint8	core_dll	BSR(word	data);	//	BitScanReverse

	class	core_dll	FastSemaphore:	//	lock-free under no contention
	public	Semaphore{
	private:
		int32	volatile	count;		//	minus the number of waiting threads
		const	int32		maxCount;	//	max number of threads allowed to run
	public:
		FastSemaphore(uint32	initialCount,uint32	maxCount);	//	initialCount >=0
		~FastSemaphore();
		void	acquire();
		void	release();
	};

	class	core_dll	String{
	public:
		static	int32		StartsWith(const std::string &s, const std::string &str);
		static	int32		EndsWith(const std::string &s, const std::string &str);
		static	void		MakeUpper(std::string &str);
		static	void		MakeLower(std::string &str);
		static	void		Trim(std::string& str, const char* chars2remove = " ");
		static	void		TrimLeft(std::string& str, const char* chars2remove = " ");
		static	void		TrimRight(std::string& str, const char* chars2remove = " ");
		static	void		ReplaceLeading(std::string& str, const char* chars2replace, char c);
		static	std::string	Int2String(int64 i);
		static	std::string	Uint2String(uint64 i);
	};

	class	core_dll	Random{
	private:
		static	int32	r250_index;
		static	int32	r521_index;
		static	uint32	r250_buffer[R250_LEN];
		static	uint32	r521_buffer[R521_LEN];
	public:
		static	void	Init();

		float32	operator	()(uint32	range);	//	returns a value in [0,range].
	};
}

#include	"utils.tpl.cpp"


#endif
