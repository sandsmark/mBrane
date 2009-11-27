//	memory.h
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

#ifndef mBrane_sdk_memory_h
#define mBrane_sdk_memory_h

#include	<cstdlib>

#include	"utils.h"
#include	"array.h"


//#define	MEMORY_MALLOC

namespace	mBrane{
	namespace	sdk{

		//	Memory allocator.
		//	One instance of Memory handles the allocation for instances of a given (fixed) size. Memory allocation is provided
		//	in the form of a segment of a larger Block. Blocks are alloacted to a certain size (objectSize x objectCount), and
		//	are linked together. ObjectSize is the smallest power of two minus one greater or equal than the object's size.
		//	ObjectSize start at 64 bytes.
		//	Memory holds statically as many instances of Memory as there is instance sizes (in 64 times a power of 2) to provide
		//	allocation for.
		//	Each Memory always holds at least one block, even if entirely free.
#ifndef	MEMORY_MALLOC
		class	dll	Memory{
		private:
			static	Array<Memory,16>	*Memories;
			static	CriticalSection		*CS;
			//	Blocks hold contiguous data storage, i.e. an array of segments (byte arrays of size objectSize)
			//	When not allocated, a segment contains a pointer to the next free segment: that's its first sizeof(uint8	*) bytes
			//	When deallocated, a segment is not initialized (except the pointer to the next free segment)
			//	When allocated a segment contains the address of its block (first sizeof(Block	*) bytes) - for speeding up deallocation:
			//	this means that the allocated object starts at segment+sizeof(Block	*)
			//	Blocks are allocated by malloc(sizeof(Block)+totalSize) and deallocated by free()
			//	Segments are allocated just after freeObjects, i.e. at ((uint8	*)this)+sizeof(Block)
			class	Block{
			friend	class	Memory;
			private:
				const	uint16	objectCount;
				uint16	freeObjects;
				const	size_t	objectSize;	//	power of 2; includes the pointer to the block
				const	size_t	totalSize;	//	objectCount x objectSize
				Block	*_next;
				Block	*_prev;
				uint8	*firstFree;		//	first free cell
				void	*operator	new(size_t	s,size_t	objectSize,uint16	objectCount);
				void	operator	delete(void	*b);
				Block(size_t	objectSize,uint16	objectCount);
				~Block();
				void	*alloc();				//	first sizeof(Block	*) bytes initialized to this, the rest is initialized to 0
				void	dealloc(void	*s);	//	s is a segment, not an object; first sizeof(uint8	*) bytes initialized to the next free segment, the rest is left untouched
			};
			const	size_t	objectSize;
			Block	*firstBlock;
			Block	*lastBlock;
			Block	*emptyBlock;	//	address of the empty block in the block list, if any; NULL otherwise
			uint32	freeObjects;
			CriticalSection	cs;
			Memory(size_t	objectSize);
			void	*operator	new(size_t	s,uint16	index);
			void	operator	delete(void	*b);
			static	size_t	GetNormalizedSize(size_t	s,uint16	&pow2);	//	converts a size into the next power of 2
		public:
			static	Memory	*Get(size_t	s);			//	called at class loading time; sequential calls
			static	Memory	*GetDynamic(size_t	s);	//	called dynamically (rwa storage); concurrent calls
			Memory();	//	do NOT use
			~Memory();
			uint32	getObjectSize()	const;
			void	*alloc();
			void	*alloc(uint32	&normalizedSize);	//	same as alloc; returns the allocated size as a power of 2
			void	dealloc(void	*o);
		};
#else
		class	dll	Memory{	//	for quick testing only!
		private:
			size_t	s;
			Memory(size_t	s):s(s){}
		public:
			static	Memory	*Get(size_t	s){	return	new	Memory(s);	}	//	memory leak: object->_Allocator is not deallocated; the right way would be to make a variant of Block instead.
			static	Memory	*GetDynamic(size_t	s){	return	new	Memory(s);	}
			~Memory(){}
			void	*alloc(){	return	malloc(s);	}
			void	*alloc(uint32	&normalizedSize){	normalizedSize=s;	return	malloc(s)}
			void	dealloc(void	*o){	free(o);	}
		};
#endif
	}
}


#endif
