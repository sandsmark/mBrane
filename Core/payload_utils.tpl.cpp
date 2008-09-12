//	payload_utils.tpl.cpp
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
		namespace	payloads{

			template<typename	T,uint32	_S,class	M>	const	size_t	Array<T,_S,M>::CoreSize(){

				return	Size()-_S*sizeof(T);
			}

			template<typename	T,uint32	_S,class	M>	const	uint8	Array<T,_S,M>::PtrCount(){

				return	1;
			}

			template<typename	T,uint32	_S,class	M>	P<_RPayload>	*Array<T,_S,M>::Ptr(__Payload	*p,uint8	i){

				if(i==0)
					return	(P<_RPayload>	*)&(((Array<T,_S,M>	*)p)->next);
			}

			template<typename	T,uint32	_S,class	M>	Array<T,_S,M>::Array():RPayload<M,Array<T,_S,M> >(),_DynamicData(),next(NULL),_count(0){
			}

			template<typename	T,uint32	_S,class	M>	Array<T,_S,M>::~Array(){
			}

			template<typename	T,uint32	_S,class	M>	size_t	Array<T,_S,M>::dynamicSize()	const{

				return	_count;
			}

			template<typename	T,uint32	_S,class	M>	uint32	Array<T,_S,M>::count()	const{

				return	_count+(!next?0:next->count());
			}

			template<typename	T,uint32	_S,class	M>	T	&Array<T,_S,M>::operator	[](uint32	i){

				if(i<_S){

					if(i>=_count){

						_count=i;
						dynamicSize=_count;
					}
					return	block[i];
				}
				if(!next)
					next=new	Array<T,_S,M>();
				return	next->operator	[](i);
			}

			template<typename	T,uint32	_S,class	M>	void	Array<T,_S,M>::clear(){

				dynamicSize=_count=0;
				if(next)
					next->clear();
				next=NULL;
			}

			template<typename	T,uint32	_S,class	M>	Array<T,_S,M>	*Array<T,_S,M>::add(uint32	&offset){

				offset+=_S;
				if(!next)
					return	next=new	Array<T,_S,M>();
				return	next->add(offset);
			}

			////////////////////////////////////////////////////////////////////////////////////////////////

			template<typename	T,uint32	_S,class	M>	Pipe<T,_S,M>::Pipe():RPayload<M,Pipe<T,_S,M> >(){

				clear();
			}

			template<typename	T,uint32	_S,class	M>	Pipe<T,_S,M>::~Pipe(){
			}

			template<typename	T,uint32	_S,class	M>	uint32	Pipe<T,_S,M>::count()	const{

				return	_count;
			}

			template<typename	T,uint32	_S,class	M>	T	*Pipe<T,_S,M>::push(){

				if(!freeSlots)
					return	NULL;

				if(_count){

					if(++tail>=_size)
						tail=0;
				}
				freeSlots--;
				_count++;
				return	buffer+tail;
			}
			
			template<typename	T,uint32	_S,class	M>	T	*Pipe<T,_S,M>::pop(){

				T	*t=buffer+head;
				if(++head>=_size)
					head=0;
				freeSlots++;
				_count--;
				return	t;
			}

			template<typename	T,uint32	_S,class	M>	void	Pipe<T,_S,M>::clear(){

				head=tail=0;
				_count=0;
				freeSlots=_S;
			}

			////////////////////////////////////////////////////////////////////////////////////////////////

			template<typename	T,uint32	_S,class	M>	const	uint32	List<T,_S,M>::NullIndex=0xFFFFFFFF;

			template<typename	T,uint32	_S,class	M>	const	size_t	List<T,_S,M>::CoreSize(){

				return	Size()-Array<T,Size,M>::Size()+Array<T,Size,M>::CoreSize();
			}

			template<typename	T,uint32	_S,class	M>	const	uint8	List<T,_S,M>::PtrCount(){

				return	1;
			}

			template<typename	T,uint32	_S,class	M>	P<_RPayload>	*List<T,_S,M>::Ptr(__Payload	*p,uint8	i){

				return	((List<T,_S,M>	*)p)->_array.ptr(i);
			}

			template<typename	T,uint32	_S,class	M>	List<T,_S,M>::List():RPayload<M,List<T,_S,M> >(),_DynamicData(){
			}

			template<typename	T,uint32	_S,class	M>	List<T,_S,M>::~List(){
			}

			template<typename	T,uint32	_S,class	M>	size_t	List<T,_S,M>::dynamicSize()	const{

				return	_array.dynamicSize();
			}

			template<typename	T,uint32	_S,class	M>	uint32	List<T,_S,M>::count()	const{

				return	_count;
			}

			template<typename	T,uint32	_S,class	M>	void	List<T,_S,M>::initArray(Array<ListElement<T>,_S,M>	*_array,uint32	offset){

				for(uint32	i=0;i<_S;i++){

					if(i>0)
						_array[i].prev=i-1;
					else
						_array[i].prev=NullIndex;
					if(i<_S-1)
						_array[i].next=i+1;
					else
						_array[i].next=NullIndex;
				}
				firstFree=offset;
				lastFree=offset+_S-1;
			}

			template<typename	T,uint32	_S,class	M>	inline	void	List<T,_S,M>::clear(){

				first=last=NullIndex;
				_count=0;
				initArray(_array,0);
			}

			template<typename	T,uint32	_S,class	M>	inline	void	List<T,_S,M>::remove(uint32	i){

				if(_array[i].prev!=NullIndex)
					_array[_array[i].prev].next=_array[i].next;
				if(_array[i].next!=NullIndex)
					_array[_array[i].next].prev=_array[i].prev;
				if(first==i)
					first=_array[i].next;
				if(last==i)
					last=_array[i].prev;
				_array[lastFree].next=i;
				_array[i].prev=lastFree;
				_array[i].next=NullIndex;
				lastFree=i;
				_count--;
			}

			template<typename	T,uint32	_S,class	M>	inline	uint32	List<T,_S,M>::removeReturnNext(uint32	i){

				remove(i);
				return	_array[i].next;
			}

			template<typename	T,uint32	_S,class	M>	inline	uint32	List<T,_S,M>::removeReturnPrevious(uint32	i){

				remove(i);
				return	_array[i].prev;
			}

			template<typename	T,uint32	_S,class	M>	inline	void	List<T,_S,M>::insertAfter(uint32	i,T	&t){

				uint32	target=getFreeSlot();
				if(i!=NullIndex){

					_array[target].next=_array[i].next;
					if(_array[i].next!=NullIndex)
						_array[_array[i].next].prev=target;
					_array[i].next=target;
				}
				_array[target].prev=i;
				_array[target].data=t;
				if(last==i)
					last=target;
				if(first==i)
					first=target;
			}

			template<typename	T,uint32	_S,class	M>	inline	void	List<T,_S,M>::insertBefore(uint32	i,T	&t){

				uint32	target=getFreeSlot();
				if(i!=NullIndex){

					_array[target].prev=_array[i].prev;
					if(_array[i].prev!=NullIndex)
						_array[_array[i].prev].next=target;
					_array[i].prev=target;
				}
				_array[target].next=i;
				_array[target].data=t;
				if(last==i)
					last=target;
				if(first==i)
					first=target;
			}

			template<typename	T,uint32	_S,class	M>	inline	uint32	List<T,_S,M>::getFreeSlot(){

				if(_count==_S){

					uint32	offset=0;
					Array<ListElement<T>,_S,M>	*a=_array.add(offset);
					initArray(a,offset);
				}

				uint32	freeSlot=firstFree;
				firstFree=_array[firstFree].next;
				_count++;

				if(first==NullIndex){

					first=freeSlot;
					last=freeSlot;
				}

				if(_array[freeSlot].prev!=NullIndex)
					_array[_array[freeSlot].prev].next=_array[freeSlot].next;
				if(_array[freeSlot].next!=NullIndex)
					_array[_array[freeSlot].next].prev=_array[freeSlot].prev;
				
				return	freeSlot;
			}
		}
	}
}