//	array.tpl.cpp
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

namespace	mBrane{
	namespace	sdk{

		template<typename	T>	StaticArray<T>::StaticArray():_count(0),_array(NULL),_once(false){
		}

		template<typename	T>	StaticArray<T>::~StaticArray(){

			if(_array)
				delete[]	_array;
		}

		template<typename	T>	inline	T	*StaticArray<T>::data()	const{

			return	_array;
		}

		template<typename	T>	inline	uint32	StaticArray<T>::count()	const{

			return	_count;
		}

		template<typename	T>	void	StaticArray<T>::alloc(uint32	count){

			if(_once)
				return;
			_array=new	T[(_count=count)*sizeof(T)];
			_once=true;
		}

		template<typename	T>	inline	T	&StaticArray<T>::operator	[]	(uint32	i){

			return	_array[i];
		}
		
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		template<typename	T,uint16	Size>	Array<T,Size>::Array():next(NULL),local_count(0),total_count(0){
		}

		template<typename	T,uint16	Size>	Array<T,Size>::~Array(){

			if(next)
				delete	next;
		}

		template<typename	T,uint16	Size>	T	*Array<T,Size>::alloc(){

			if(local_count==Size){

				if(!next)
					next=new	Array<T,Size>();
				total_count++;
				return	next->alloc();
			}

			local_count++;
			total_count++;
			return	block+local_count-1;
		}

		template<typename	T,uint16	Size>	inline	T	*Array<T,Size>::get(uint32	i){

			if(i<Size)
				return	block+i;
			if(next)
				return	next->get(i);
			return	NULL;
		}

		template<typename	T,uint16	Size>	inline	uint32	Array<T,Size>::count()	const{

			return	total_count;
		}

		template<typename	T,uint16	Size>	inline	T	&Array<T,Size>::operator	[]	(uint32	i){

			if(i<Size){

				int32	delta=i-local_count;
				if(delta>=0){

					local_count=i+1;
					total_count+=delta+1;
				}

				return	block[i];
			}
			if(!next)
				next=new	Array<T,Size>();
			uint32	tc=next->total_count;
			T	&r=next->operator	[](i-Size);
			total_count+=next->total_count-tc;
			return	r;
		}

		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		template<typename	T,uint16	Size>	AArray<T,Size>::AArray():next(NULL),local_count(0),total_count(0){

			memset(block,NULL,Size*sizeof(T	*));

		}

		template<typename	T,uint16	Size>	AArray<T,Size>::~AArray(){

			if(next)
				delete	next;
			for(uint32	i=0;i<Size;i++)
				if(block[i])
					delete	block[i];
		}

		template<typename	T,uint16	Size>	T	**AArray<T,Size>::alloc(){

			if(local_count==Size){

				if(!next)
					next=new	AArray<T,Size>();
				total_count++;
				return	next->alloc();
			}

			local_count++;
			total_count++;
			return	block+local_count-1;
		}

		template<typename	T,uint16	Size>	inline	T	**AArray<T,Size>::get(uint32	i){

			if(i<Size)
				return	block+i;
			if(next)
				return	next->get(i);
			return	NULL;
		}

		template<typename	T,uint16	Size>	inline	uint32	AArray<T,Size>::count()	const{

			return	total_count;
		}

		template<typename	T,uint16	Size>	inline	T	*&AArray<T,Size>::operator	[]	(uint32	i){

			if(i<Size){

				int32	delta=i-local_count;
				if(delta>=0){

					local_count=i+1;
					total_count+=delta+1;
				}

				if(!block[i])
					block[i]=new	T();
				return	block[i];
			}
			if(!next)
				next=new	AArray<T,Size>();
			uint32	tc=next->total_count;
			T	*&r=next->operator	[](i-Size);
			total_count+=next->total_count-tc;
			return	r;
		}
	}
}