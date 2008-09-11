//	module_utils.cpp
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

#include	"module_utils.h"
#include	"utils.h"


namespace	mBrane{
	namespace	sdk{
		namespace	module{

			ModuleUtils::ModuleUtils(_Module	*c):module(c){
			}

			////////////////////////////////////////////////////////////////////////////////////////////////

			Semaphore::Semaphore(_Module	*c,uint32	initialCount,uint32	maxCount):mBrane::Semaphore(initialCount,maxCount),ModuleUtils(c){
			}

			Semaphore::~Semaphore(){
			}

			bool	Semaphore::acquire(uint32	timeout){

				if(module->processor	&&	timeout)
					module->processor->block();
				return	mBrane::Semaphore::acquire(timeout);
			}

			void	Semaphore::release(uint32	count){

				mBrane::Semaphore::release();
			}

			void	Semaphore::reset(){

				mBrane::Semaphore::reset();
			}

			////////////////////////////////////////////////////////////////////////////////////////////////

			Mutex::Mutex(_Module	*c):mBrane::Mutex(),ModuleUtils(c){
			}

			Mutex::~Mutex(){
			}

			bool	Mutex::acquire(uint32	timeout){

				if(module->processor	&&	timeout)
					module->processor->block();
				return	mBrane::Mutex::acquire(timeout);
			}

			void	Mutex::release(){

				mBrane::Mutex::release();
			}

			////////////////////////////////////////////////////////////////////////////////////////////////

			CriticalSection::CriticalSection(_Module	*c):mBrane::CriticalSection(),ModuleUtils(c){
			}

			CriticalSection::~CriticalSection(){
			}
			
			void	CriticalSection::enter(){

				if(module->processor)
					module->processor->block();
				mBrane::CriticalSection::enter();
			}

			void	CriticalSection::leave(){

				mBrane::CriticalSection::leave();
			}

			////////////////////////////////////////////////////////////////////////////////////////////////

			Timer::Timer(_Module	*c):mBrane::Timer(),ModuleUtils(c){
			}

			Timer::~Timer(){
			}

			void	Timer::start(uint32	deadline,uint32	period){

				mBrane::Timer::start(deadline,period);
			}

			bool	Timer::wait(uint32	timeout){

				if(module->processor	&&	timeout)
					module->processor->block();
				return	mBrane::Timer::wait(timeout);
			}

			bool	Timer::wait(uint64	&us,uint32	timeout){

				if(module->processor)
					module->processor->block();
				return	mBrane::Timer::wait(us,timeout);
			}
		}
	}
}
