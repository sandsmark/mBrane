//	module.h
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

#ifndef mBrane_sdk_module_h
#define mBrane_sdk_module_h

#include	"utils.h"
#include	"message.h"


using	namespace	mBrane::sdk::payloads;

namespace	mBrane{
	class	XThread;
	class	ModuleDescriptor;
	class	Node;
	namespace	sdk{
		namespace	module{

			//	Root class for all modules.
			//	The actual base class for user-defined modules is defined in application.h and respectively, in library.h for module library vendors
			//	Migration sequence: migrateOut->dump->payload->send-/ /-receive->load->migrateIn; modules can then launch their own internal threads if any
			class	mBrane_dll	_Module:
			public	_Object{
			friend	class	mBrane::Node;
			friend	class	mBrane::XThread;
			friend	class	mBrane::ModuleDescriptor;
			private:
				XThread				*processor;
				FastSemaphore		*sync;
				ModuleDescriptor	*descriptor;
			protected:
				uint16	_cid;
				uint16	_id;
				bool	_canMigrate;
				uint8	_priority;
				bool	_ready;	//	set to false after stop and migrateOut, set to true after start and migrateIn
				_Module();
				void	sleep(int64	d);
				void	wait(Thread	**threads,uint32	threadCount);
				void	wait(Thread	*_thread);
			public:
				typedef	enum{
					DISCARD=0,
					WAIT=1,
					PREEMPT=2
				}Decision;
				virtual	~_Module();
				virtual	void	loadParameters(const	std::vector<word32>	&numbers,const	std::vector<std::string>	&strings){}	//	parameters is deallocated after the call; called only upon local construction - in other words, will not be called upon remote construction (using the CreateModule message)
				uint8	&priority();
				bool	canMigrate();	//	on another node; dynamic
				bool	isReady();	//	if not, messages will be lost
				virtual	uint32		dumpSize();	//	dynamic
				virtual	_Payload	*dump();	//	dumps the current state; can be called anytime
				virtual	void		load(_Payload	*chunk);	//	initializes itself from a previously saved state
				virtual	void		_start()=0;	//	called when the module is loaded in a thread for the first time, i.e. at node starting time
				virtual	void		_stop()=0;	//	called just before the module is unloaded from the thread for the last time, i.e. at node shutdown time
				virtual	void		migrateOut();	//	called when the module is unloaded from its current thread for migration
				virtual	void		migrateIn();	//	called when the module is loaded in a new thread after having migrated
				virtual	void		notify(_Payload	*p)=0;	//	called when the module receives a message
				virtual	void		notify(uint16	sid,_Payload	*p)=0;	//	called when the module receives data from a stream
				virtual	Decision	dispatch(_Payload	*p)=0;	//	called when the module code is already processed by an XThread and a new message comes in
			};
		}
	}
}


#endif
