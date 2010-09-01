//	list.tpl.cpp
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

		template<typename	T,uint16	Size>	List<T,Size>::_List::_List():Array<ListElement<T>,Size>(){
		}

		template<typename	T,uint16	Size>	List<T,Size>::_List::~_List(){
		}

		template<typename	T,uint16	Size>	void	List<T,Size>::_List::clear(){

			for(uint32	i=0;i<Size;i++){

				if(i>0)
					this->block.data[i].prev=this->block.data+i-1;
				else
					this->block.data[i].prev=NULL;
				if(i<Size-1)
					this->block.data[i].next=this->block.data+i+1;
				else
					this->block.data[i].next=NULL;
			}

			if(this->block.next){

				delete	this->block.next;
				this->block.next=this->block.prev=NULL;
				this->last=&this->block;
			}

			this->_count=Size-1;
		}

		template<typename	T,uint16	Size>	ListElement<T>	*List<T,Size>::_List::expand(){

			this->last->next=new	Block<ListElement<T>,Size,ArrayUnmanaged>(this->last);
			this->last=this->last->next;
			return	this->last->data;
		}

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		template<typename	T,uint16	Size>	List<T,Size>::List(){

			clear();
		}

		template<typename	T,uint16	Size>	List<T,Size>::~List(){
		}

		template<typename	T,uint16	Size>	inline	uint32	List<T,Size>::elementCount()	const{

			return	_elementCount;
		}

		template<typename	T,uint16	Size>	inline	void	List<T,Size>::clear(){

			first=last=NULL;
			_elementCount=0;
			data.clear();
			firstFree=data.get(0);
			lastFree=data.get(0)+Size-1;
		}

		template<typename	T,uint16	Size>	inline	ListElement<T>	*List<T,Size>::getFreeSlot(){

			if(!firstFree)
				firstFree=data.expand();

			ListElement<T>	*freeSlot=firstFree;
			firstFree=firstFree->next;
			_elementCount++;

			if(freeSlot->prev)
				freeSlot->prev->next=freeSlot->next;
			if(freeSlot->next)
				freeSlot->next->prev=freeSlot->prev;

			if(!first){

				first=freeSlot;
				last=freeSlot;
			}
			
			return	freeSlot;
		}

		template<typename	T,uint16	Size>	inline	void	List<T,Size>::remove(ListElement<T>	*o){

			if(o->prev)
				o->prev->next=o->next;
			if(o->next)
				o->next->prev=o->prev;
			if(first==o)
				first=o->next;
			if(last==o)
				last=o->prev;
			lastFree->next=o;
			o->prev=lastFree;
			o->next=NULL;
			lastFree=o;
			_elementCount--;
		}

		template<typename	T,uint16	Size>	inline	void	List<T,Size>::removeElement(T	*t){

			remove((ListElement<T>	*)t);
		}

		template<typename	T,uint16	Size>	inline	uint32	List<T,Size>::removeReturnNext(ListElement<T>	*o){

			remove(o);
			return	o->next;
		}

		template<typename	T,uint16	Size>	inline	uint32	List<T,Size>::removeReturnPrevious(ListElement<T>	*o){

			remove(o);
			return	o->prev;
		}

		template<typename	T,uint16	Size>	inline	typename	List<T,Size>::Iterator	List<T,Size>::insertAfter(ListElement<T>	*o,T	&t){

			ListElement<T>	*target=getFreeSlot();
			if(o){

				target->next=o->next;
				if(o->next)
					o->next->prev=target;
				o->next=target;
			}else
				target->next=NULL;
			target->prev=o;
			target->data=t;
			if(last==o)
				last=target;
			return	Iterator(this,target);
		}

		template<typename	T,uint16	Size>	inline	typename	List<T,Size>::Iterator	List<T,Size>::insertBefore(ListElement<T>	*o,T	&t){

			ListElement<T>	target=getFreeSlot();
			if(o){

				target->prev=o->prev;
				if(o->prev)
					o->prev->next=target;
				o->prev=target;
			}else
				target->prev=NULL;
			target->next=o;
			target->data=t;
			if(first==o)
				first=target;
			return	Iterator(this,target);
		}

		template<typename	T,uint16	Size>	inline	typename	List<T,Size>::Iterator	List<T,Size>::addElementHead(T	&t){

			return	insertBefore(first,t);
		}
		
		template<typename	T,uint16	Size>	inline	typename	List<T,Size>::Iterator	List<T,Size>::addElementTail(T	&t){

			return	insertAfter(last,t);
		}

		template<typename	T,uint16	Size>	inline	typename	List<T,Size>::Iterator	List<T,Size>::addElement(T	&t){

			return	insertBefore(first,t);
		}
	}
}
