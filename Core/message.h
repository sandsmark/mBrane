// message.h
//
// Author: Eric Nivel
//
//

#ifndef	mBrane_sdk_message_h
#define	mBrane_sdk_message_h

#include	"object.h"


namespace	mBrane{
	namespace	sdk{

		template<class	U>	class	Message:
		public	Payload<Memory,U>{
		protected:
			uint32	_mid;
		public:
			Message();
			~Message();
			uint32	&mid();
		};
	}
}

#include	"message.tpl.cpp"


#endif
