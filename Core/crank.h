// crank.h
//
// Author: Eric Nivel
//
//

#ifndef mBrane_sdk_crank_h
#define mBrane_sdk_crank_h

#include	"payload.h"


namespace	mBrane{
	namespace	sdk{

		class	_Crank{
		protected:
			uint16	_ID;
			_Crank(uint16	_ID);
		public:
			virtual	~_Crank();
			virtual	void	notify(_Payload	*p)=0;
			uint16	ID()	const;
		};
	}
}


#endif
