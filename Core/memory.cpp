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
#include	"object.h"
#include	"memory.h"


#define	BLOCK_SIZE		1024
#define	BLOCK_SIZE_x_2	2048

namespace	mBrane{
	namespace	sdk{

		inline	void	*Memory::Block::operator new(size_t	s,size_t	objectSize,uint16	objectCount){

			return	malloc(s+objectCount*objectSize);
		}				

		inline	void	Memory::Block::operator	delete(void	*b){

			free(b);
		}

		inline	Memory::Block::Block(size_t	objectSize,uint16	objectCount):objectCount(objectCount),
																			objectSize(objectSize),
																			totalSize(objectCount*objectSize),
																			_next(NULL),
																			freeObjects(objectCount){

			begin=(uint8	*)(((uint8 *)this)+offsetof(Block,end)+sizeof(uint8	*));
			end=begin+totalSize;
			firstFree=begin;
			memset(begin,0xFF,totalSize);
		}

		inline	Memory::Block::~Block(){

			if(_next)
				delete	_next;
		}

		inline	Memory::Block	*Memory::Block::next()	const{
			
			return	_next;
		}

		inline	Memory::Block	*Memory::Block::link(Block	*b){
			
			return	_next=b;
		}

		inline	void	*Memory::Block::alloc(){
			
			if(!freeObjects)
				return	NULL;
			void	*p=firstFree;
			freeObjects--;
			if(freeObjects>1){

				if(*(uint32	*)firstFree==WORD_MASK)
					firstFree+=objectSize;
				else
					firstFree=*(uint8	**)firstFree;
			}
			memset(p,0x0,objectSize);
			return	p;
		}

		inline	uint8	Memory::Block::dealloc(void	*o){

			if(o<begin	||	o>=end)
				return	0;

			*(uint8	**)o=firstFree;
			firstFree=(uint8 *)o;
			return	(++freeObjects==objectCount?2:1);
		}

		////////////////////////////////////////////////////////////////////////

		Array<Memory,128>	*Memory::Memories=NULL;

		Array<Memory,128>	*Memory::Get(){

			if(!Memories)
				Memories=new	Array<Memory,128>();
			return	Memories;
		}

		inline	Memory	*Memory::Get(size_t	s){

			Memory	*m;
			for(uint32	i=0;i<Get()->count();i++){

				m=&(*Memories)[i];
				if(m->objectSize==s)
					return	m;
			}

			return	new	Memory(s);
		}

		void	*Memory::operator	new(size_t	s){

			return	Memories->alloc();
		}

		void	Memory::operator	delete(void	*b){
		}

		Memory::Memory():objectSize(0),firstBlock(NULL),lastBlock(NULL){
		}

		Memory::Memory(size_t	objectSize):objectSize(objectSize){

			firstBlock=lastBlock=new(objectSize,BLOCK_SIZE)	Block(objectSize,BLOCK_SIZE);
			freeObjects=BLOCK_SIZE;
		}

		Memory::~Memory(){

			if(firstBlock)
				delete	firstBlock;
		}

		inline	void	*Memory::alloc(){

			for(Block	*b=firstBlock;b;b=b->next()){

				void	*p=b->alloc();
				if(p){

					if(--freeObjects<BLOCK_SIZE-1){

						lastBlock=lastBlock->link(new(objectSize,BLOCK_SIZE)	Block(objectSize,BLOCK_SIZE));
						freeObjects+=BLOCK_SIZE;
					}
					return	p;
				}
			}

			lastBlock=lastBlock->link(new(objectSize,BLOCK_SIZE)	Block(objectSize,BLOCK_SIZE));
			freeObjects+=BLOCK_SIZE-1;
			return	lastBlock->alloc();
		}

		inline	void	Memory::dealloc(void	*o){

			Block	*_b=NULL;
			for(Block	*b=firstBlock;b;_b=b,b=b->next()){

				uint8	r=b->dealloc(o);
				if(r>0){
			
					if(++freeObjects>=BLOCK_SIZE_x_2	&&	r==2){

						if(_b)
							_b->link(b->next());
						else{

							firstBlock=b->next();
							if(b==lastBlock)
								lastBlock=_b;
						}
						delete	b;
					}
					return;
				}
			}
		}

		void	Memory::Cleanup(){

			//delete	Memories;
		}
	}
}