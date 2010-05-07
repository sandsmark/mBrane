//	array.h
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

#ifndef mBrane_sdk_array_h
#define mBrane_sdk_array_h

#include	"types.h"
#include	<string.h>

using	namespace	core;

namespace	mBrane{
	namespace	sdk{

		template<typename	T>	class	StaticArray{
		private:
			uint32	_count;
			T		*_array;
			bool	_once;
		public:
			StaticArray();
			~StaticArray();
			void	alloc(uint32	count);	//	to be called only once
			uint32	count()	const;
			T	&operator	[]	(uint32	i);	//	unprotected, i.e. do not let i>=count()
			T	*data()	const;
		};

		class	ArrayManaged{};
		class	ArrayUnmanaged{};

		template<typename	T,uint16	Size,class	Managed>	class	Block{
		public:
			Block(Block	*prev=NULL):next(NULL),prev(prev){}
			~Block(){	if(next)	delete	next;	}
			T		data[Size];
			Block	*next;
			Block	*prev;
		};

		//	Specialization of Block<T,Size> holding pointers and managing objects.
		template<typename	T,uint16	Size>	class	Block<T*,Size,ArrayManaged>{
		public:
			Block(Block	*prev=NULL):next(NULL),prev(prev){	memset(data,0,Size*sizeof(T));	}
			~Block(){	for(uint32	i=0;i<Size;i++)if(data[i])delete	data[i];	if(next)	delete	next;	}
			T		*data[Size];
			Block	*next;
			Block	*prev;
		};

		template<typename	T,uint16	Size,class	Managed>	class	_Array{
		protected:
			uint32					_count;
			Block<T,Size,Managed>	block;
			Block<T,Size,Managed>	*last;
			//	navigation optimization
			Block<T,Size,Managed>	*current;
			uint32					minIndex;
			uint32					maxIndex;
			_Array();
		public:
			virtual	~_Array();
			uint32	count()	const;
		};

		//	Dynamic array, i.e. a linked list of Block<T,Size>.
		template<typename	T,uint16	Size,class	Managed=ArrayUnmanaged>	class	Array:
		public	_Array<T,Size,Managed>{
		public:
			Array();
			~Array();
			T	*get(uint32	i);	//	no allocation performed
			T	&operator	[]	(uint32	i);	//	changes counts, i.e. makes sure that there's room. Doesn't mean any object has been assigned to the ith position
		};

		//	Specialization of _Array<T,Size> holding pointers and managing objects.
		//	Array<T*,Size> instantiates pointed objects (calling T()) whenever needed upon calls to operator [].
		//	Array<T*,Size> deletes pointed objects.
		template<typename	T,uint16	Size>	class	Array<T*,Size,ArrayManaged>:
		public	_Array<T*,Size,ArrayManaged>{
		public:
			Array();
			~Array();
			T	**get(uint32	i);	//	no allocation performed
			T	*&operator	[]	(uint32	i);	//	changes counts, i.e. makes sure that there's room. Doesn't mean any object has been assigned to the ith position
		};
	}
}


#include	"array.tpl.cpp"


#endif
