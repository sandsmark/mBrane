//	payload.tpl.cpp
//
//	Author: Eric Nivel
//
//	BSD license:
//	Copyright (c) 2008, Eric Nivel, Thor List
//	All rights reserved.
//	Redistribution and use in source and binary forms, with or without
//	modification, are permitted provided that the following conditions are met:
//
//   - Redistributions of source code must retain the above copyright
//     notice, this list of conditions and the following disclaimer.
//   - Redistributions in binary form must reproduce the above copyright
//     notice, this list of conditions and the following disclaimer in the
//     documentation and/or other materials provided with the distribution.
//   - Neither the name of Eric Nivel, Thor List nor the
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

		template<class	M,class	U>	const	uint32	Payload<M,U>::_MetaData=ClassRegister::Load<U,M>();

		template<class	M,class	U>	inline	void	*Payload<M,U>::New(){	//	to initialize the _vftable

			return	new	U();
		}

		template<class	M,class	U>	inline	const	uint16	Payload<M,U>::CID(){

			return	_MetaData>>16;
		}

		template<class	M,class	U>	const	AllocationScheme	Payload<M,U>::_AllocationScheme(){

			return	STATIC;
		}

		template<class	M,class	U>	const	uint8	Payload<M,U>::PtrCount(){

			return	0;
		}

		template<class	M,class	U>	const	P<_RPayload>	*Payload<M,U>::Ptr(_RPayload	*p,uint8	i){

			return	NULL;
		}

		template<class	M,class	U>	inline	const	size_t	Payload<M,U>::Size(){

			return	sizeof(U);
		}

		template<class	M,class	U>	inline	const	size_t	Payload<M,U>::CoreSize(){

			return	sizeof(U);
		}

		template<class	M,class	U>	inline	Payload<M,U>::Payload():Object<M,_Payload,U>(){
		}

		template<class	M,class	U>	inline	void	*Payload<M,U>::operator	new(size_t	s){

			U	*p=(U	*)Object<M,_Payload,U>::operator	new(s);
			p->_metaData=_MetaData;
			return	p;
		}
		
		template<class	M,class	U>	inline	void	Payload<M,U>::operator	delete(void	*o){

			Object<M,_Payload,U>::operator	delete(o);
		}

		////////////////////////////////////////////////////////////////////////////////////

		template<class	C,class	M,class	U>	inline	PayloadAdapter<C,M,U>::PayloadAdapter():Payload<M,U>(),C(){
		}

		////////////////////////////////////////////////////////////////////////////////////

		template<class	M,class	U>	const	uint16	RPayload<M,U>::_CID=ClassRegister::Load<U,M>();

		template<class	M,class	U>	inline	void	*RPayload<M,U>::New(){	//	to initialize the _vftable

			return	new	U();
		}

		template<class	M,class	U>	inline	const	uint16	RPayload<M,U>::CID(){

			return	_CID;
		}

		template<class	M,class	U>	const	AllocationScheme	RPayload<M,U>::_AllocationScheme(){

			return	STATIC;
		}

		template<class	M,class	U>	const	uint8	RPayload<M,U>::PtrCount(){

			return	0;
		}

		template<class	M,class	U>	const	P<_RPayload>	*RPayload<M,U>::Ptr(_RPayload	*p,uint8	i){

			return	NULL;
		}
		
		template<class	M,class	U>	inline	const	size_t	RPayload<M,U>::Size(){

			return	sizeof(U);
		}

		template<class	M,class	U>	inline	const	size_t	RPayload<M,U>::CoreSize(){

			return	sizeof(U);
		}

		template<class	M,class	U>	inline	RPayload<M,U>::RPayload():Object<M,_RPayload,U>(){
		}

		template<class	M,class	U>	inline	void	*RPayload<M,U>::operator	new(size_t	s){

			U	*p=(U	*)Object<M,_RPayload,U>::operator	new(s);
			p->_cid=_CID;
			return	p;
		}
		
		template<class	M,class	U>	inline	void	RPayload<M,U>::operator	delete(void	*o){

			Object<M,_RPayload,U>::operator	delete(o);
		}

		////////////////////////////////////////////////////////////////////////////////////

		template<class	C,class	M,class	U>	inline	RPayloadAdapter<C,M,U>::RPayloadAdapter():RPayload<M,U>(),C(){
		}
	}
}