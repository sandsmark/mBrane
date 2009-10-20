//	utils.cpp
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

#include	"utils.h"

#include	<iostream>

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
#elif defined OSX
#endif


namespace	mBrane{

	void PrintBinary(void* p, uint32 size, bool asInt, const char* title) {
		if (title != NULL)
			printf("--- %s %u ---\n", title, size);
		unsigned char c;
		for (uint32 n=0; n<size; n++) {
			c = *(((unsigned char*)p)+n);
			if (asInt)
				printf("[%u] ", (unsigned int)c);
			else
				printf("[%c] ", c);
			if ( (n > 0) && ((n+1)%10 == 0) )
				printf("\n");
		}
		printf("\n");
	}

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
		if(library)
			dlclose(library);
#elif defined OSX
#endif
	}

	SharedLibrary	*SharedLibrary::load(const	char	*fileName){
#if defined	WINDOWS
		library=LoadLibrary(TEXT(fileName));
		if(!library){

			DWORD	error=GetLastError();
			std::cout<<"> Error: unable to load shared library "<<fileName<<" :"<<error<<std::endl;
			return	NULL;
		}
#elif defined LINUX
		/*
		 * libraries on Linux are called 'lib<name>.so'
		 * if the passed in fileName does not have those
		 * components add them in.
		 */
		char *libraryName = (char *)calloc(1, strlen(fileName)+6+1);
		if (strstr(fileName, "lib") == NULL) {
			strcat(libraryName, "lib");
		}
		strcat(libraryName, fileName);
		if (strstr(fileName+(strlen(fileName)-3), ".so") == NULL) {
			strcat(libraryName, ".so");
		}
		library = dlopen(libraryName, RTLD_NOW | RTLD_GLOBAL);
		if (!library) {
			std::cout<<"> Error: unable to load shared library "<< fileName << " :" << dlerror() << std::endl;
			free(libraryName);
			return NULL;
		}
		free(libraryName);
#elif defined OSX
#endif
		return	this;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////

	void	Thread::Wait(Thread	**threads,uint32	threadCount){

		if(!threads)
			return;
#if defined	WINDOWS
		for(uint32	i=0;i<threadCount;i++)
			WaitForSingleObject(threads[i]->_thread,INFINITE);
#elif defined LINUX
		for(uint32	i=0;i<threadCount;i++)
			pthread_join(threads[i]->_thread, NULL);
#elif defined OSX
#endif
	}

	void	Thread::Wait(Thread	*_thread){

		if(!_thread)
			return;
#if defined	WINDOWS
		WaitForSingleObject(_thread->_thread,INFINITE);
#elif defined LINUX
		pthread_join(_thread->_thread, NULL);
#elif defined OSX
#endif
	}

	void	Thread::Sleep(int64	d){
#if defined	WINDOWS
		::Sleep((uint32)d);
#elif defined LINUX
		// we are actually being passed millisecond, so multiply up
		usleep(d*1000);
#elif defined OSX
#endif
	}

	void	Thread::Sleep(){
#if defined	WINDOWS
		::Sleep(INFINITE);
#elif defined LINUX
		while(true)
			sleep(1000);
#elif defined OSX
#endif
	}

	Thread::Thread(){
	}

	Thread::~Thread(){
#if defined	WINDOWS
		ExitThread(0);
#elif defined LINUX
		pthread_exit(0);
#elif defined OSX
#endif
	}

	void	Thread::start(thread_function	f){
#if defined	WINDOWS
		_thread=CreateThread(NULL,0,f,this,0,NULL);
#elif defined LINUX
		pthread_create(&_thread, NULL, f, this);
#elif defined OSX
#endif
	}

	void	Thread::suspend(){
#if defined	WINDOWS
		SuspendThread(_thread);
#elif defined LINUX
		pthread_kill(_thread, SIGSTOP);
#elif defined OSX
#endif
	}

	void	Thread::resume(){
#if defined	WINDOWS
		ResumeThread(_thread);
#elif defined LINUX
		pthread_kill(_thread, SIGCONT);
#elif defined OSX
#endif
	}

	////////////////////////////////////////////////////////////////////////////////////////////////

	inline	void	TimeProbe::set(){
		
		cpu_counts=getCounts();
	}

	inline	int64	TimeProbe::getCounts(){
#if defined	WINDOWS
		LARGE_INTEGER	counter;
		QueryPerformanceCounter(&counter);
		return	counter.QuadPart;
#elif defined LINUX
		return 0;
#elif defined OSX
#endif
	}

	inline	void	TimeProbe::check(){

		cpu_counts=getCounts()-cpu_counts;
	}

	inline	int64	TimeProbe::us(){

		return	(int64)(cpu_counts*Time::Period);
	}

	////////////////////////////////////////////////////////////////////////////////////////////////

#if defined	WINDOWS
	typedef LONG NTSTATUS;
	typedef NTSTATUS (__stdcall *NSTR)(ULONG, BOOLEAN, PULONG);
	#define STATUS_SUCCESS ((NTSTATUS)0x00000000L)
	bool	NtSetTimerResolution(IN	ULONG	RequestedResolution,IN	BOOLEAN	Set,OUT	PULONG	ActualResolution);
#elif defined LINUX
	// TODO
#elif defined OSX
#endif

	float64	Time::Period;
	
	int64	Time::InitTime;

	void	Time::Init(uint32	r){
#if defined	WINDOWS
	NTSTATUS	nts;
	HMODULE	NTDll=::LoadLibrary("NTDLL");
	ULONG	actualResolution=0;
	if(NTDll){

		NSTR	pNSTR=(NSTR)::GetProcAddress(NTDll,"NtSetTimerResolution");	//	undocumented win xp sys internals
		if(pNSTR)
			nts=(*pNSTR)(10*r,true,&actualResolution);	//	in 100 ns units
	}
	LARGE_INTEGER	f;
	QueryPerformanceFrequency(&f);
	Period=1000000.0/f.QuadPart;	//	in us
	struct	_timeb	local_time;
	_ftime(&local_time);
	InitTime=(int64)(local_time.time*1000+local_time.millitm)*1000;	//	in us
#elif defined LINUX
		// we are actually setup a timer resolution of 1ms
		// we can simulate this by performing a gettimeofday call
		struct timeval tv;
		gettimeofday(&tv, NULL);
		InitTime = ((int64)tv.tv_sec * 1000000) + (tv.tv_usec);
#elif defined OSX
#endif
	}

	int64	Time::Get(){
#if defined	WINDOWS
		LARGE_INTEGER	counter;
		QueryPerformanceCounter(&counter);
		return	(int64)(InitTime+counter.QuadPart*Period);
#elif defined LINUX
		timeval perfCount;
		struct timezone tmzone;
		gettimeofday(&perfCount, &tmzone);
		int64 r = ((int64)perfCount.tv_sec * 1000000) + perfCount.tv_usec;
		return	r;
#elif defined OSX
#endif
	}

	////////////////////////////////////////////////////////////////////////////////////////////////

	uint8	Host::Name(char	*name){
#if defined	WINDOWS
		uint32	s=255;
		GetComputerName(name,&s);
		return	(uint8)s;
#elif defined LINUX
		struct utsname utsname;
		uname(&utsname);
		strcpy(name, utsname.nodename);
		return strlen(name);
#elif defined OSX
#endif
	}

	////////////////////////////////////////////////////////////////////////////////////////////////

#if defined	WINDOWS
	const	uint32	Semaphore::Infinite=INFINITE;
#elif defined LINUX
	/*
	 * Normally this should be SEM_VALUE_MAX but apparently the <semaphore.h> header
	 * does not define it. The documents I have read indicate that on Linux it is
	 * always equal to INT_MAX - so use that instead.
	 */
	const	uint32	Semaphore::Infinite=INT_MAX;
#elif defined OSX
#endif

	Semaphore::Semaphore(uint32	initialCount,uint32	maxCount){
#if defined	WINDOWS
		s=CreateSemaphore(NULL,initialCount,maxCount,NULL);
#elif defined LINUX
		sem_init(&s, 0, initialCount);
#elif defined OSX
#endif
	}

	Semaphore::~Semaphore(){
#if defined	WINDOWS
		CloseHandle(s);
#elif defined LINUX
		sem_destroy(&s);
#elif defined OSX
#endif
	}

	bool	Semaphore::acquire(uint32	timeout){
#if defined	WINDOWS
		uint32	r=WaitForSingleObject(s,timeout);
		return	r==WAIT_TIMEOUT;
#elif defined LINUX
		struct timespec t;
		int r;
		
		t.tv_sec = timeout;
		t.tv_nsec = 0;
		
		r = sem_timedwait(&s, &t);
		return r == ETIMEDOUT;
#elif defined OSX
#endif
	}

	void	Semaphore::release(uint32	count){
#if defined	WINDOWS
		ReleaseSemaphore(s,count,NULL);
#elif defined LINUX
		for (uint32 c = 0; c < count; c++)
			sem_post(&s);
#elif defined OSX
#endif
	}

	void	Semaphore::reset(){
#if defined	WINDOWS
		bool	r;
		do
			r=acquire(0);
		while(!r);
#elif defined LINUX
		bool	r;
		do
			r=acquire(0);
		while(!r);
#elif defined OSX
#endif
	}

	////////////////////////////////////////////////////////////////////////////////////////////////

#if defined LINUX
bool CalcTimeout(struct timespec &timeout, struct timeval &now, uint32 ms) {

	if (gettimeofday(&now, NULL) != 0)
		return false;
	ldiv_t div_result;
	div_result = ldiv( ms, 1000 );
	timeout.tv_sec = now.tv_sec + div_result.quot;
	long x = now.tv_usec + (div_result.rem * 1000);
	if (x >= 1000000) {
		timeout.tv_sec++;
		x -= 1000000;
	}
	timeout.tv_nsec = x * 1000;
	return true;
}
#endif

#if defined	WINDOWS
	const	uint32	Mutex::Infinite=INFINITE;
#elif defined LINUX
	/*
	 * Normally this should be SEM_VALUE_MAX but apparently the <semaphore.h> header
	 * does not define it. The documents I have read indicate that on Linux it is
	 * always equal to INT_MAX - so use that instead.
	 */
	const	uint32	Mutex::Infinite=INT_MAX;
#elif defined OSX
#endif

	Mutex::Mutex(){
#if defined	WINDOWS
		m=CreateMutex(NULL,false,NULL);
#elif defined LINUX
		pthread_mutex_init(&m, NULL);
#elif defined OSX
#endif
	}

	Mutex::~Mutex(){
#if defined	WINDOWS
		CloseHandle(m);
#elif defined LINUX
		pthread_mutex_destroy(&m);
#elif defined OSX
#endif
	}

	bool	Mutex::acquire(uint32	timeout){
#if defined	WINDOWS
		uint32	r=WaitForSingleObject(m,timeout);
		return	r==WAIT_TIMEOUT;
#elif defined LINUX
		int64 start = Time::Get();
		int64 uTimeout = timeout*1000;

		while (pthread_mutex_trylock(&m) != 0) {
			Thread::Sleep(10);
			if (Time::Get() - start >= uTimeout)
				return false;
		}
		return true;

#elif defined OSX
#endif
	}

	void	Mutex::release(){
#if defined	WINDOWS
		ReleaseMutex(m);
#elif defined LINUX
		pthread_mutex_unlock(&m);
#elif defined OSX
#endif
	}

	////////////////////////////////////////////////////////////////////////////////////////////////

	CriticalSection::CriticalSection(){
#if defined	WINDOWS
		InitializeCriticalSection(&cs);
#elif defined LINUX
		pthread_mutex_init(&cs, NULL);
#elif defined OSX
#endif
	}

	CriticalSection::~CriticalSection(){
#if defined	WINDOWS
		DeleteCriticalSection(&cs);
#elif defined LINUX
		pthread_mutex_destroy(&cs);
#elif defined OSX
#endif
	}

	void	CriticalSection::enter(){
#if defined	WINDOWS
		EnterCriticalSection(&cs);
#elif defined LINUX
		pthread_mutex_lock(&cs);
#elif defined OSX
#endif
	}

	void	CriticalSection::leave(){
#if defined	WINDOWS
		LeaveCriticalSection(&cs);
#elif defined LINUX
		pthread_mutex_unlock(&cs);
#elif defined OSX
#endif
	}

	////////////////////////////////////////////////////////////////////////////////////////////////

#if defined	WINDOWS
	const	uint32	Timer::Infinite=INFINITE;
#elif defined LINUX
#elif defined OSX
#endif

	Timer::Timer(){
#if defined	WINDOWS
		t=CreateWaitableTimer(NULL,true,NULL);
#elif defined LINUX
#elif defined OSX
#endif
	}

	Timer::~Timer(){
#if defined	WINDOWS
		CloseHandle(t);
#elif defined LINUX
#elif defined OSX
#endif
	}

	inline	void	Timer::start(uint32	deadline,uint32	period){
#if defined	WINDOWS
	LARGE_INTEGER	_deadline;	//	in 100 ns intervals
    _deadline.QuadPart=-10LL*deadline;	//	negative means relative
    bool	r=SetWaitableTimer(t,&_deadline,period,NULL,NULL,0);
#elif defined LINUX
#elif defined OSX
#endif
	}

	inline	bool	Timer::wait(uint32	timeout){
#if defined	WINDOWS
		uint32	r=WaitForSingleObject(t,timeout);
		return	r==WAIT_TIMEOUT;
#elif defined LINUX
		//TODO
		return	false;
#elif defined OSX
#endif
	}

	inline	bool	Timer::wait(uint64	&us,uint32	timeout){

		TimeProbe	probe;
		probe.set();
		bool	r=wait(timeout);
		probe.check();
		us=probe.us();
		return	r;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////

	void	SignalHandler::Add(signal_handler	h){
#if defined	WINDOWS
		if(SetConsoleCtrlHandler(h,true)==0){

			int	e=GetLastError();
			std::cout<<"Error: "<<e<<" failed to add signal handler"<<std::endl;
			return;
		}
#elif defined LINUX
#elif defined OSX
#endif
	}

	void	SignalHandler::Remove(signal_handler	h){
#if defined	WINDOWS
		SetConsoleCtrlHandler(h,false);
#elif defined LINUX
#elif defined OSX
#endif
	}

	////////////////////////////////////////////////////////////////////////////////////////////////

	int32	Atomic::Increment(int32	*v){
#if defined	WINDOWS
		return	InterlockedIncrement(v);
#elif defined LINUX
		//	TODO
		return	++(*v);
#elif defined OSX
#endif
	};

	int32	Atomic::Decrement(int32	*v){
#if defined	WINDOWS
		return	InterlockedDecrement(v);
#elif defined LINUX
		//	TODO
		return	--(*v);
#elif defined OSX
#endif
	};

	////////////////////////////////////////////////////////////////////////////////////////////////

	uint8	BSR(word	data){
#if defined	WINDOWS
#if defined	ARCH_32
		uint32	index;
		_BitScanReverse(&index,data);
		return	(uint8)index;
#elif defined	ARCH_64
		uint64	index;
		_BitScanReverse64(&index,data);
		return	(uint8)index;
#endif
#elif defined LINUX
#if defined	ARCH_32
		return	(uint8)(31-__builtin_clz(data));
#elif defined	ARCH_64
		return	(uint8)(63-__builtin_clz(data));
#endif
#elif defined OSX
#endif
	}
}
