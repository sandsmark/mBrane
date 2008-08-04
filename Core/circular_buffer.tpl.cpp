//	circular_buffer.tpl.cpp
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

#include	<memory.h>


namespace	mBrane{
	namespace	sdk{

		template<typename	T>	CircularBuffer<T>::CircularBuffer():Semaphore(0,65535),Mutex(){
		}

		template<typename	T>	CircularBuffer<T>::~CircularBuffer(){

			delete[]	buffer;
		}

		template<typename	T>	inline	void	CircularBuffer<T>::init(uint32	size){

			buffer=new	T[_size=size];
			_clear();
		}

		template<typename	T>	inline	void	CircularBuffer<T>::_clear(){

			head=tail=0;
			_count=0;
			freeSlots=_size;
		}

		template<typename	T>	inline	uint32	CircularBuffer<T>::size()	const{

			return	_size;
		}

		template<typename	T>	inline	uint32	CircularBuffer<T>::count()	const{

			return	_count;
		}

		template<typename	T>	inline	void	CircularBuffer<T>::push(T	&t){

			Mutex::acquire();

			if(!freeSlots){

				T	*oldBuffer=buffer;
				buffer=new	T[_size*2];
				memcpy(buffer,oldBuffer+head,sizeof(T)*(_size-head));
				memcpy(buffer,oldBuffer,sizeof(T)*(_size-tail));
				head=0;
				tail=_size-1;
				_size*=2;
				freeSlots=_size;
				delete[]	oldBuffer;
			}

			if(_count){

				if(++tail>=_size)
					tail=0;
			}
			buffer[tail]=t;

			freeSlots--;
			_count++;

			Semaphore::release();

			Mutex::release();
		}

		template<typename	T>	inline	T	*CircularBuffer<T>::pop(bool	blocking){

			if(blocking)
				Semaphore::acquire();
			else	if(Semaphore::acquire(0))
				return	NULL;

			Mutex::acquire();

			T	*t=buffer+head;
			if(++head>=_size)
				head=0;
			freeSlots++;
			_count--;

			Mutex::release();

			return	t;
		}

		template<typename	T>	inline	void	CircularBuffer<T>::clear(){

			Mutex::acquire();
			Semaphore::reset();
			_clear();
			Mutex::release();
		}
	}
}