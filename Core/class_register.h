// class_register.h
//
// Author: Eric Nivel
//
//

#ifndef mBrane_sdk_class_register_h
#define mBrane_sdk_class_register_h

#include	"object.h"
#include	"memory.h"


namespace	mBrane{
	namespace	sdk{

		class	dll	ClassRegister{
		private:
			class	dll	Array{
			private:
				ClassRegister	*_array;
				uint32			_count;
			public:
				Array();
				~Array();
				ClassRegister	*alloc(uint16	&CID);
				ClassRegister	*get(uint16	CID);
				uint16			count()	const;
			};
			static	Array	Classes;
			Allocator	*_allocator;
			size_t		_size;
			size_t		_offset;
			ClassRegister();
			~ClassRegister();
		public:
			template<class	C,class	M>	static	uint16	Load(){

				uint16	CID;
				ClassRegister	*r=Classes.alloc(CID);
				r->_allocator=C::_Allocator=M::Get(sizeof(C));
				r->_size=sizeof(C)-sizeof(_Object)-sizeof(int64);
				r->_offset=sizeof(_Object)+sizeof(int64);
				return	CID;
			}
			static	ClassRegister	*Get(uint16	CID);
			static	uint16	Count();
			Allocator	*allocator()	const;
			size_t		size()	const;
			size_t		offset()	const;
		};
	}
}


#endif