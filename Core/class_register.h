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

		class	ClassRegister{
		private:
			class	Array{
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
			Allocator	*allocator;
			ClassRegister();
			~ClassRegister();
		public:
			template<class	C,class	M>	static	uint16	Load(){

				uint16	CID;
				ClassRegister	*r=Classes.alloc(CID);
				r->allocator=C::_Allocator=M::Get(sizeof(C));
				return	CID;
			}
			static	ClassRegister	*Get(uint16	CID);
			static	uint16	Count();
		};
	}
}


#endif