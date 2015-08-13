//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
//_/_/
//_/_/ HUMANOBS - mBrane
//_/_/
//_/_/ Eric Nivel
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

#ifndef mBrane_sdk_module_h
#define mBrane_sdk_module_h

#include	"../CoreLibrary/utils.h"
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
