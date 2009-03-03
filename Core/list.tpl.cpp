//	list.tpl.cpp
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

		template<typename	T,uint16	Size,bool	(*B)(T	&,T	&)>	const	uint32	List<T,Size,B>::NullIndex=0xFFFFFFFF;

		template<typename	T,uint16	Size,bool	(*B)(T	&,T	&)>	List<T,Size,B>::List():Array<ListElement<T>,Size>(){

			clear();
		}

		template<typename	T,uint16	Size,bool	(*B)(T	&,T	&)>	List<T,Size,B>::~List(){
		}

		template<typename	T,uint16	Size,bool	(*B)(T	&,T	&)>	inline	uint32	List<T,Size,B>::elementCount()	const{

			return	_elementCount;
		}

		template<typename	T,uint16	Size,bool	(*B)(T	&,T	&)>	inline	void	List<T,Size,B>::alloc(){

			if(total_count>0)
				return;
			Array<ListElement<T>,Size>::alloc();
			initFreeZone(0);
		}

		template<typename	T,uint16	Size,bool	(*B)(T	&,T	&)>	inline	void	List<T,Size,B>::initFreeZone(uint32	start){

			if(total_count==0){

				firstFree=0;
				lastFree=0;
			}else{

				for(uint32	i=start;i<total_count;i++){

					if(i>start)
						get(i)->prev=i-1;
					else
						get(i)->prev=NullIndex;
					if(i<total_count-1)
						get(i)->next=i+1;
					else
						get(i)->next=NullIndex;
				}
				firstFree=start;
				lastFree=total_count-1;
			}
		}

		template<typename	T,uint16	Size,bool	(*B)(T	&,T	&)>	inline	void	List<T,Size,B>::clear(){

			first=last=NullIndex;
			_elementCount=0;
			initFreeZone(0);
		}

		template<typename	T,uint16	Size,bool	(*B)(T	&,T	&)>	inline	void	List<T,Size,B>::remove(uint32	i){

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
			_elementCount--;
		}

		template<typename	T,uint16	Size,bool	(*B)(T	&,T	&)>	inline	void	List<T,Size,B>::removeElement(T	*t){

			ListElement<T>	*e=(ListElement<T>	*)t;
			remove((e-get(0))/sizeof(ListElement<T>));
		}

		template<typename	T,uint16	Size,bool	(*B)(T	&,T	&)>	inline	uint32	List<T,Size,B>::removeReturnNext(uint32	i){

			remove(i);
			return	get(i)->next;
		}

		template<typename	T,uint16	Size,bool	(*B)(T	&,T	&)>	inline	uint32	List<T,Size,B>::removeReturnPrevious(uint32	i){

			remove(i);
			return	get(i)->prev;
		}

		template<typename	T,uint16	Size,bool	(*B)(T	&,T	&)>	inline	typename	List<T,Size,B>::Iterator	List<T,Size,B>::insertAfter(uint32	i,T	&t){

			uint32	target=getFreeSlot();
			if(i!=NullIndex){

				get(target)->next=get(i)->next;
				if(get(i)->next!=NullIndex)
					get(get(i)->next)->prev=target;
				get(i)->next=target;
			}
			get(target)->prev=i;
			get(target)->data=t;
			if(last==i)
				last=target;
			if(first==i)
				first=target;
			return	Iterator(this,target);
		}

		template<typename	T,uint16	Size,bool	(*B)(T	&,T	&)>	inline	typename	List<T,Size,B>::Iterator	List<T,Size,B>::insertBefore(uint32	i,T	&t){

			uint32	target=getFreeSlot();
			if(i!=NullIndex){

				get(target)->prev=get(i)->prev;
				if(get(i)->prev!=NullIndex)
					get(get(i)->prev)->next=target;
				get(i)->prev=target;
			}
			get(target)->next=i;
			get(target)->data=t;
			if(last==i)
				last=target;
			if(first==i)
				first=target;
			return	Iterator(this,target);
		}

		template<typename	T,uint16	Size,bool	(*B)(T	&,T	&)>	inline	uint32	List<T,Size,B>::getFreeSlot(){

			if(_elementCount==total_count){

				uint32	oldCount=total_count;
				alloc();
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

		template<typename	T,uint16	Size,bool	(*B)(T	&,T	&)>	inline	typename	List<T,Size,B>::Iterator	List<T,Size,B>::addElementHead(T	&t){

			return	insertBefore(first,t);
		}
		
		template<typename	T,uint16	Size,bool	(*B)(T	&,T	&)>	inline	typename	List<T,Size,B>::Iterator	List<T,Size,B>::addElementTail(T	&t){

			return	insertAfter(last,t);
		}

		template<typename	T,uint16	Size,bool	(*B)(T	&,T	&)>	inline	typename	List<T,Size,B>::Iterator	List<T,Size,B>::addElement(T	&t){

			if(!B)	//	optimization: calls for a partial specialization, i.e. template<typename	T>	List;
				return	insertBefore(first,t);

			for(uint32	i=first;i!=NullIndex;i=get(i)->next){

				if(B(t,get(i)->data)){

					return	insertBefore(i,t);
				}
			}

			return	insertAfter(last,t);
		}
		
		template<typename	T,uint16	Size,bool	(*B)(T	&,T	&)>	inline	void	List<T,Size,B>::removeElement(T	&t){

			for(uint32	i=first;i!=NullIndex;i=get(i)->next){

				if(get(i)->data==t){

					remove(i);
					return;
				}
			}
		}
	}
}