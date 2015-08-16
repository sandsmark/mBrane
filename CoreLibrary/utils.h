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
#define LINUX
#include	"types.h"
#include	<stdio.h>
#include <semaphore.h>
#include	<iostream>
#include	<string>
#include <functional>
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

//	bool core_dll	WaitForSocketReadability(socket s, int32 timeout);
//	bool core_dll	WaitForSocketWriteability(socket s, int32 timeout);

	class	core_dll	Error{
	public:
                static	int32_t	GetLastOSErrorNumber();
                static	bool	GetOSErrorMessage(char* buffer, uint32_t buflen, int32_t err = -1);
		static	bool	PrintLastOSErrorMessage(const char* title);
                static	void	PrintBinary(void* p, uint32_t size, bool asInt, const char* title = NULL);
	};

	#if defined	WINDOWS
	#elif defined LINUX
		struct SemaTex {
			pthread_mutex_t mutex;
			pthread_cond_t semaphore;
		};
	#endif
/*
	class	core_dll	SharedLibrary{
	private:
		shared_object	library;
	public:
		static	SharedLibrary	*New(const	char	*fileName);
		SharedLibrary();
		~SharedLibrary();
		SharedLibrary	*load(const	char	*fileName);
		template<typename	T>	T	getFunction(const	char	*functionName);
        };*/

        class	core_dll	Thread{
	private:
		thread	_thread;
		bool	is_meaningful;
	protected:
		Thread();
	public:
                template<class	T>	static	T	*New(std::function<void>	f,void	*args);
                static	void	TerminateAndWait(Thread	**threads,uint32_t	threadCount);
		static	void	TerminateAndWait(Thread	*_thread);
                static	void	Wait(Thread	**threads,uint32_t	threadCount);
		static	void	Wait(Thread	*_thread);
                static	void	Sleep(int64_t	ms);
		static	void	Sleep();	//	inifnite
		virtual	~Thread();
                void	start(std::function<void(void*)>	f);
		void	suspend();
		void	resume();
		void	terminate();
        };

	class	core_dll	TimeProbe{	//	requires Time::Init()
	private:
                int64_t	cpu_counts;
                int64_t	getCounts();
	public:
		void	set();		//	initialize
		void	check();	//	gets the cpu count elapsed between set() and check()
                uint64_t	us();		//	converts cpu counts in us
	};

	class	core_dll	Time{	//	TODO:	make sure time stamps are consistent when computed by different cores
	friend	class	TimeProbe;
	private:
                static	double	Period;
		static	int64_t	InitTime;
	public:
		static	void	Init(uint32_t	r);	//	detects the hardware timing capabilities; r: time resolution in us (on windows xp: max ~1000; use 1000, 2000, 5000 or 10000)
		static	uint64_t	Get();			//	in us since 01/01/1970

                static	std::string	ToString_seconds(uint64_t	t);	//	seconds:milliseconds:microseconds since 01/01/1970.
                static	std::string	ToString_year(uint64_t	t);	//	day_name day_number month year hour:minutes:seconds:milliseconds:microseconds GMT since 01/01/1970.
	};

        class	core_dll	Host{
	public:
		typedef	char	host_name[255];
                static	uint8_t	Name(char	*name);	//	name size=255; return the actual size
	};

	class	core_dll	Semaphore{
	private:
                sem_t	s;
	protected:
                static	const	uint32_t	Infinite;
	public:
                Semaphore(uint32_t	initialCount,uint32_t	maxCount);
		~Semaphore();
                bool	acquire(uint32_t	timeout=Infinite);	//	returns true if timedout
                void	release(uint32_t	count=1);
		void	reset();
        };

	class	core_dll	Mutex{
	private:
		mutex	m;
	protected:
                static	const	uint32_t	Infinite;
	public:
		Mutex();
		~Mutex();
                bool	acquire(uint32_t	timeout=Infinite);	//	returns true if timedout
		void	release();
	};

	class	core_dll	CriticalSection{
	private:
                pthread_mutex_t	cs;
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
                static	const	uint32_t	Infinite;
	public:
		Timer();
		~Timer();
                void	start(uint64_t	deadline,uint32_t	period=0);	//	deadline in us, period in ms.
                bool	wait(uint32_t	timeout=Infinite);				//	timeout in ms; returns true if timedout.
                bool	wait(uint64_t	&us,uint32_t	timeout=Infinite);	//	idem; updates the us actually spent.
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
                static	void	Add(sighandler_t	h);
                static	void	Remove(sighandler_t	h);
	};

	class	core_dll	Atomic{
	public:
                static	int32_t	Increment32(int32_t	volatile	*v);									//	return the final value of *v
                static	int32_t	Decrement32(int32_t	volatile	*v);									//	return the final value of *v
                static	int32_t	CompareAndSwap32(int32_t	volatile	*target,int32_t	v1,int32_t	v2);	//	compares *target with v1, if equal, replace it with v2; return the initial value of *target
                static	int64_t	CompareAndSwap64(int64_t	volatile	*target,int64_t	v1,int64_t	v2);
//		static	word	CompareAndSwap(word	volatile	*target,word	v1,word	v2);			//	uses the right version according to ARCH_xx
                static	int32_t	Swap32(int32_t	volatile	*target,int32_t	v);							//	writes v at target; return the initial value of *target
                static	int64_t	Swap64(int64_t	volatile	*target,int64_t	v);							//	ifndef ARCH_64, calls CompareAndSwap64(target,v,v)
//		static	word	Swap(word	volatile	*target,word	v);								//	uses the right version according to ARCH_xx
#if defined	ARCH_64
                static	int64_t	Increment64(int64_t	volatile	*v);
                static	int64_t	Decrement64(int64_t	volatile	*v);
                static	int32_t	Add32(int32_t	volatile	*target,int32_t	v);	//	adds v to *target; return the final value of *target
                static	int64_t	Add64(int64_t	volatile	*target,int64_t	v);
#endif
	};

        uint8_t	core_dll	BSR(uint32_t	data);	//	BitScanReverse

        class	core_dll	FastSemaphore:	//	lock-free under no contention
        public	Semaphore{
	private:
                int32_t	volatile	count;		//	minus the number of waiting threads
                const	int32_t		maxCount;	//	max number of threads allowed to run
	public:
                FastSemaphore(uint32_t	initialCount,uint32_t	maxCount);	//	initialCount >=0
		~FastSemaphore();
		void	acquire();
		void	release();
	};

	class	core_dll	String{
	public:
                static	int32_t		StartsWith(const std::string &s, const std::string &str);
                static	int32_t		EndsWith(const std::string &s, const std::string &str);
		static	void		MakeUpper(std::string &str);
		static	void		MakeLower(std::string &str);
		static	void		Trim(std::string& str, const char* chars2remove = " ");
		static	void		TrimLeft(std::string& str, const char* chars2remove = " ");
		static	void		TrimRight(std::string& str, const char* chars2remove = " ");
		static	void		ReplaceLeading(std::string& str, const char* chars2replace, char c);
                static	std::string	Int2String(int64_t i);
                static	std::string	Uint2String(uint64_t i);
        };

	class	core_dll	Random{
	private:
                static	int32_t	r250_index;
                static	int32_t	r521_index;
                static	uint32_t	r250_buffer[R250_LEN];
                static	uint32_t	r521_buffer[R521_LEN];
	public:
		static	void	Init();

                float	operator	()(uint32_t	range);	//	returns a value in [0,range].
	};
}

#include	"utils.tpl.cpp"


#endif
