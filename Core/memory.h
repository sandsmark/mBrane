// memory.h
//
// Author: Eric Nivel
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

#ifndef mBrane_sdk_memory_h
#define mBrane_sdk_memory_h

#include	<cstdlib>

#include	"types.h"


namespace	mBrane{
	namespace	sdk{

		class	Allocator{
		public:
			virtual	void	*alloc()=0;
			virtual	void	dealloc(void	*o)=0;
		};

		class	dll	Memory:
		public	Allocator{
		private:
			class	Array{
			private:
				Memory	*_array;
				uint32	_count;
			public:
				Array();
				~Array();
				Memory	*init(size_t	s);
				void	*alloc();
			};
			static	Array	Memories;
			class	Block{
				const	uint16	objectCount;
				const	size_t	objectSize;
				const	size_t	totalSize;
				Block	*_next;
				uint8	*firstFree;
				uint16	freeObjects;
				uint8	*begin;
				uint8	*end;
			public:
				void	*operator	new(size_t	s,size_t	objectSize,uint16	objectCount);
				void	operator	delete(void	*b);
				Block(size_t	objectSize,uint16	objectCount);
				~Block();
				Block	*next()	const;
				Block	*link(Block	*b);
				void	*alloc();
				uint8	dealloc(void	*o);
			};
			const	size_t	objectSize;
			Block	*firstBlock;
			Block	*lastBlock;
			uint32	freeObjects;
			Memory(size_t	objectSize);
			~Memory();
		public:
			static	Memory	*Get(size_t	s);
			void	*operator	new(size_t	s);
			void	operator	delete(void	*b);
			void	*alloc();
			void	dealloc(void	*o);
		};
	}
}


#endif