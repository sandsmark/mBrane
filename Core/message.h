// message.h
//
// Author: Eric Nivel
//
//

#ifndef	mBrane_sdk_message_h
#define	mBrane_sdk_message_h

#include	"payload.h"
#include	"memory.h"


namespace	mBrane{
	namespace	sdk{

		class	dll	_Message{
		protected:
			uint32	_mid;	//	content identifer
			uint8	_priority;
			_Message();
			virtual	~_Message();
		public:
			operator	_Payload	*()	const;
			uint32	&mid();
			uint8	&priority();
		};

		template<class	U>	class	Message:
		public	Payload<Memory,U>,
		public	_Message{
		public:
			Message();
			virtual	~Message();
		};
	}
}

#include	"message.tpl.cpp"


#endif
