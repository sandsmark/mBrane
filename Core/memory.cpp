/*
* HUMANOBS - mBrane
*
* Eric Nivel
* Center for Analysis and Design of Intelligent Agents
*   Reykjavik University, Menntavegur 1, 101 Reykjavik, Iceland
*   http://cadia.ru.is
* Copyright(c)2012
*
* This software was developed by the above copyright holder as part of 
* the HUMANOBS EU research project, in collaboration with the 
* following parties:
* 
* Autonomous Systems Laboratory
*   Technical University of Madrid, Spain
*   http://www.aslab.org/
*
* Communicative Machines
*   Edinburgh, United Kingdom
*   http://www.cmlabs.com/
*
* Istituto Dalle Molle di Studi sull'Intelligenza Artificiale
*   University of Lugano and SUPSI, Switzerland
*   http://www.idsia.ch/
*
* Institute of Cognitive Sciences and Technologies
*   Consiglio Nazionale delle Ricerche, Italy
*   http://www.istc.cnr.it/
*
* Dipartimento di Ingegneria Informatica
*   University of Palermo, Italy
*   http://roboticslab.dinfo.unipa.it/index.php/Main/HomePage
*
*
* --- HUMANOBS Open-Source BSD License, with CADIA Clause v 1.0 ---
*
* Redistribution and use in source and binary forms, with or without 
* modification, is permitted provided that the following conditions 
* are met:
*
* - Redistributions of source code must retain the above copyright 
* and collaboration notice, this list of conditions and the 
* following disclaimer.
*
* - Redistributions in binary form must reproduce the above copyright 
* notice, this list of conditions and the following
* disclaimer in the documentation and/or other materials provided 
* with the distribution.
*
* - Neither the name of its copyright holders nor the names of its 
* contributors may be used to endorse or promote products 
* derived from this software without specific prior written permission.
*
* - CADIA Clause: The license granted in and to the software under this 
* agreement is a limited-use license. The software may not be used in 
* furtherance of: 
* (i) intentionally causing bodily injury or severe emotional distress 
* to any person; 
* (ii) invading the personal privacy or violating the human rights of 
* any person; or 
* (iii) committing or preparing for any act of war.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
* "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
* LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR 
* A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
* OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
* SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
* LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, 
* DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY 
* THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
* OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include	<memory>
#include	<cstring>
#include	"object.h"
#include	"memory.h"


#ifdef	MEMORY_1

#define	BLOCK_OBJECT_COUNT	2048	//	number of objects in a block

namespace	mBrane{
	namespace	sdk{

		inline	void	*Memory::Block::operator new(size_t	s,size_t	objectSize,uint16_t	objectCount){

			void	*a=malloc(s+objectCount*objectSize);	//	OPTIMIZATION: allocate from a private heap
			memset(a,0x0,objectCount*objectSize);
			return	a;
		}				

		inline	void	Memory::Block::operator	delete(void	*b){

			free(b);
		}

		inline	Memory::Block::Block(size_t	objectSize,uint16_t	objectCount):objectCount(objectCount),
																			objectSize(objectSize),
																			totalSize(objectCount*objectSize),
																			_next(NULL),
																			_prev(NULL),
																			freeObjects(objectCount){

			firstFree=((uint8_t	*)this)+sizeof(Block);
			uint8_t	*p=firstFree;
			uint8_t	*next_p;
			for(uint16_t	i=0;i<objectCount-1;i++){	//	initializes the pointers from each segment to the next free one; this loop can be eliminated (see Block::alloc): but some basic preliminary tests show that what we gain here is lost in the extra tests in Block::alloc

				next_p=p+objectSize;
				*(uint8_t	**)p=next_p;
				p=next_p;
			}
			*(uint8_t	**)p=NULL;
		}

		inline	Memory::Block::~Block(){

			if(_next)
				delete	_next;
		}

		inline	void	*Memory::Block::alloc(){	//	called only if freeObjects>0
			
			uint8_t	*segment=firstFree;
			if(--freeObjects){	//	make firstFree the next free segment if any
				/* alternative: allows removing the loop in Block::Block()
				if(!*(uint8_t	**)segment)	//	if no link to the next free block, jump to the next segment in the array; it is safe to do so since (a) allocations are made from the beginning of the block and (b) deallocations move the firstFree to the deallocated block
					firstFree=segment+objectSize;
				else*/
					firstFree=*(uint8_t	**)segment;
			}else
				firstFree=NULL;
			//memset(segment,0x0,objectSize);
			*(Block	**)segment=this;
			return	(void	*)(segment+sizeof(Block	*));
		}

		inline	void	Memory::Block::dealloc(void	*s){
			
			//	have s point to firstFree and make s the firstFree segment
			*(uint8_t	**)s=firstFree;
			firstFree=(uint8_t	*)s;
			++freeObjects;
		}

		////////////////////////////////////////////////////////////////////////

		Array<Memory,16>	*Memory::Memories=NULL;	//	indexed by exponents of 2

		CriticalSection		*Memory::CS=NULL;

		inline	size_t	Memory::GetNormalizedSize(size_t	s,uint8_t	&pow2){

			size_t	segmentSize=s+sizeof(Block	*);	//	makes room for a pointer to the block
			size_t	objectSize=64;
			pow2=0;	//	smallest pow2 such as (objectSize=2^pow2) >= s when objectSize>64
			if(segmentSize>64){

				pow2=BSR(segmentSize)-5;
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

		Memory	*Memory::GetStatic(size_t	s){	//	s>0; called early, once per class, at static member init time
			
			uint8_t	i;
			size_t	objectSize=GetNormalizedSize(s,i);

			if(!CS){	//	entered once early at class loading time; TODO: dealloc when the app terminates
		//		printf("New CS[%u]!\n", s);
		//		fflush(stdout);
				CS=new	CriticalSection();
				Memories=new	Array<Memory,16>();
			}
			Memory	*m=Memories->get(i);
		//	if (m==NULL) {
		//		printf("Use CS[%u/%u]!\n", s, objectSize);
		//		fflush(stdout);
		//	}
			//if ( (m == NULL) || (m->objectSize==0) )
			if (m->objectSize==0)
				m=new(i)	Memory(objectSize);
		//	printf("Done CS[%u]!\n", s);
			fflush(stdout);
			return	m;
		}

		Memory	*Memory::GetDynamic(size_t	s){	//	s>0; called anytime, for raw storage
			
			uint8_t	i;
			size_t	objectSize=GetNormalizedSize(s,i);

			CS->enter();	//	concurrent access for raw storage requests
			Memory	*m=Memories->get(i);
			if(m->objectSize==0)
				m=new(i)	Memory(objectSize);
			CS->leave();
			return	m;
		}

		inline	void	*Memory::operator	new(size_t	s,uint16_t	index){	//	already allocated in Memories

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

		inline	const	uint32_t	Memory::getObjectSize()	const{

			return	objectSize;
		}

		void	*Memory::alloc(uint32_t	&normalizedSize){

			normalizedSize=objectSize;
			return	alloc();
		}

		void	*Memory::alloc(){	//	concurrent access

			void	*allocated;
			Block	*b;
			cs.enter();
			if(freeObjects){	//	get the first block with a free segment

				for(b=firstBlock;!b->freeObjects;b=b->_next);	//	OPTIMIZATION: either reduce the number of blocks, or keep track of the blocks that still have some room - or both
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
			uint8_t	*segment=(uint8_t	*)(((uint8_t	**)o)-1);
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
#elif defined	MEMORY_2

#define	BLOCK_OBJECT_COUNT	16384	//	number of objects in a block
#define	STACK_SWAP_THR		1024	//	number of objects in a stack above which stacks will be swapped when dealloc stack is starved (otherwise a new block is allocated)
									//	must be <= BLOCK_OBJECT_COUNT

namespace	mBrane{
	namespace	sdk{

		inline	void	*Memory::Block::operator new(size_t	s,size_t	objectSize,uint16_t	objectCount){

			void	*a=malloc(s+objectCount*objectSize);	//	OPTIMIZATION: allocate from a private heap
			memset(a,0x0,objectCount*objectSize);
			return	a;
		}				

		inline	void	Memory::Block::operator	delete(void	*b){

			free(b);
		}

		inline	Memory::Block::Block(size_t	segmentSize,uint16_t	segmentCount):segmentCount(segmentCount),
																			segmentSize(segmentSize),
																			totalSize(segmentCount*segmentSize),
																			_next(NULL),
																			_prev(NULL){
			segments=((uint8_t	*)this)+sizeof(Block);
			uint32_t	index=0;
			for(uint32_t	i=0;i<segmentCount;++i,index+=segmentSize)
				*(Block	**)(segments+index)=this;

			freeSegments[0]=segmentCount;	//	0 is the id of the initial alloc stack
			freeSegments[1]=0;
		}

		inline	Memory::Block::~Block(){

			if(_next)
				delete	_next;
		}

		////////////////////////////////////////////////////////////////////////

		Array<Memory,16>	*Memory::Memories=NULL;	//	indexed by exponents of 2

		CriticalSection		*Memory::CS=NULL;

		inline	size_t	Memory::GetNormalizedSize(size_t	s,uint8_t	&pow2){

			size_t	segmentSize=s+sizeof(Block	*);	//	makes room for a pointer to the block
			size_t	objectSize=64;
			pow2=0;	//	smallest pow2 such as (objectSize=2^pow2) >= s when objectSize>64
			if(segmentSize>64){

				pow2=BSR(segmentSize)-5;
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

		Memory	*Memory::GetStatic(size_t	s){	//	s>0; called early, once per class, at static member init time
			
			uint8_t	i;
			size_t	objectSize=GetNormalizedSize(s,i);

			if(!CS){	//	entered once early at class loading time; TODO: dealloc when the app terminates

				CS=new	CriticalSection();
				Memories=new	Array<Memory,16>();
			}
			Memory	*m=Memories->get(i);
			if(m->objectSize==0)
				m=new(i)	Memory(objectSize);
			return	m;
		}

		inline	Memory	*Memory::GetDynamic(size_t	s){	//	s>0; called anytime, for raw storage
			
			uint8_t	i;
			size_t	objectSize=GetNormalizedSize(s,i);

			CS->enter();	//	concurrent access for raw storage requests
			Memory	*m=Memories->get(i);
			if(m->objectSize==0)
				m=new(i)	Memory(objectSize);
			CS->leave();
			return	m;
		}

		inline	void	*Memory::operator	new(size_t	s,uint16_t	index){	//	already allocated in Memories

			return	&Memories->operator [](index);
		}

		inline	void	Memory::operator	delete(void	*b){	//	will be deallocated when Memories is
		}

		Memory::Memory():objectSize(0){	//	uninitialized (object size == 0); called at Array<Memory,16> construction time
		}

		inline	Memory::Memory(size_t	objectSize):objectSize(objectSize){	//	initialized; explicit call in Memory::GetXXX(size_t	s)

			firstBlock=lastBlock=new(objectSize,BLOCK_OBJECT_COUNT)	Block(objectSize,BLOCK_OBJECT_COUNT);

			stacks[0].stack=(uint8_t	**)malloc(BLOCK_OBJECT_COUNT*sizeof(uint8_t	*));	//	initial alloc stack
			stacks[0].size=BLOCK_OBJECT_COUNT;
			stacks[0].top=BLOCK_OBJECT_COUNT-1;
			stacks[0].id=0;

			stacks[1].stack=(uint8_t	**)malloc(BLOCK_OBJECT_COUNT*sizeof(uint8_t	*));	//	initial dealloc stack
			stacks[1].size=BLOCK_OBJECT_COUNT;
			stacks[1].top=-1;
			stacks[1].id=1;

			uint32_t	index=0;
			for(uint32_t	i=0;i<stacks[0].size;++i,index+=objectSize)	//	fill the alloc stack, leave the dealloc stack empty
				stacks[0].stack[i]=firstBlock->segments+index;

			alloc_stack=&stacks[0];
			dealloc_stack=&stacks[1];

			alloc_top=BLOCK_OBJECT_COUNT-1;
			dealloc_top=-1;

			a_sem=new	Semaphore(0,1);
			d_sem=new	Semaphore(0,1);
			a_d_sem=new	Semaphore(0,1);
			d_a_sem=new	Semaphore(0,1);
			a_d_guard=0;
			d_a_guard=0;
		}

		inline	Memory::~Memory(){

			if(objectSize){	//	this was initialized by Memory::Get(size_t	s)

				delete	firstBlock;
				free(alloc_stack);
				free(dealloc_stack);
				delete		a_sem;
				delete		d_sem;
				delete		a_d_sem;
				delete		d_a_sem;
			}
		}

		inline	const	uint32_t	Memory::getObjectSize()	const{

			return	objectSize;
		}

		void	*Memory::alloc(uint32_t	&normalizedSize){

			normalizedSize=objectSize;
			return	alloc();
		}

		void	*Memory::alloc(){	//	concurrent access

			void	*allocated_segment;
check_top:	int32_t	top=Atomic::Decrement32(&alloc_top);	//	after this call, alloc_top may be changed by other alloc threads

			if(top<-2){	//	the alloc stack is being refilled by alloc(), alloc_top will change or alloc_stack will change: wait until it's done and check again

				a_sem->acquire();
				a_sem->release();	//	unlock the next waiting alloc thread
				goto	check_top;
			}

			if(d_a_guard==-1)	//	a dealloc thread is using the alloc stack
				d_a_sem->acquire();	//	wait until it's done

			Atomic::Swap32(&d_a_guard,1);	//	signals the current thread is using the alloc stack; any dealloc thread attempting to use the alloc stack will wait on d_a_sem

			if(top>-2)	//	the alloc stack was not empty
				allocated_segment=allocSegment(top+1);
			else{	//	top==-2: the stack was empty
//std::cout<<"alloc starving\n";fflush(stdout);
				alloc_stack->top=-1;

				dealloc_stack->top=dealloc_top;
				if(dealloc_stack->top>=STACK_SWAP_THR){	//	the dealloc stack has segments enough

					int32_t	former_guard=Atomic::Swap32(&a_d_guard,-1);	//	signals the current thread is using the dealloc stack; any dealloc thread attempting to use the dealloc stack will wait on a_d_sem
					if(former_guard==1)
						a_d_sem->acquire();

					//	at this point the alloc stack is empty: if a block is empty, all its free segments are in the dealloc stack
					//	this is the best time to deallocate an empty block, if any, considering the following cases:
					//	a - the dealloc stack contains less than BLOCK_OBJECT_COUNT: there cannot be any free block
					//	b - the dealloc stack contains less than 2*BLOCK_OBJECT_COUNT: it's not a good idea to deallocate anything, as this might leave only a few segments in the dealloc stack,
					//	which in turn increases the probability of having to allocate a new block when the alloc stack is starved
					//	c - the dealloc stack contains more than 2*BLOCK_OBJECT_COUNT: find the first empty block and deallocate it
					if(dealloc_stack->top>=2*BLOCK_OBJECT_COUNT)
						deallocBlock();
//std::cout<<"alloc swapping stacks\n";fflush(stdout);
					swapStackPointers();
					dealloc_top=dealloc_stack->top;
					
					allocated_segment=allocSegment(alloc_stack->top);

					former_guard=Atomic::Swap32(&a_d_guard,0);	//	signals no alloc thread uses the dealloc stack
					if(former_guard==1)
						a_d_sem->releaseAll();	//	unlock the dealloc threads waiting to use the dealloc stack
				}else{	//	the dealloc stack is not filled enough: alloc a new block and refill alloc stack
//std::cout<<"new block\n";fflush(stdout);
					Block	*b=new(objectSize,BLOCK_OBJECT_COUNT)	Block(objectSize,BLOCK_OBJECT_COUNT);
					lastBlock->_next=b;
					b->_prev=lastBlock;
					lastBlock=b;
					b->freeSegments[alloc_stack->id]=BLOCK_OBJECT_COUNT-1;

					uint32_t	index=0;
					for(uint32_t	i=0;i<BLOCK_OBJECT_COUNT;i++,index+=objectSize)
						alloc_stack->stack[i]=b->segments+index;

					alloc_stack->top=BLOCK_OBJECT_COUNT-1;

					allocated_segment=allocSegment(alloc_stack->top);
				}

				alloc_top=--alloc_stack->top;
				
				a_sem->release();	//	unlock alloc threads
				a_sem->acquire();	//	make sure the count falls back to 0
			}

			int32_t	former_guard=Atomic::Swap32(&d_a_guard,0);	//	signals no alloc thread uses the alloc stack
			if(former_guard==-1)
				d_a_sem->release();	//	unlock the dealloc thread waiting to use the alloc stack

			return	allocated_segment;
		}

		void	Memory::dealloc(void	*o){	//	concurrent access

check_top:	int32_t	top=Atomic::Increment32(&dealloc_top);	//	after this call, dealloc_top may be changed by other dealloc threads

			if(top>dealloc_stack->size){	//	the dealloc stack is being increased by dealloc(), dealloc_top will change: wait until it's done and check again

				d_sem->acquire();
				d_sem->release();	//	unlock the next waiting dealloc thread
				goto	check_top;
			}
			
			if(a_d_guard==-1)	//	an alloc thread is using the dealloc stack
				a_d_sem->acquire();	//	wait until it's done

			Atomic::Swap32(&a_d_guard,1);	//	signals the current thread is using the dealloc stack; any alloc thread attempting to use the dealloc stack will wait on a_d_sem

			if(top<dealloc_stack->size)	//	the dealloc stack was not full
				deallocSegment(o,top);
			else{	//	top==dealloc_stack->size: the dealloc stack was full; swap pointers if it's worth it, dealloc an empty block if any: if everything fails, increase dealloc stack
//std::cout<<"dealloc full\n";fflush(stdout);

				dealloc_stack->top=dealloc_top;

				alloc_stack->top=alloc_top;
				if(alloc_stack->top<=STACK_SWAP_THR){	//	the alloc stack is empty enough

					int32_t	former_guard=Atomic::Swap32(&d_a_guard,-1);	//	signals the current thread is using the alloc stack; any alloc thread attempting to use the alloc stack will wait on d_a_sem
					if(former_guard==1)
						d_a_sem->acquire();
//std::cout<<"dealloc swapping stacks\n";fflush(stdout);
					swapStackPointers();
					alloc_top=alloc_stack->top;

					former_guard=Atomic::Swap32(&d_a_guard,0);	//	signals no alloc thread uses the alloc stack
					if(former_guard==-1)
						d_a_sem->releaseAll();	//	unlock the alloc threads waiting to use the alloc stack
				}else{

					//	this is not the best time to deallocate an empty block, if any: some its free segments might be in the alloc stack
					//	however, deallocating blocks cannot be left to alloc() alone: if there is no allocations for some time, no block would be freed
					//	as for alloc(), the following cases apply:
					//	a - the dealloc stack contains less than BLOCK_OBJECT_COUNT: there cannot be any free block with all its free segments in dealloc stack
					//	b - the dealloc stack contains less than 2*BLOCK_OBJECT_COUNT: it's not a good idea to deallocate anything, as this might leave only a few segments in the dealloc stack,
					//	which in turn increases the probability of having to allocate a new block when the alloc stack is starved
					//	c - the dealloc stack contains more than 2*BLOCK_OBJECT_COUNT: find the first empty block with all its free segments in the dealloc stack and deallocate it
					if(top>=2*BLOCK_OBJECT_COUNT	&&	!deallocBlock()){
//std::cout<<"dealloc growing\n";fflush(stdout);						
						dealloc_stack->size+=BLOCK_OBJECT_COUNT;
						dealloc_stack->stack=(uint8_t	**)realloc(dealloc_stack->stack,dealloc_stack->size*sizeof(uint8_t	*));
					}
				}

				dealloc_top=dealloc_stack->top;

				deallocSegment(o,top);

				d_sem->release();	//	unlock dealloc threads
				d_sem->acquire();	//	make sure the count falls back to 0
			}

			int32_t	former_guard=Atomic::Swap32(&a_d_guard,0);	//	signals no dealloc thread touches the dealloc stack
			if(former_guard==-1)
				a_d_sem->release();	//	unlock the alloc thread waiting to use the dealloc stack
		}

		inline	void	*Memory::allocSegment(int32_t	location)	const{

			uint8_t	*allocated_segment=alloc_stack->stack[location];
			Block	*b=*(Block	**)allocated_segment;
			Atomic::Decrement32(&b->freeSegments[alloc_stack->id]);
			return	(void	*)(allocated_segment+sizeof(Block	*));
		}

		inline	void	Memory::deallocSegment(void	*o,int32_t	location)	const{

			uint8_t	*deallocated_segment=(((uint8_t	*)o)-sizeof(Block	*));
			Block	*b=*(Block	**)deallocated_segment;
			Atomic::Increment32(&b->freeSegments[dealloc_stack->id]);
			dealloc_stack->stack[location]=deallocated_segment;
		}

		inline	bool	Memory::deallocBlock(){

			Block	*b;
			for(b=firstBlock;b->freeSegments[dealloc_stack->id]<BLOCK_OBJECT_COUNT;b=b->_next);	//	find the first block having all its free segments in the dealloc stack

			if(b){
//std::cout<<"dealloc shrinking\n";fflush(stdout);
				uint8_t	**new_stack=(uint8_t	**)malloc((dealloc_stack->size-BLOCK_OBJECT_COUNT)*sizeof(uint8_t	*));	//	decrease the size of dealloc stack by BLOCK_OBJECT_COUNT
				uint32_t	j=0;
				for(uint32_t	i=0;i<=dealloc_stack->top;++i){	//	retain segments that do not belong to b

					if(*(Block	**)dealloc_stack->stack[i]!=b)
						new_stack[j++]=dealloc_stack->stack[i];
				}
				free(dealloc_stack->stack);
				dealloc_stack->stack=new_stack;
				dealloc_stack->size-=BLOCK_OBJECT_COUNT;
				dealloc_stack->top=j-1;

				//	delete block
				if(b->_prev)
					b->_prev->_next=b->_next;
				else	//	b was first
					firstBlock=b->_next;
				if(b->_next)
					b->_next->_prev=b->_prev;
				else	//	b was last
					lastBlock=b->_prev;
				b->_next=NULL;	//	to prevent deletion cascade in ~Block()
				delete	b;
				return	true;
			}
			return	false;
		}

		inline	void	Memory::swapStackPointers(){

			Stack	*tmp=dealloc_stack;
			dealloc_stack=alloc_stack;
			alloc_stack=tmp;
		}
	}
}
#endif
