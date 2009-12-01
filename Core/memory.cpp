//	memory.cpp
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

#include	<memory>
#include	<cstring>
#include	"object.h"
#include	"memory.h"


#define	BLOCK_OBJECT_COUNT	2048	//	number of objects in a block

#ifndef	MEMORY_MALLOC

namespace	mBrane{
	namespace	sdk{

		inline	void	*Memory::Block::operator new(size_t	s,size_t	objectSize,uint16	objectCount){

			void	*a=malloc(s+objectCount*objectSize);	//	NEEDED OPTIMIZATION: allocate from a private heap
			memset(a,0x0,objectCount*objectSize);
			return	a;
		}				

		inline	void	Memory::Block::operator	delete(void	*b){

			free(b);
		}

		inline	Memory::Block::Block(size_t	objectSize,uint16	objectCount):objectCount(objectCount),
																			objectSize(objectSize),
																			totalSize(objectCount*objectSize),
																			_next(NULL),
																			_prev(NULL),
																			freeObjects(objectCount){

			firstFree=((uint8	*)this)+sizeof(Block);
			uint8	*p=firstFree;
			uint8	*next_p;
			for(uint16	i=0;i<objectCount-1;i++){	//	initializes the pointers from each segment to the next free one; this loop can be eliminated (see Block::alloc): but some basic preliminary tests show that what we gain here is lost in the extra tests in Block::alloc

				next_p=p+objectSize;
				*(uint8	**)p=next_p;
				p=next_p;
			}
			*(uint8	**)p=NULL;
		}

		inline	Memory::Block::~Block(){

			if(_next)
				delete	_next;
		}

		inline	void	*Memory::Block::alloc(){	//	called only if freeObjects>0
			
			uint8	*segment=firstFree;
			if(--freeObjects){	//	make firstFree the next free segment if any
				/* alternative: allows removing the loop in Block::Block()
				if(!*(uint8	**)segment)	//	if no link to the next free block, jump to the next segment in the array; it is safe to do so since (a) allocations are made from the beginning of the block and (b) deallocations move the firstFree to the deallocated block
					firstFree=segment+objectSize;
				else*/
					firstFree=*(uint8	**)segment;
			}else
				firstFree=NULL;
			//memset(segment,0x0,objectSize);
			*(Block	**)segment=this;
			return	(void	*)(segment+sizeof(Block	*));
		}

		inline	void	Memory::Block::dealloc(void	*s){
			
			//	have s point to firstFree and make s the firstFree segment
			*(uint8	**)s=firstFree;
			firstFree=(uint8	*)s;
			++freeObjects;
		}

		////////////////////////////////////////////////////////////////////////

		Array<Memory,16>	*Memory::Memories=NULL;	//	indexed by exponents of 2

		CriticalSection		*Memory::CS=NULL;

		inline	size_t	Memory::GetNormalizedSize(size_t	s,uint16	&pow2){

			size_t	segmentSize=s+sizeof(Block	*);	//	makes room for a pointer to the block
			size_t	objectSize=64;
			pow2=0;	//	smallest pow2 such as (objectSize=2^pow2) >= s
			if(segmentSize>64){

				pow2=BSR(segmentSize)-5;
				#if defined	ARCH_64
					pow2 -= 32;
				#endif
				objectSize=segmentSize-1;
				objectSize|=objectSize>>1;
				objectSize|=objectSize>>2;
				objectSize|=objectSize>>4;
				objectSize|=objectSize>>8;
				objectSize|=objectSize>>16;
				++objectSize;
			}
			return	objectSize;
		}

		Memory	*Memory::Get(size_t	s){	//	s>0; called early, once per class, at static member init time
			
			uint16	i;
			size_t	objectSize=GetNormalizedSize(s,i);

			if(!CS){	//	entered once early at class loading time; TODO: dealloc when the app terminates
				printf("New CS[%u]!\n", s);
				fflush(stdout);
				CS=new	CriticalSection();
				Memories=new	Array<Memory,16>();
			}
			Memory	*m=Memories->get(i);
			if (m==NULL) {
				printf("Use CS[%u/%u]!\n", s, objectSize);
				fflush(stdout);
			}
			//if ( (m == NULL) || (m->objectSize==0) )
			if (m->objectSize==0)
				m=new(i)	Memory(objectSize);
			printf("Done CS[%u]!\n", s);
			fflush(stdout);
			return	m;
		}

