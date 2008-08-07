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

#define	CRANK_INPUT_QUEUE_SIZE	512


namespace	mBrane{
	namespace	sdk{
		namespace	crank{

			void	_Crank::Build(uint16	CID){

				Node::Get()->buildCrank(CID);
			}

			_Crank::_Crank(uint16	_ID,bool	canMigrate,bool	canBeSwapped):CircularBuffer<P<_Payload> >(),_ID(_ID),_canMigrate(canMigrate),_canBeSwapped(canBeSwapped),_alive(true){

				init(CRANK_INPUT_QUEUE_SIZE);
			}

			_Crank::~_Crank(){
			}

			inline	uint16	_Crank::id()	const{

				return	_ID;
			}

			inline	bool	_Crank::alive()	const{

				return	_alive;
			}

			inline	bool	_Crank::canMigrate(){

				return	_canMigrate;
			}

			inline	bool	_Crank::canBeSwapped(){

				return	_canBeSwapped;
			}

			inline	bool	_Crank::run(){

				return	true;
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

			void	_Crank::sleep(int64	d)	const{

				Thread::Sleep(d);
			}

			void	_Crank::quit(){

				_alive=false;
			}

			inline	void	_Crank::peek(int32	depth){

				CriticalSection::enter();

				Iterator	i;
				uint32	d=0;
				for(i=begin();i!=end()	&&	d<depth;i++,d++){

					_Payload	*p=(_Payload	*)(P<_Payload>)i;
					if(!p)
						continue;
					if(preview(p)){

						((P<_Payload>)i)=NULL;
						_messageCount--;
					}
				}

				CriticalSection::leave();
			}

			inline	void	_Crank::send(_Payload	*p)	const{

				Node::Get()->send(this,p);
			}

			inline	uint32	_Crank::messageCount(){

				return	_messageCount;
			}

			inline	void	_Crank::push(P<_Payload>	&p){

				CircularBuffer<P<_Payload> >::push(p);
				_messageCount++;
			}

			inline	P<_Payload>	*_Crank::pop(bool	blocking){

				P<_Payload>	*p=CircularBuffer<P<_Payload> >::pop(blocking);
				_messageCount--;
				return	p;
			}

			inline	void	_Crank::_clear(){

				CircularBuffer<P<_Payload> >::_clear();
				_messageCount=0;
			}
		}
	}
}
