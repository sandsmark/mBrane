// message.h
//
// Author: Eric Nivel
//
//

#ifndef	mBrane_sdk_message_h
#define	mBrane_sdk_message_h

#include	"object.h"
#include	"memory.h"


namespace	mBrane{
	namespace	sdk{

		template<class	U>	class	Message:
		public	Payload<Memory,U>{
		protected:
			uint32	_mid;	//	content identifer
			uint8	_priority();
		public:
			Message();
			virtual	~Message();
			uint32	&mid();
			uint8	&priority();
		};
	}
}

#include	"message.tpl.cpp"


#endif
