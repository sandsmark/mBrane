//	array.tpl.cpp
//
//	Author: Eric Nivel
//
//	BSD license:
//	Copyright (c) 2010, Eric Nivel
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

		template<typename	T,uint16	Size,class	Managed>	_Array<T,Size,Managed>::_Array():_count(0),current(&block),minIndex(0),maxIndex(Size-1),last(&block){
		}

		template<typename	T,uint16	Size,class	Managed>	_Array<T,Size,Managed>::~_Array(){
		}

		template<typename	T,uint16	Size,class	Managed>	inline	uint32	_Array<T,Size,Managed>::count()	const{

			return	_count;
		}
		
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		template<typename	T,uint16	Size,class	Managed>	Array<T,Size,Managed>::Array():_Array<T,Size,Managed>(){
		}

		template<typename	T,uint16	Size,class	Managed>	Array<T,Size,Managed>::~Array(){
		}

		template<typename	T,uint16	Size,class	Managed>	inline	T	*Array<T,Size,Managed>::get(uint32	i){

			if(i<this->minIndex){

				if(this->current->prev){

					this->minIndex-=Size;
					this->maxIndex-=Size;
					this->current=this->current->prev;
					return	get(i);
				}
				return	NULL;
			}else	if(i>this->maxIndex){

				if(this->current->next){

					this->minIndex+=Size;
					this->maxIndex+=Size;
					this->current=this->current->next;
					return	get(i);
				}
				return	NULL;
			}else
				return	this->current->data+i-this->minIndex;
		}

		template<typename	T,uint16	Size,class	Managed>	inline	T	&Array<T,Size,Managed>::operator	[]	(uint32	i){

			if(i<this->minIndex){

				if(this->current->prev){

					this->minIndex-=Size;
					this->maxIndex-=Size;
					this->current=this->current->prev;
					return	(*this)[i];
				}
			}else	if(i>this->maxIndex){

				if(!this->current->next)
					this->last=this->current->next=new	Block<T,Size,Managed>(this->current);

				this->minIndex+=Size;
				this->maxIndex+=Size;
				this->current=this->current->next;
				return	(*this)[i];
			}
			if(i>=this->_count)
				this->_count=i+1;
			return	this->current->data[i-this->minIndex];
		}

		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		template<typename	T,uint16	Size>	Array<T*,Size,ArrayManaged>::Array():_Array<T*,Size,ArrayManaged>(){
		}

		template<typename	T,uint16	Size>	Array<T*,Size,ArrayManaged>::~Array(){
		}

		template<typename	T,uint16	Size>	inline	T	**Array<T*,Size,ArrayManaged>::get(uint32	i){

			if(i<this->minIndex){

				if(this->current->prev){

					this->minIndex-=Size;
					this->maxIndex-=Size;
					this->current=this->current->prev;
					return	get(i);
				}
				return	NULL;
			}else	if(i>this->maxIndex){

				if(this->current->next){

					this->minIndex+=Size;
					this->maxIndex+=Size;
					this->current=this->current->next;
					return	get(i);
				}
				return	NULL;
			}else
				return	this->current->data+i-this->minIndex;
		}

		template<typename	T,uint16	Size>	inline	T	*&Array<T*,Size,ArrayManaged>::operator	[]	(uint32	i){

			if(i<this->minIndex){

				if(this->current->prev){

					this->minIndex-=Size;
					this->maxIndex-=Size;
					this->current=this->current->prev;
					return	(*this)[i];
				}
			}else	if(i>this->maxIndex){

				if(!this->current->next)
					this->last=this->current->next=new	Block<T*,Size,ArrayManaged>(this->current);

				this->minIndex+=Size;
				this->maxIndex+=Size;
				this->current=this->current->next;
				return	(*this)[i];
			}
			if(i>=this->_count)
				this->_count=i+1;
			if(!this->current->data[i-this->minIndex])
				return	this->current->data[i-this->minIndex]=new	T();
			else
				return	this->current->data[i-this->minIndex];
		}
	}
}
