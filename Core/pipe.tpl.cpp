//	Pipe.tpl.cpp
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

#include	<memory.h>


namespace	mBrane{
	namespace	sdk{

		template<typename	T,uint32	_S>	const	uint32	Pipe<T,_S>::NullIndex=0xFFFFFFFF;

		template<typename	T,uint32	_S>	Pipe<T,_S>::Pipe():Semaphore(0,65535){

			_clear();
			first=last=new	Block(NULL);
			spare=NULL;
		}

		template<typename	T,uint32	_S>	Pipe<T,_S>::~Pipe(){

			delete	first;
			if(spare)
				delete	spare;
		}

		template<typename	T,uint32	_S>	inline	void	Pipe<T,_S>::_clear(){

			head=tail=NullIndex;
		}

		template<typename	T,uint32	_S>	inline	void	Pipe<T,_S>::push(T	&t){
	
			if(tail==NullIndex)
				tail=head=0;
			last->buffer[tail]=t;
			if(++tail==_S){

				if(spare){

					last->next=spare;
					last=spare;
					spare=NULL;
				}else{

					Block	*b=new	Block(NULL);
					last->next=b;
					last=b;
				}
				tail=0;
			}

			Semaphore::release();
		}

		template<typename	T,uint32	_S>	inline	T	*Pipe<T,_S>::pop(bool	blocking){

			if(blocking)
				Semaphore::acquire();
			else	if(Semaphore::acquire(0))
				return	NULL;

			T	*t=first->buffer+head;
			if(++head=_S){

				if(first->next){

					if(!spare){

						spare=first;
						first=first->next;
					}else{

						Block	*b=first->next;
						delete	first;
						first=b;
					}
					head=0;
				}else
					head=tail=NullIndex;	//	stay in the same block; next push will reset head and tail to 0
			}

			return	t;
		}

		template<typename	T,uint32	_S>	inline	void	Pipe<T,_S>::clear(){

			Semaphore::reset();
			if(first->next)
				delete	first->next;
			_clear();
		}
	}
}