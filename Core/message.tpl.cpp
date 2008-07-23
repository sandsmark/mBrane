// message.tpl.cpp
//
// Author: Eric Nivel
//
//

namespace	mBrane{
	namespace	sdk{

		template<class	U>	Message<U>::Message():Payload<Memory,U>(){
		}

		template<class	U>	Message<U>::~Message(){
		}

		template<class	U>	uint32	&Message<U>::mid(){

			return	_mid;
		}

		template<class	U>	uint8	&Message<U>::priority(){

			return	_priority;
		}
	}
}