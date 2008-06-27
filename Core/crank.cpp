// crank.cpp
//
// Author: Eric Nivel
//
//

#include	"crank.h"


namespace	mBrane{
	namespace	sdk{

		_Crank::_Crank(uint16	_ID):_ID(_ID){
		}

		_Crank::~_Crank(){
		}

		uint16	_Crank::ID()	const{

			return	_ID;
		}
	}
}
