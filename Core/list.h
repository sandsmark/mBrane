//	list.h
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

#ifndef mBrane_sdk_list_h
#define mBrane_sdk_list_h

#include	"array.h"


namespace	mBrane{
	namespace	sdk{

		template<typename	T>	class	ListElement{
		public:
			T	data;
			uint32	next;
			uint32	prev;
		};

		template<typename	T>	class	List:
		protected	Array<ListElement<T> >{
		public:
			class	Iterator{
			private:
				List	*list;
				uint32	index;
				Iterator(const	List	*l,uint32	index):list(l),index(index){}
			public:
				Iterator():list(NULL),index(0xFFFFFFFF){}
				Iterator(Iterator	&i):list(i.list),index(i.index){}
				~Iterator(){}
				Iterator	&operator	=(Iterator	&i){	list=i.list;	index=i.index;	return	*this;	}
				Iterator	&operator	++(){	if(list->buffer[index].next!=0xFFFFFFFF)	index=list->buffer[index].next;	return	*this;	}
				bool	operator	==(Iterator	&i)	const{	return	index==i.index;	}
				bool	operator	!=(Iterator	&i)	const{	return	index!=i.index;	}
				operator	T&()	const{	return	list->buffer[index].data;	}
			};
		protected:
			uint32	first;
			uint32	last;
			uint32	_elementCount;
		public:
			List(uint32	count=0);
			~List();
			uint32	elementCount()	const;
			void	clear();
			Iterator	&begin()	const{	return	Iterator(this,first);	}
			Iterator	&end()	const{	return	Iterator(this,last);	}
		};
	}
}


#include	"list.tpl.cpp"


#endif