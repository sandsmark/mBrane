//	payload_utils.h
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

#ifndef mBrane_sdk_payload_utils_h
#define mBrane_sdk_payload_utils_h

#include	"payload.h"

//	Classes the instances of which are to be embedded in messages instances
namespace	mBrane{
	namespace	sdk{
		namespace	payloads{

			template<typename	T,uint32	_S,class	M>	class	List;
			template<typename	T,uint32	_S,class	M>	class	Array:
			public	RPayload<M,Array<T,_S,M> >,
			public	_DynamicData{
			template<typename	T,uint32	_S,class	M>	friend	class	List;
			protected:
				P<Array<T,_S,M> >	next;
				uint32	_count;
				T	block[_S];
				Array<T,_S,M>	*add();
			public:
				static	const	size_t	CoreSize();
				Array();
				~Array();
				uint8			ptrCount()	const;
				P<_RPayload>	*ptr(uint8	i);
				size_t	dynamicSize()	const;
				uint32	count()	const;
				T	&operator	[](uint32	i);	//	allocs a new block if i>=_count
				void	clear();
			};

			template<typename	T,uint32	S,class	M>	class	Pipe:
			public	RPayload<M,Pipe<T,S,M> >{
			protected:
				uint32	_count;
				uint32	head;
				uint32	tail;
				uint32	freeSlots;
				T	buffer[S];
			public:
				Pipe();
				~Pipe();
				T	*push();	//	returns a pointer to the newly pushed slot in the buffer
				T	*pop();		//	returns a pointer to the popped slot, still in the buffer (warning: might be overwritten by subsequent pushes)
				uint32	count()	const;
				void	clear();
			};

			template<typename	T,uint32	_S,class	M>	class	List:
			public	RPayload<M,List<T,_S,M> >,
			public	_DynamicData{
			public:
				class	Iterator{
				friend	class	List;
				private:
					const	List	*list;
					uint32	index;
					Iterator(const	List	*l,uint32	index):list(l),index(index){}
				public:
					Iterator():list(NULL),index(NullIndex){}
					Iterator(Iterator	&i):list(i.list),index(i.index){}
					~Iterator(){}
					Iterator	&operator	=(Iterator	&i){	list=i.list;	index=i.index;	return	*this;	}
					Iterator	&operator	++(){	if(index!=NullIndex)	index=list->_array[index].next;	return	*this;	}
					Iterator	&operator	--(){	if(index!=NullIndex)	index=list->_array[index].prev;	return	*this;	}
					bool	operator	==(Iterator	&i)	const{	return	index==i.index;	}
					bool	operator	!=(Iterator	&i)	const{	return	index!=i.index;	}
					bool	operator	!()	const{	return	index==NullIndex;	}
					operator	T&()	const{	return	list->_array[index].data;	}
					void	insertAfter(T	&t)	const{	list->insertAfter(index,t);	}
					void	insertBefore(T	&t)	const{	list->insertBefore(index,t);	}
					void	removeJumpNext()	const{	index=list->removeReturnNext(index);	}
					void	removeJumpPrevious()	const{	index=list->removeReturnPrevious(index);	}
				};
			protected:
				template<typename	T>	class	ListElement{
				public:
					T	data;
					uint32	next;
					uint32	prev;
				};
				static	const	uint32	NullIndex;
				uint32	first;
				uint32	last;
				uint32	_count;
				uint32	firstFree;
				uint32	lastFree;
				void	initArray(Array<ListElement<T>,_S,M>	*_array);
				uint32	getFreeSlot();
				void	remove(uint32	i);
				uint32	removeReturnNext(uint32	i);
				uint32	removeReturnPrevious(uint32	i);
				void	insertAfter(uint32	i,T	&t);
				void	insertBefore(uint32	i,T	&t);
				Array<ListElement<T>,_S,M>	_array;
			public:
				static	const	size_t	CoreSize();
				List();
				~List();
				uint8			ptrCount()	const;
				P<_RPayload>	*ptr(uint8	i);
				size_t	dynamicSize()	const;
				uint32	count()	const;
				void	clear();
				Iterator	&begin()	const{	return	Iterator(this,first);	}
				Iterator	&end()	const{	return	Iterator(this,last);	}
			};
		}
	}
}


#include	"payload_utils.tpl.cpp"


#endif