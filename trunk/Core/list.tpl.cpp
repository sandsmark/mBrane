//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/ //_/_/ //_/_/  list.tpl.cpp //_/_/ //_/_/  Eric Nivel //_/_/  Center for Analysis and Design of Intelligent Agents //_/_/    Reykjavik University, Menntavegur 1, 101 Reykjavik, Iceland //_/_/    http://cadia.ru.is
//_/_/  Copyright©2012 //_/_/ //_/_/  This software was developed by the above copyright holder as part of  //_/_/  the HUMANOBS EU research project, in collaboration with the  //_/_/  following parties:
//_/_/   //_/_/  Autonomous Systems Laboratory //_/_/    Technical University of Madrid, Spain //_/_/    http://www.aslab.org/ //_/_/ //_/_/  Communicative Machines //_/_/    Edinburgh, United Kingdom //_/_/    http://www.cmlabs.com/ //_/_/ //_/_/  Istituto Dalle Molle di Studi sull'Intelligenza Artificiale //_/_/    University of Lugano and SUPSI, Switzerland //_/_/    http://www.idsia.ch/ //_/_/ //_/_/  Institute of Cognitive Sciences and Technologies //_/_/    Consiglio Nazionale delle Ricerche, Italy //_/_/    http://www.istc.cnr.it/
//_/_/
//_/_/  Dipartimento di Ingegneria Informatica
//_/_/    University of Palermo, Italy
//_/_/    http://roboticslab.dinfo.unipa.it/index.php/Main/HomePage
//_/_/ //_/_/
//_/_/  --- HUMANOBS Open-Source BSD License, with CADIA Clause v 1.0 ---
//_/_/
//_/_/  Redistribution and use in source and binary forms, with or without 
//_/_/  modification, is permitted provided that the following conditions 
//_/_/  are met:
//_/_/
//_/_/  - Redistributions of source code must retain the above copyright 
//_/_/    and collaboration notice, this list of conditions and the 
//_/_/    following disclaimer.
//_/_/
//_/_/  - Redistributions in binary form must reproduce the above copyright 
//_/_/    notice, this list of conditions and the following
//_/_/    disclaimer in the documentation and/or other materials provided 
//_/_/    with the distribution.
//_/_/
//_/_/  - Neither the name of its copyright holders nor the names of its 
//_/_/    contributors may be used to endorse or promote products 
//_/_/    derived from this software without specific prior written permission.
//_/_/
//_/_/  - CADIA Clause: The license granted in and to the software under this 
//_/_/     agreement is a limited-use license. The software may not be used in 
//_/_/     furtherance of: 
//_/_/     (i) intentionally causing bodily injury or severe emotional distress 
//_/_/         to any person; 
//_/_/     (ii) invading the personal privacy or violating the human rights of 
//_/_/         any person; or 
//_/_/     (iii) committing  or preparing for any act of war.
//_/_/
//_/_/  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
//_/_/  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
//_/_/  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR 
//_/_/  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
//_/_/  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
//_/_/  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
//_/_/  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, 
//_/_/  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY 
//_/_/  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
//_/_/  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
//_/_/  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//_/_/
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

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
