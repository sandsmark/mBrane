//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
//_/_/
//_/_/ HUMANOBS - mBrane
//_/_/
//_/_/ Eric Nivel
//_/_/ Center for Analysis and Design of Intelligent Agents
//_/_/   Reykjavik University, Menntavegur 1, 101 Reykjavik, Iceland
//_/_/   http://cadia.ru.is
//_/_/ Copyright(c)2012
//_/_/
//_/_/ This software was developed by the above copyright holder as part of 
//_/_/ the HUMANOBS EU research project, in collaboration with the 
//_/_/ following parties:
//_/_/ 
//_/_/ Autonomous Systems Laboratory
//_/_/   Technical University of Madrid, Spain
//_/_/   http://www.aslab.org/
//_/_/
//_/_/ Communicative Machines
//_/_/   Edinburgh, United Kingdom
//_/_/   http://www.cmlabs.com/
//_/_/
//_/_/ Istituto Dalle Molle di Studi sull'Intelligenza Artificiale
//_/_/   University of Lugano and SUPSI, Switzerland
//_/_/   http://www.idsia.ch/
//_/_/
//_/_/ Institute of Cognitive Sciences and Technologies
//_/_/   Consiglio Nazionale delle Ricerche, Italy
//_/_/   http://www.istc.cnr.it/
//_/_/
//_/_/ Dipartimento di Ingegneria Informatica
//_/_/   University of Palermo, Italy
//_/_/   http://roboticslab.dinfo.unipa.it/index.php/Main/HomePage
//_/_/
//_/_/
//_/_/ --- HUMANOBS Open-Source BSD License, with CADIA Clause v 1.0 ---
//_/_/
//_/_/ Redistribution and use in source and binary forms, with or without 
//_/_/ modification, is permitted provided that the following conditions 
//_/_/ are met:
//_/_/
//_/_/ - Redistributions of source code must retain the above copyright 
//_/_/ and collaboration notice, this list of conditions and the 
//_/_/ following disclaimer.
//_/_/
//_/_/ - Redistributions in binary form must reproduce the above copyright 
//_/_/ notice, this list of conditions and the following
//_/_/ disclaimer in the documentation and/or other materials provided 
//_/_/ with the distribution.
//_/_/
//_/_/ - Neither the name of its copyright holders nor the names of its 
//_/_/ contributors may be used to endorse or promote products 
//_/_/ derived from this software without specific prior written permission.
//_/_/
//_/_/ - CADIA Clause: The license granted in and to the software under this 
//_/_/ agreement is a limited-use license. The software may not be used in 
//_/_/ furtherance of: 
//_/_/ (i) intentionally causing bodily injury or severe emotional distress 
//_/_/ to any person; 
//_/_/ (ii) invading the personal privacy or violating the human rights of 
//_/_/ any person; or 
//_/_/ (iii) committing or preparing for any act of war.
//_/_/
//_/_/ THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
//_/_/ "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
//_/_/ LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR 
//_/_/ A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
//_/_/ OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
//_/_/ SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
//_/_/ LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, 
//_/_/ DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY 
//_/_/ THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
//_/_/ (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
//_/_/ OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//_/_/
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/ 

#ifndef mBrane_sdk_list_h
#define mBrane_sdk_list_h

#include	"array.h"


namespace	mBrane{
	namespace	sdk{

		template<typename	T>	class	ListElement{
		public:
			T			data;
			ListElement	*next;
			ListElement	*prev;
		};

		template<typename	T,uint16	Size>	class	List{
		public:
			class	Iterator{
			friend	class	List;
			private:
				List			*list;
				ListElement<T>	*object;
				Iterator(List	*l,ListElement<T>	*object):list(l),object(object){}
			public:
				Iterator():list(NULL),object(NULL){}
				Iterator(const Iterator	&i):list(i.list),object(i.object){}
				~Iterator(){}
				Iterator	&operator	=(const Iterator	&i){	list=i.list;	object=i.object;	return	*this;	}
				Iterator	&operator	++(){	object=object->next;	return	*this;	}
				Iterator	&operator	--(){	object=object->prev;	return	*this;	}
				bool	operator	==(const	Iterator	&i)	const{	return	object==i.object;	}
				bool	operator	!=(const	Iterator	&i)	const{	return	object!=i.object;	}
				operator	ListElement<T>	*(){	return	object;	}
				T&		operator	*()	const{	return	object->data;	}
				Iterator	insertAfter(T	&t)	const{	list->insertAfter(object,t);	return	Iterator(list,object->next);	}
				Iterator	insertBefore(T	&t)	const{	list->insertBefore(object,t);	return	Iterator(list,object->prev);	}
				void		remove(){	list->remove(object);	object=NULL;	}
				void		removeJumpNext(){	object=list->removeReturnNext(object);	}
				void		removeJumpPrevious(){	object=list->removeReturnPrevious(object);	}
			};
		protected:
			class	_List:
			public	Array<ListElement<T>,Size>{
			public:
				_List();
				~_List();
				void	clear();
				ListElement<T>	*expand();
			};

			_List	data;

			ListElement<T>	*first;
			ListElement<T>	*last;
			uint32			_elementCount;
			ListElement<T>	*firstFree;
			ListElement<T>	*lastFree;
			ListElement<T>	*getFreeSlot();
			void		remove(ListElement<T>	*o);
			uint32		removeReturnNext(ListElement<T>	*o);
			uint32		removeReturnPrevious(ListElement<T>	*o);
			Iterator	insertAfter(ListElement<T>	*o,T	&t);	//	returns the index of the new element
			Iterator	insertBefore(ListElement<T>	*o,T	&t);
		public:
			List();
			~List();
			uint32		elementCount()	const;
			void		clear();
			Iterator	addElementHead(T	&t);	//	returns the index of the new element
			Iterator	addElementTail(T	&t);
			Iterator	addElement(T	&t);	//	inserts in order with respect to the B function (i.e. new element "before" current element)
			void		removeElement(T	*t);
			Iterator	begin(){	Iterator i(this,first);	return i; }
			Iterator	end(){	Iterator i(this,last); return i;	}
		};
	}
}


#include	"list.tpl.cpp"


#endif
