// payload.cpp
//
// Author: Eric Nivel
//
//

#include	<memory>
#include	"payload.h"
#include	"node.h"


namespace	mBrane{
	namespace	sdk{

		inline	_Payload::_Payload():_Object(){
		}

		inline	_Payload::~_Payload(){
		}

		inline	uint16	_Payload::cid()	const{
			
			return	_cid;
		}

		inline	uint8	_Payload::ptrCount()	const{

			return	0;
		}

		inline	_Payload	**_Payload::ptr(uint8	i){

			return	NULL;
		}

		inline	int64	&_Payload::send_ts(){

			return	_send_ts;
		}
				
		inline	int64	&_Payload::recv_ts(){

			return	_recv_ts;
		}

		////////////////////////////////////////////////////////////////////////////////////

		inline	_PP::_PP():__P(){
		}

		inline	_PP::~_PP(){
		}

		inline	_PP::operator	_Payload	*(){

			return	(_Payload	*)object;
		}

		inline	_PP	&_PP::operator	=(_Payload	*o){

			__P::operator	=(o);
			return	*this;
		}

		inline	_PP	&_PP::operator	=(_PP	&p){

			__P::operator	=(p);
			return	*this;
		}
	}
}