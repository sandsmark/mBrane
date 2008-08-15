//	utils.cpp
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

	inline	void	Thread::Wait(Thread	**threads,uint32	threadCount){

		if(!threads)
			return;
#if defined	WINDOWS
		for(uint32	i=0;i<threadCount;i++)
			WaitForSingleObject(threads[i]->_thread,INFINITE);
#elif defined LINUX
#elif defined OSX
#endif
	}

	inline	void	Thread::Wait(Thread	*_thread){

		if(!_thread)
			return;
#if defined	WINDOWS
		WaitForSingleObject(_thread->_thread,INFINITE);
#elif defined LINUX
#elif defined OSX
#endif
	}

	inline	void	Thread::Sleep(int64	d){
#if defined	WINDOWS
		::Sleep(d);
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

	void	Thread::start(thread_function	f){
#if defined	WINDOWS
		_thread=CreateThread(NULL,0,f,this,0,NULL);
#elif defined LINUX
#elif defined OSX
#endif
	}

	void	Thread::suspend(){
#if defined	WINDOWS
		SuspendThread(_thread);
#elif defined LINUX
#elif defined OSX
#endif
	}

	void	Thread::resume(){
#if defined	WINDOWS
		ResumeThread(_thread);
#elif defined LINUX
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
#elif defined OSX
#endif
	}

	inline	void	TimeProbe::check(){

		cpu_counts=getCounts()-cpu_counts;
	}

	inline	int64	TimeProbe::us(){

		return	cpu_counts*Time::Period;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////

#if defined	WINDOWS
	typedef LONG NTSTATUS;
	typedef NTSTATUS (__stdcall *NSTR)(ULONG, BOOLEAN, PULONG);
	#define STATUS_SUCCESS ((NTSTATUS)0x00000000L)
	bool	NtSetTimerResolution(IN	ULONG	RequestedResolution,IN	BOOLEAN	Set,OUT	PULONG	ActualResolution);
#elif defined LINUX
#elif defined OSX
#endif

	float64	Time::Period;
	
	int64	Time::InitTime;

	inline	void	Time::Init(uint32	r){
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
#elif defined OSX
#endif
	}

	inline	int64	Time::Get(){
#if defined	WINDOWS
		LARGE_INTEGER	counter;
		QueryPerformanceCounter(&counter);
		return	(int64)(InitTime+counter.QuadPart*Period);
#elif defined LINUX
#elif defined OSX
#endif
	}

	////////////////////////////////////////////////////////////////////////////////////////////////

	uint8	Host::Name(char	*name){
#if defined	WINDOWS
		uint32	s=255;
		GetComputerName(name,&s);
		return	s;
#elif defined LINUX
#elif defined OSX
#endif
	}

	////////////////////////////////////////////////////////////////////////////////////////////////

#if defined	WINDOWS
	const	uint32	Semaphore::Infinite=INFINITE;
#elif defined LINUX
#elif defined OSX
#endif

	Semaphore::Semaphore(uint32	initialCount,uint32	maxCount){
#if defined	WINDOWS
		s=CreateSemaphore(NULL,initialCount,maxCount,NULL);
#elif defined LINUX
#elif defined OSX
#endif
	}

	Semaphore::~Semaphore(){
#if defined	WINDOWS
		CloseHandle(s);
#elif defined LINUX
#elif defined OSX
#endif
	}

	inline	bool	Semaphore::acquire(uint32	timeout){
#if defined	WINDOWS
		uint32	r=WaitForSingleObject(s,timeout);
		return	r==WAIT_TIMEOUT;
#elif defined LINUX
#elif defined OSX
#endif
	}

	inline	void	Semaphore::release(uint32	count){
#if defined	WINDOWS
		ReleaseSemaphore(s,count,NULL);
#elif defined LINUX
#elif defined OSX
#endif
	}

	inline	void	Semaphore::reset(){
#if defined	WINDOWS
		bool	r;
		do
			r=acquire(0);
		while(!r);
#elif defined LINUX
#elif defined OSX
#endif
	}

	////////////////////////////////////////////////////////////////////////////////////////////////

#if defined	WINDOWS
	const	uint32	Mutex::Infinite=INFINITE;
#elif defined LINUX
#elif defined OSX
#endif

	Mutex::Mutex(){
#if defined	WINDOWS
		m=CreateMutex(NULL,false,NULL);
#elif defined LINUX
#elif defined OSX
#endif
	}

	Mutex::~Mutex(){
#if defined	WINDOWS
		CloseHandle(m);
#elif defined LINUX
#elif defined OSX
#endif
	}

	inline	bool	Mutex::acquire(uint32	timeout){
#if defined	WINDOWS
		uint32	r=WaitForSingleObject(m,timeout);
		return	r==WAIT_TIMEOUT;
#elif defined LINUX
#elif defined OSX
#endif
	}

	inline	void	Mutex::release(){
#if defined	WINDOWS
		ReleaseMutex(m);
#elif defined LINUX
#elif defined OSX
#endif
	}

	////////////////////////////////////////////////////////////////////////////////////////////////

	CriticalSection::CriticalSection(){
#if defined	WINDOWS
		InitializeCriticalSection(&cs);
#elif defined LINUX
#elif defined OSX
#endif
	}

	CriticalSection::~CriticalSection(){
#if defined	WINDOWS
		DeleteCriticalSection(&cs);
#elif defined LINUX
#elif defined OSX
#endif
	}

	inline	void	CriticalSection::enter(){
#if defined	WINDOWS
		EnterCriticalSection(&cs);
#elif defined LINUX
#elif defined OSX
#endif
	}

	inline	void	CriticalSection::leave(){
#if defined	WINDOWS
		LeaveCriticalSection(&cs);
#elif defined LINUX
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
		SetConsoleCtrlHandler(h,true);
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
}