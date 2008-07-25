// message.tpl.cpp
//
// Author: Eric Nivel
//
//

namespace	mBrane{
	namespace	sdk{

		template<class	U>	inline	Message<U>::Message():Payload<Memory,U>(){
		}

		template<class	U>	inline	Message<U>::~Message(){
		}

		////////////////////////////////////////////////////////////////////////////////////////////////

		template<class	U>	inline	ControlMessage<U>::ControlMessage():Message<U>(){
		}

		template<class	U>	inline	ControlMessage<U>::~ControlMessage(){
		}
	}
}