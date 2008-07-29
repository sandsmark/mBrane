// array.tpl.cpp
//
// Author: Eric Nivel
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

namespace	mBrane{
	namespace	sdk{

		template<typename	T>	Array<T>::Array():_array(NULL),_count(0){
		}

		template<typename	T>	Array<T>::~Array(){

			if(_array)
				delete[]	_array;
		}

		template<typename	T>	T	*Array<T>::alloc(){

			if(_array){

				T	*oldArray=_array;
				_array=new	T[_count+1];
				memcpy(_array,oldArray,_count*sizeof(T));
				_count++;
			}else
				_array=new	T[(++_count)];
			
			return	_array+_count-1;
		}

		template<typename	T>	T	*Array<T>::alloc(uint32	&i){

			T	*r=alloc();
			i=_count-1;
			return	r;
		}

		template<typename	T>	inline	T	*Array<T>::get(uint32	i)	const{

			return	_array+i;
		}

		template<typename	T>	inline	uint32	Array<T>::count()	const{

			return	_count;
		}

		template<typename	T>	inline	Array<T>::operator	T	*(){

			return	_array;
		}
	}
}