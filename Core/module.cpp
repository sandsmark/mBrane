//	module.cpp
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

#include	"module.h"
#include	"module_node.h"
#include	"utils.h"


namespace	mBrane{
	namespace	sdk{
		namespace	module{

			_Module::_Module():	_Object(),_priority(0),processor(NULL),_ready(false){
			}

			_Module::~_Module(){
			}

			inline	uint8	&_Module::priority(){

				return	_priority;
			}

			inline	bool	_Module::canMigrate(){

				return	_canMigrate;
			}

			inline	bool	_Module::canBeSwapped(){

				return	_canBeSwapped;
			}

			bool	_Module::isReady(){

				return	_ready;
			}

			uint32	_Module::dumpSize(){

				return	0;
			}

			_Payload	*_Module::dump(){

				return	NULL;
			}

			void	_Module::load(_Payload	*chunk){
			}

			void	_Module::start(){

				_ready=true;
			}

			void	_Module::stop(){

				_ready=false;
			}

			inline	void	_Module::migrateOut(){

				_ready=false;
			}

			inline	void	_Module::migrateIn(){

				_ready=true;
			}

			void	_Module::sleep(int64	d){

				if(processor	&&	d)
					processor->block();
				Thread::Sleep(d);
			}

			void	_Module::wait(Thread	**threads,uint32	threadCount){

				if(processor)
					processor->block();
				Thread::Wait(threads,threadCount);
			}
			
			void	_Module::wait(Thread	*_thread){

				if(processor)
					processor->block();
				Thread::Wait(_thread);
			}
		}
	}
}
