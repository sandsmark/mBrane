// payload.tpl.cpp
//
// Author: Eric Nivel
//
//

#include	"class_register.h"


namespace	mBrane{
	namespace	sdk{

		template<class	M,class	U>	const	uint16	Payload<M,U>::_CID=ClassRegister::Load<U,M>();

		template<class	M,class	U>	inline	const	uint16	Payload<M,U>::CID(){

			return	_CID;
		}

		template<class	M,class	U>	inline	Payload<M,U>::Payload():Object<M,_Payload,U>(){
		}

		template<class	M,class	U>	inline	Payload<M,U>::~Payload(){
		}

		template<class	M,class	U>	inline	void	*Payload<M,U>::operator	new(size_t	s){

			U	*p=(U	*)Object<M,_Payload,U>::operator	new(s);
			p->_cid=_CID;
			return	p;
		}
		
		template<class	M,class	U>	inline	void	Payload<M,U>::operator	delete(void	*o){

			Object<M,_Payload,U>::operator	delete(o);
		}

		////////////////////////////////////////////////////////////////////////////////////

		template<class	C,class	M,class	U>	inline	PayloadAdapter<C,M,U>::PayloadAdapter():Payload<M,U>(),C(){
		}

		template<class	C,class	M,class	U>	inline	PayloadAdapter<C,M,U>::~PayloadAdapter(){
		}

		////////////////////////////////////////////////////////////////////////////////////

		template<class	C>	_Payload	**PP<C>::objectAddr(){	
			
			return	(_Payload	**)&object;
		}
	}
}