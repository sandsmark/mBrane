//	list.tpl.cpp
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

namespace	mBrane{
	namespace	sdk{

		template<typename	T,bool	(*B)(T	&,T	&)>	const	uint32	List<T,B>::NullIndex=0xFFFFFFFF;

		template<typename	T,bool	(*B)(T	&,T	&)>	List<T,B>::List(uint32	count):Array<ListElement<T> >(count){

			clear();
		}

		template<typename	T,bool	(*B)(T	&,T	&)>	List<T,B>::~List(){
		}

		template<typename	T,bool	(*B)(T	&,T	&)>	inline	uint32	List<T,B>::elementCount()	const{

			return	_elementCount;
		}

		template<typename	T,bool	(*B)(T	&,T	&)>	inline	void	List<T,B>::initFreeZone(uint32	start){

			for(uint32	i=start;i<_count;i++){

				if(i>start)
					get(i)->prev=i-1;
				else
					get(i)->prev=NullIndex;
				if(i<_count-1)
					get(i)->next=i+1;
				else
					get(i)->next=NullIndex;
			}
			firstFree=start;
			lastFree=_count-1;
		}

		template<typename	T,bool	(*B)(T	&,T	&)>	inline	void	List<T,B>::clear(){

			first=last=NullIndex;
			_elementCount=0;
			initFreeZone(0);
		}

		template<typename	T,bool	(*B)(T	&,T	&)>	inline	void	List<T,B>::remove(uint32	i){

			if(get(i)->prev!=NullIndex)
				get(get(i)->prev)->next=get(i)->next;
			if(get(i)->next!=NullIndex)
				get(get(i)->next)->prev=get(i)->prev;
			if(first==i)
				first=get(i)->next;
			if(last==i)
				last=get(i)->prev;
			get(lastFree)->next=i;
			get(i)->prev=lastFree;
			get(i)->next=NullIndex;
			lastFree=i;
			if(--_elementCount==0){

				first=NullIndex;
				last=NullIndex;
			}
		}

		template<typename	T,bool	(*B)(T	&,T	&)>	inline	void	List<T,B>::removeElement(T	*t){

			ListElement<T>	*e=(ListElement<T>	*)t;
			remove(e-get(0));
		}

		template<typename	T,bool	(*B)(T	&,T	&)>	inline	uint32	List<T,B>::removeReturnNext(uint32	i){

			remove(i);
			return	get(i)->next;
		}

		template<typename	T,bool	(*B)(T	&,T	&)>	inline	uint32	List<T,B>::removeReturnPrevious(uint32	i){

			remove(i);
			return	get(i)->prev;
		}

		template<typename	T,bool	(*B)(T	&,T	&)>	inline	typename	List<T,B>::Iterator	List<T,B>::insertAfter(uint32	i,T	&t){

			uint32	target=getFreeSlot();
			get(target)->next=get(i)->next;
			get(target)->prev=i;
			get(target)->data=t;
			if(get(i)->next!=NullIndex)
				get(get(i)->next)->prev=target;
			if(last==i)
				last=target;
			return	Iterator(this,target);
		}

		template<typename	T,bool	(*B)(T	&,T	&)>	inline	typename	List<T,B>::Iterator	List<T,B>::insertBefore(uint32	i,T	&t){

			uint32	target=getFreeSlot();
			get(target)->prev=get(i)->prev;
			get(target)->next=i;
			get(target)->data=t;
			if(get(i)->prev!=NullIndex)
				get(get(i)->prev)->next=target;
			if(first==i)
				first=target;
			return	Iterator(this,target);
		}

		template<typename	T,bool	(*B)(T	&,T	&)>	inline	uint32	List<T,B>::getFreeSlot(){

			if(_elementCount==_count){

				uint32	oldCount=_count;
				alloc(_count);
				initFreeZone(oldCount);
			}

			uint32	freeSlot=firstFree;
			firstFree=get(firstFree)->next;
			_elementCount++;

			if(first==NullIndex){

				first=freeSlot;
				last=freeSlot;
			}

			if(get(freeSlot)->prev!=NullIndex)
				get(get(freeSlot)->prev)->next=get(freeSlot)->next;
			if(get(freeSlot)->next!=NullIndex)
				get(get(freeSlot)->next)->prev=get(freeSlot)->prev;
			
			return	freeSlot;
		}

		template<typename	T,bool	(*B)(T	&,T	&)>	inline	typename	List<T,B>::Iterator	List<T,B>::addElementHead(T	&t){

			return	insertBefore(first,t);
		}
		
		template<typename	T,bool	(*B)(T	&,T	&)>	inline	typename	List<T,B>::Iterator	List<T,B>::addElementTail(T	&t){

			return	insertAfter(last,t);
		}

		template<typename	T,bool	(*B)(T	&,T	&)>	inline	typename	List<T,B>::Iterator	List<T,B>::addElement(T	&t){

			if(!B)	//	calls for a partial specialization, i.e. template<typename	T>	List;
				return	insertBefore(first,t);

			for(uint32	i=first;i!=NullIndex;i=get(i)->next){

				if(B(t,get(i)->data)){

					return	insertBefore(i,t);
				}
			}

			return	insertAfter(last,t);
		}
		
		template<typename	T,bool	(*B)(T	&,T	&)>	inline	void	List<T,B>::removeElement(T	&t){

			for(uint32	i=first;i!=NullIndex;i=get(i)->next){

				if(get(i)->data==t){

					remove(i);
					return;
				}
			}
		}
	}
}