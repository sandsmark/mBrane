//	payload.tpl.cpp
//
//	Author: Eric Nivel
//
//	BSD license:
//	Copyright (c) 2008, Eric Nivel
//	All rights reserved.
//	Redistribution and use in source and binary forms, with or without
//	modification, are permitted provided that the following conditions are met:
//
//   - Redistributions of source code must retain the above copyright
//     notice, this list of conditions and the following disclaimer.
//   - Redistributions in binary form must reproduce the above copyright
//     notice, this list of conditions and the following disclaimer in the
//     documentation and/or other materials provided with the distribution.
//   - Neither the name of Eric Nivel nor the
//     names of their contributors may be used to endorse or promote products
//     derived from this software without specific prior written permission.
//
//	THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND ANY
//	EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
//	WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
//	DISCLAIMED. IN NO EVENT SHALL THE REGENTS AND CONTRIBUTORS BE LIABLE FOR ANY
//	DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
//	(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
//	LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
//	ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//	(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
//	SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include	"class_register.h"


namespace	mBrane{
	namespace	sdk{

		template<class	P,class	U,class	M>	uint64	___Payload<P,U,M>::_MetaData=ClassRegister::Load<U,M>();

		template<class	P,class	U,class	M>	inline	void	*___Payload<P,U,M>::New(uint32	size){

			return	new	U();
		}

		template<class	P,class	U,class	M>	inline	uint16	___Payload<P,U,M>::CID(){

			return	(uint16)(_MetaData>>16);
		}

		template<class	P,class	U,class	M>	inline	void	*___Payload<P,U,M>::operator	new(size_t	s){

			U	*p=(U	*)Object<M,P,U>::operator	new(s);
			p->_metaData=_MetaData;
			return	p;
		}
		
		template<class	P,class	U,class	M>	inline	void	___Payload<P,U,M>::operator	delete(void	*o){

			Object<M,P,U>::operator	delete(o);
		}

		template<class	P,class	U,class	M>	inline	size_t	___Payload<P,U,M>::Offset(){

			return	offsetof(U,_metaData);
		}

		template<class	P,class	U,class	M>	inline	___Payload<P,U,M>::___Payload():Object<M,P,U>(){

			this->_metaData = ___Payload<P,U,M>::_MetaData;
		}

		template<class	P,class	U,class	M>	inline	___Payload<P,U,M>::~___Payload(){
		}

		template<class	P,class	U,class	M>	uint16	___Payload<P,U,M>::cid()	const{
			
			return	(uint16)(this->_metaData>>16);
		}

		template<class	P,class	U,class	M>	size_t	___Payload<P,U,M>::size()	const{

			return	sizeof(U);
		}

		////////////////////////////////////////////////////////////////////////////////////

		template<class	U,class	M>	inline	Payload<U,M>::Payload():___Payload<_Payload,U,M>(){
		}

		template<class	U,class	M>	inline	Payload<U,M>::~Payload(){
		}

		////////////////////////////////////////////////////////////////////////////////////

		template<class	U,class	M>	inline	RPayload<U,M>::RPayload():___Payload<_RPayload,U,M>(){
		}

		template<class	U,class	M>	inline	RPayload<U,M>::~RPayload(){
		}

		////////////////////////////////////////////////////////////////////////////////////

		template<class	C,class	U,class	M>	inline	PayloadAdapter<C,U,M>::PayloadAdapter():Payload<U,M>(),C(){
		}

		template<class	C,class	U,class	M>	inline	PayloadAdapter<C,U,M>::~PayloadAdapter(){
		}

		////////////////////////////////////////////////////////////////////////////////////

		template<class	C,class	U,class	M>	inline	RPayloadAdapter<C,U,M>::RPayloadAdapter():RPayload<U,M>(),C(){
		}

		template<class	C,class	U,class	M>	inline	RPayloadAdapter<C,U,M>::~RPayloadAdapter(){
		}
	}
}
