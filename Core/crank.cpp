//	crank.cpp
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

#include	"crank.h"
#include	"crank_node.h"
#include	"utils.h"


namespace	mBrane{
	namespace	sdk{
		namespace	crank{

			void	_Crank::New(uint16	CID){

				Node::Get()->buildCrank(CID);
			}

			_Crank::_Crank(uint16	_ID,bool	canMigrate,bool	canBeSwapped):_ID(_ID),_canMigrate(canMigrate),_canBeSwapped(canBeSwapped),_activationCount(0),_priority(0),processor(NULL){
			}

			_Crank::~_Crank(){
			}

			inline	uint16	_Crank::id()	const{

				return	_ID;
			}

			inline	bool	_Crank::active()	const{

				return	_activationCount;
			}

			inline	void	_Crank::activate(){

				_activationCount++;
			}

			inline	void	_Crank::deactivate(){

				_activationCount--;
			}

			inline	uint8	&_Crank::priority(){

				return	_priority;
			}

			inline	bool	_Crank::canMigrate(){

				return	_canMigrate;
			}

			inline	bool	_Crank::canBeSwapped(){

				return	_canBeSwapped;
			}

			uint32	_Crank::dumpSize(){

				return	0;
			}

			_Payload	*_Crank::dump(){

				return	NULL;
			}

			void	_Crank::load(_Payload	*chunk){
			}

			void	_Crank::start(){
			}

			void	_Crank::stop(){
			}

			inline	void	_Crank::swapOut(){
			}

			inline	void	_Crank::swapIn(){
			}

			inline	void	_Crank::migrateOut(){
			}

			inline	void	_Crank::migrateIn(){
			}

			inline	int64	_Crank::time()	const{

				return	Node::Get()->time();
			}

			void	_Crank::sleep(int64	d){

				if(processor	&&	d)
					processor->block();
				Thread::Sleep(d);
			}

			void	_Crank::wait(Thread	**threads,uint32	threadCount){

				if(processor)
					processor->block();
				Thread::Wait(threads,threadCount);
			}
			
			void	_Crank::wait(Thread	*_thread){

				if(processor)
					processor->block();
				Thread::Wait(_thread);
			}
			
			inline	void	_Crank::send(_Payload	*p)	const{

				Node::Get()->send(this,p);
			}

			////////////////////////////////////////////////////////////////////////////////////////////////

			CrankUtils::CrankUtils(_Crank	*c):crank(c){
			}

			////////////////////////////////////////////////////////////////////////////////////////////////

			Semaphore::Semaphore(_Crank	*c,uint32	initialCount,uint32	maxCount):mBrane::Semaphore(initialCount,maxCount),CrankUtils(c){
			}

			Semaphore::~Semaphore(){
			}

			bool	Semaphore::acquire(uint32	timeout){

				if(crank->processor	&&	timeout)
					crank->processor->block();
				return	mBrane::Semaphore::acquire(timeout);
			}

			void	Semaphore::release(uint32	count){

				mBrane::Semaphore::release();
			}

			void	Semaphore::reset(){

				mBrane::Semaphore::reset();
			}

			////////////////////////////////////////////////////////////////////////////////////////////////

			Mutex::Mutex(_Crank	*c):mBrane::Mutex(),CrankUtils(c){
			}

			Mutex::~Mutex(){
			}

			bool	Mutex::acquire(uint32	timeout){

				if(crank->processor	&&	timeout)
					crank->processor->block();
				return	mBrane::Mutex::acquire(timeout);
			}

			void	Mutex::release(){

				mBrane::Mutex::release();
			}

			////////////////////////////////////////////////////////////////////////////////////////////////

			CriticalSection::CriticalSection(_Crank	*c):mBrane::CriticalSection(),CrankUtils(c){
			}

			CriticalSection::~CriticalSection(){
			}
			
			void	CriticalSection::enter(){

				if(crank->processor)
					crank->processor->block();
				mBrane::CriticalSection::enter();
			}

			void	CriticalSection::leave(){

				mBrane::CriticalSection::leave();
			}

			////////////////////////////////////////////////////////////////////////////////////////////////

			Timer::Timer(_Crank	*c):mBrane::Timer(),CrankUtils(c){
			}

			Timer::~Timer(){
			}

			void	Timer::start(uint32	deadline,uint32	period){

				mBrane::Timer::start(deadline,period);
			}

			bool	Timer::wait(uint32	timeout){

				if(crank->processor	&&	timeout)
					crank->processor->block();
				return	mBrane::Timer::wait(timeout);
			}

			bool	Timer::wait(uint64	&us,uint32	timeout){

				if(crank->processor)
					crank->processor->block();
				return	mBrane::Timer::wait(us,timeout);
			}
		}
	}
}
