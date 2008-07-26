//	circular_buffer.h
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

#ifndef mBrane_sdk_circular_buffer_h
#define mBrane_sdk_circular_buffer_h


namespace	mBrane{
	namespace	sdk{

		template<typename	T>	class	CircularBuffer{
		private:
			uint32	_size;
			T	*buffer;
			uint32	head;
			uint32	tail;
			uint32	freeSlots;
			uint32	_count;
		public:
			class	Iterator{
			friend	class	CircularBuffer;
			private:
				const	CircularBuffer	*buffer;
				uint32	index;
				Iterator(const	CircularBuffer	*b,uint32	index):buffer(b),index(index){}
			public:
				Iterator():buffer(NULL),index(0){}
				Iterator(Iterator	&i):buffer(i.buffer),index(i.index){}
				~Iterator(){}
				Iterator	&operator	=(Iterator	&i){	buffer=i.buffer;	index=i.index;	return	*this;	}
				Iterator	&operator	++(){	if(++index>=buffer->size())	index=0;	return	*this;	}
				bool	operator	==(Iterator	&i)	const{	return	index==i.index;	}
				bool	operator	!=(Iterator	&i)	const{	return	index!=i.index;	}
				operator	T&()	const{	return	buffer->buffer[index];	}
			};
			CircularBuffer(uint32	size);
			virtual	~CircularBuffer();
			uint32	size()	const;
			uint32	count()	const;
			void	push(T	&t);
			T	*pop();
			void	clear();
			Iterator	&begin()	const{	return	Iterator(this,head);	}
			Iterator	&end()	const{	return	Iterator(this,tail);	}
		};
	}
}


#include	"circular_buffer.tpl.cpp"


#endif