		inline	Memory	*Memory::GetDynamic(size_t	s){	//	s>0; called anytime, for raw storage
			
			uint16	i;
			size_t	objectSize=GetNormalizedSize(s,i);

			CS->enter();	//	concurrent access for raw storage requests
			Memory	*m=Memories->get(i);
			if(m->objectSize==0)
				m=new(i)	Memory(objectSize);
			CS->leave();
			return	m;
		}

		inline	void	*Memory::operator	new(size_t	s,uint16	index){	//	already allocated in Memories

			return	&Memories->operator [](index);
		}

		inline	void	Memory::operator	delete(void	*b){	//	will be deallocated when Memories is
		}

		Memory::Memory():objectSize(0){	//	uninitialized (object size == 0); called at Array<Memory,16> construction time
		}

		inline	Memory::Memory(size_t	objectSize):objectSize(objectSize){	//	initialized; explicit call in Memory::Get(size_t	s)

			emptyBlock=firstBlock=lastBlock=new(objectSize,BLOCK_OBJECT_COUNT)	Block(objectSize,BLOCK_OBJECT_COUNT);
			freeObjects=BLOCK_OBJECT_COUNT;
		}

		inline	Memory::~Memory(){

			delete	firstBlock;	//	blocks propagate deletion: careful when deleting just one block
		}

		inline	uint32	Memory::getObjectSize()	const{

			return	objectSize;
		}

		void	*Memory::alloc(uint32	&normalizedSize){

			normalizedSize=objectSize;
			return	alloc();
		}

		void	*Memory::alloc(){	//	concurrent access

			void	*allocated;
			Block	*b;
			cs.enter();
			if(freeObjects){	//	get the first block with a free segment

				for(b=firstBlock;!b->freeObjects;b=b->_next);	//	NEEDED OPTIMIZATION: either reduce the number of blocks, or keep track of the blocks that still have some room - or both
				--freeObjects;
				if(b==emptyBlock)
					emptyBlock=NULL;
			}else{	//	appends one block to the block list

				b=new(objectSize,BLOCK_OBJECT_COUNT)	Block(objectSize,BLOCK_OBJECT_COUNT);
				lastBlock->_next=b;
				b->_prev=lastBlock;
				lastBlock=b;
				freeObjects+=BLOCK_OBJECT_COUNT-1;	//	+BLOCK_OBJECT_COUNT for the new block, -1 for the allocated object
			}
			allocated=b->alloc();
			cs.leave();
			return	allocated;
		}

		void	Memory::dealloc(void	*o){	//	concurrent access

			Block	*b;
			cs.enter();
			//	find the block holding o
			uint8	*segment=(uint8	*)(((uint8	**)o)-1);
			b=*(Block	**)segment;	//	o was allocated from a segment in which the first 4 bytes are the address of the block
			//	dealloc the segment in that block
			b->dealloc(segment);
			//	if the block is empty and if there is already an empty block, delete the block
			if(b->freeObjects==b->objectCount){

				if(emptyBlock){

					if(b->_prev)
						b->_prev->_next=b->_next;
					else	//	b was first
						firstBlock=b->_next;
					if(b->_next)
						b->_next->_prev=b->_prev;
					else	//	b was last
						lastBlock=b->_prev;
					freeObjects+=1-BLOCK_OBJECT_COUNT;	//	+1 for the deallocated object, -BLOCK_OBJECT_COUNT for the deallocated block
					b->_next=NULL;	//	to prevent deletion cascade in ~Block()
					delete	b;
				}else{

					emptyBlock=b;
					++freeObjects;
				}
			}else
				++freeObjects;
			cs.leave();
		}
	}
}

#endif
