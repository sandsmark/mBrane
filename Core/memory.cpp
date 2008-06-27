// memory.cpp
//
// Author: Eric Nivel
//
//

#include	<memory>
#include	"object.h"
#include	"memory.h"


#define	BLOCK_SIZE		1024
#define	BLOCK_SIZE_x_2	2048

namespace	mBrane{
	namespace	sdk{

		Memory::Array::Array():_array(NULL),_count(0){
		}

		Memory::Array::~Array(){

			if(_array)
				delete[]	_array;
		}

		Memory	*Memory::Array::init(size_t s){

			for(uint32	i=0;i<_count;i++){

				if(_array[i].objectSize==s)
					return	_array+i;
			}

			new	Memory(s);
		}

		void	*Memory::Array::alloc(){

			if(_array)
				realloc(_array,(++_count)*sizeof(Memory));
			else
				_array=(Memory	*)malloc((++_count)*sizeof(Memory));

			return	_array+_count-1;
		}

		////////////////////////////////////////////////////////////////////////

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

			begin=(uint8	*)(((uint8	**)&end)+1);
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

		Memory::Array	Memory::Memories;

		Memory	*Memory::Get(size_t	s){

			return	Memories.init(s);
		}

		void	*Memory::operator	new(size_t	s){

			return	Memories.alloc();
		}

		void	Memory::operator	delete(void	*b){
		}

		Memory::Memory(size_t	objectSize):objectSize(objectSize){

			firstBlock=lastBlock=new(objectSize,BLOCK_SIZE)	Block(objectSize,BLOCK_SIZE);
			freeObjects=BLOCK_SIZE;
		}

		Memory::~Memory(){

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
	}
}