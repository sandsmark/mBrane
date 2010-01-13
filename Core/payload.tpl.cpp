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

		template<class	S,AllocationScheme	AS>	inline	const	AllocationScheme	PayloadAlloc<S,AS>::_AllocationScheme(){

			return	AS;
		}

		template<class	S,AllocationScheme	AS>	PayloadAlloc<S,AS>::PayloadAlloc():S(){
		}

		template<class	S,AllocationScheme	AS>	PayloadAlloc<S,AS>::~PayloadAlloc(){
		}

		////////////////////////////////////////////////////////////////////////////////////

		template<class	S>	_DynamicData	*DynamicData<S>::as_DynamicData(){

			return	(_DynamicData	*)(((uint8	*)this)+sizeof(PayloadAlloc<S,DYNAMIC>));
		}

		////////////////////////////////////////////////////////////////////////////////////

		template<class	S>	_CompressedData	*CompressedData<S>::as_CompressedData(){

			return	(_CompressedData	*)(((uint8	*)this)+sizeof(PayloadAlloc<S,COMPRESSED>));
		}

		////////////////////////////////////////////////////////////////////////////////////

		template<class	S>	_RawStorage	*RawStorage<S>::as_RawStorage(){

			return	(_RawStorage	*)(((uint8	*)this)+sizeof(PayloadAlloc<S,RAW>));
		}

		////////////////////////////////////////////////////////////////////////////////////

		template<template<class>	class	A,	class	P,class	U,class	M>	uint64	___Payload<A,P,U,M>::_MetaData=ClassRegister::Load<U,M>();

		template<template<class>	class	A,	class	P,class	U,class	M>	inline	void	*___Payload<A,P,U,M>::New(uint32	size){

			return	new	U();
		}

		template<template<class>	class	A,	class	P,class	U,class	M>	inline	uint16	___Payload<A,P,U,M>::CID(){

			return	(uint16)(_MetaData>>16);
		}

		template<template<class>	class	A,	class	P,class	U,class	M>	inline	void	*___Payload<A,P,U,M>::operator	new(size_t	s){

			return	Object<M,A<P>,U>::operator	new(s);
		}
		
		template<template<class>	class	A,	class	P,class	U,class	M>	inline	void	___Payload<A,P,U,M>::operator	delete(void	*o){

			Object<M,A<P>,U>::operator	delete(o);
		}

		template<template<class>	class	A,	class	P,class	U,class	M>	inline	size_t	___Payload<A,P,U,M>::Offset(){

			return	offsetof(U,_metaData);
		}

		template<template<class>	class	A,	class	P,class	U,class	M>	inline	___Payload<A,P,U,M>::___Payload():Object<M,A<P>,U>(){

			this->_metaData = ___Payload<A,P,U,M>::_MetaData;
		}

		template<template<class>	class	A,	class	P,class	U,class	M>	inline	___Payload<A,P,U,M>::~___Payload(){
		}

		template<template<class>	class	A,	class	P,class	U,class	M>	uint16	___Payload<A,P,U,M>::cid()	const{
			
			return	(uint16)(this->_metaData>>16);
		}

		template<template<class>	class	A,	class	P,class	U,class	M>	inline	AllocationScheme	___Payload<A,P,U,M>::allocationScheme()	const{

			return	(AllocationScheme)(this->_metaData	&	0x0000000000000003);
		}

		////////////////////////////////////////////////////////////////////////////////////

		template<class	U,template<class>	class	A,class	M>	inline	Payload<U,A,M>::Payload():___Payload<A,_Payload,U,M>(){
		}

		template<class	U,template<class>	class	A,class	M>	inline	Payload<U,A,M>::~Payload(){
		}

		////////////////////////////////////////////////////////////////////////////////////

		template<class	U,template<class>	class	A,class	M>	inline	RPayload<U,A,M>::RPayload():___Payload<A,_RPayload,U,M>(){
		}

		template<class	U,template<class>	class	A,class	M>	inline	RPayload<U,A,M>::~RPayload(){
		}

		////////////////////////////////////////////////////////////////////////////////////

		template<class	C,class	U,template<class>	class	A,class	M>	inline	PayloadAdapter<C,U,A,M>::PayloadAdapter():Payload<U,A,M>(),C(){
		}

		template<class	C,class	U,template<class>	class	A,class	M>	inline	PayloadAdapter<C,U,A,M>::~PayloadAdapter(){
		}

		////////////////////////////////////////////////////////////////////////////////////

		template<class	C,class	U,template<class>	class	A,class	M>	inline	RPayloadAdapter<C,U,A,M>::RPayloadAdapter():RPayload<U,A,M>(),C(){
		}

		template<class	C,class	U,template<class>	class	A,class	M>	inline	RPayloadAdapter<C,U,A,M>::~RPayloadAdapter(){
		}
	}
}
