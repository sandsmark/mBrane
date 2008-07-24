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
	}
}