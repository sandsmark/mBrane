//	Pipe1.tpl.cpp
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

		template<typename	T,uint32	_S>	Pipe11<T,_S>::Pipe11():Semaphore(0,65535){

			head=tail=-1;
			first=last=new	Block(NULL);
			spare=NULL;
		}

		template<typename	T,uint32	_S>	Pipe11<T,_S>::~Pipe11(){

			delete	first;
			if(spare)
				delete	spare;
		}

		template<typename	T,uint32	_S>	inline	void	Pipe11<T,_S>::_clear(){	//	leaves spare as is

			enter();
			reset();
			if(first->next)
				delete	first->next;
			first->next=NULL;
			head=tail=-1;
			leave();
		}

		template<typename	T,uint32	_S>	inline	T	*Pipe11<T,_S>::_pop(){

			T	*t=first->buffer+head;
			if(++head==_S){

				enter();
				if(first==last)
					head=tail=-1;	//	stay in the same block; next push will reset head and tail to 0
				else{

					if(!spare){

						spare=first;
						first=first->next;
						spare->next=NULL;
					}else{

						Block	*b=first->next;
						first->next=NULL;
						delete	first;
						first=b;
					}
					head=0;
				}
				leave();
			}
			return	t;
		}

		template<typename	T,uint32	_S>	inline	void	Pipe11<T,_S>::push(T	&t){

			enter();
			if(++tail==0)
				head=0;
			uint32	index=tail;
			if(tail==_S){

				if(spare){

					last->next=spare;
					last=spare;
					last->next=NULL;
					spare=NULL;
				}else
					last=new	Block(last);
				tail=0;
				index=tail;
			}
			leave();

			last->buffer[index]=t;
			release();
		}

		template<typename	T,uint32	_S>	inline	T	*Pipe11<T,_S>::pop(){

			Semaphore::acquire();
			return	_pop();
		}

		template<typename	T,uint32	_S>	inline	void	Pipe11<T,_S>::clear(){

			_clear();
		}

		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		template<typename	T,uint32	_S>	Pipe1N<T,_S>::Pipe1N() {
		}

		template<typename	T,uint32	_S>	Pipe1N<T,_S>::~Pipe1N(){
		}

		template<typename	T,uint32	_S>	void	Pipe1N<T,_S>::clear(){

			popCS.enter();
			Pipe11<T,_S>::_clear();
			popCS.leave();
		}

		template<typename	T,uint32	_S>	T	*Pipe1N<T,_S>::pop(){

			Semaphore::acquire();
			popCS.enter();
			T	*t=Pipe11<T,_S>::_pop();
			popCS.leave();
			return	t;
		}

		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		template<typename	T,uint32	_S>	PipeN1<T,_S>::PipeN1() {
		}

		template<typename	T,uint32	_S>	PipeN1<T,_S>::~PipeN1(){
		}

		template<typename	T,uint32	_S>	void	PipeN1<T,_S>::clear(){

			pushCS.enter();
			Pipe11<T,_S>::_clear();
			pushCS.leave();
		}

		template<typename	T,uint32	_S>	void	PipeN1<T,_S>::push(T	&t){

			pushCS.enter();
			Pipe11<T,_S>::push(t);
			pushCS.leave();
		}

		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		template<typename	T,uint32	_S>	PipeNN<T,_S>::PipeNN() {
		}

		template<typename	T,uint32	_S>	PipeNN<T,_S>::~PipeNN(){
		}

		template<typename	T,uint32	_S>	void	PipeNN<T,_S>::clear(){

			pushCS.enter();
			popCS.enter();
			Pipe11<T,_S>::_clear();
			popCS.leave();
			pushCS.leave();
		}

		template<typename	T,uint32	_S>	void	PipeNN<T,_S>::push(T	&t){

			pushCS.enter();
			Pipe11<T,_S>::push(t);
			pushCS.leave();
		}

		template<typename	T,uint32	_S>	T	*PipeNN<T,_S>::pop(){

			Semaphore::acquire();
			popCS.enter();
			T	*t=Pipe11<T,_S>::_pop();
			popCS.leave();
			return	t;
		}
	}
}
