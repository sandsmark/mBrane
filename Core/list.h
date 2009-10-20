//	list.h
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
