//	module.h
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

#ifndef mBrane_sdk_module_h
#define mBrane_sdk_module_h

#include	"utils.h"
#include	"message.h"


using	namespace	mBrane::sdk::payloads;

namespace	mBrane{
	class	XThread;
	namespace	sdk{
		namespace	module{

			class	XThread{
			public:
				virtual	void	block()=0;
			};

			class	Semaphore;
			class	Mutex;
			class	CriticalSection;
			class	Timer;
			class	dll	_Module{	//	migration: migrateOut->dump->payload->send-/ /-receive->load->migrateIn; modules can launch their own internal threads
			friend	class	mBrane::XThread;
			friend	class	Semaphore;
			friend	class	Mutex;
			friend	class	CriticalSection;
			friend	class	Timer;
			private:
				uint16	_ID;
				bool	_canMigrate;
				bool	_canBeSwapped;
				uint32	_activationCount;
				uint8	_priority;
				XThread	*processor;
			protected:
				_Module(uint16	_ID,bool	canMigrate=true,bool	canBeSwapped=true);
				int64	time()	const;
				void	sleep(int64	d);
				void	wait(Thread	**threads,uint32	threadCount);
				void	wait(Thread	*_thread);
				void	send(_Payload	*p)	const;
			public:
				typedef	enum{
					DISCARD=0,
					WAIT=1,
					PREEMPT=2
				}Decision;
				static	void	New(uint16	CID);
				virtual	~_Module();
				uint16	id()	const;
				bool	active()	const;
				void	activate();
				void	deactivate();
				uint8	&priority();
				bool	canMigrate();	//	on another node; dynamic
				bool	canBeSwapped();	//	on another thread within the same node; dynamic
				virtual	uint32		dumpSize();	//	dynamic
				virtual	_Payload	*dump();	//	dumps the current state; can be called anytime
				virtual	void		load(_Payload	*chunk);	//	initializes itself from a previously saved state
				virtual	void		start();	//	called when the module is loaded in a thread for the first time, i.e. at node starting time
				virtual	void		stop();	//	called just before the module is unloaded from the thread for the last time, i.e. at node shutdown time
				virtual	void		swapOut();	//	called when the module is unloaded from its current thread for swapping
				virtual	void		swapIn();	//	called when the module is loaded in a new thread after having been swapped out
				virtual	void		migrateOut();	//	called when the module is unloaded from its current thread for migration
				virtual	void		migrateIn();	//	called when the module is loaded in a new thread after having migrated
				virtual	void		notify(_Payload	*p)=0;	//	called when the module receives a message
				virtual	Decision	decide(_Payload	*p);	//	called when the module code is already processing and a new message comes in; default: WAIT
			};

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
