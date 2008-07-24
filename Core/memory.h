// memory.h
//
// Author: Eric Nivel
//
//

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