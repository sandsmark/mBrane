//	payload.cpp
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

#include	<memory>
#include	"payload.h"


namespace	mBrane{
	namespace	sdk{

		const	size_t	_Payload::Offset(){

			return	sizeof(_Object)+sizeof(int64)*2;
		}

		inline	_Payload::_Payload():_Object(){
		}

		inline	_Payload::~_Payload(){
		}

		inline	uint16	_Payload::cid()	const{
			
			return	_metaData>>16;
		}

		inline	_Payload::Category	_Payload::category()	const{

			return	(_Payload::Category)((_metaData	&&	0x0000000C)>>2);
		}

		inline	AllocationScheme	_Payload::allocationSceme()	const{

			return	(AllocationScheme)(_metaData	&&	0x00000003);
		}

		inline	void	_Payload::init(){
		}

		inline	int64	&_Payload::node_send_ts(){

			return	_node_send_ts;
		}

		inline	int64	&_Payload::node_recv_ts(){

			return	_node_recv_ts;
		}

		inline	int64	&_Payload::send_ts(){

			return	_send_ts;
		}
				
		inline	int64	&_Payload::recv_ts(){

			return	_recv_ts;
		}

		inline	_Payload::operator	payloads::_DynamicData	*()	const{

			return	NULL;
		}

		inline	_Payload::operator	payloads::_CompressedData	*()	const{

			return	NULL;
		}

		inline	_Payload::operator	payloads::_Message	*()	const{

			return	NULL;
		}

		inline	_Payload::operator	payloads::_StreamData	*()	const{

			return	NULL;
		}

		////////////////////////////////////////////////////////////////////////////////////////////////

		const	size_t	_RPayload::Offset(){

			return	sizeof(_Object);
		}

		inline	_RPayload::_RPayload(AllocationScheme	a):_Object(){

			_metaData|=a;	//	CID set in Payload<M,U>::operator	new
		}

		inline	_RPayload::~_RPayload(){
		}

		inline	uint16	_RPayload::cid()	const{
			
			return	_metaData>>16;
		}

		inline	AllocationScheme	_RPayload::allocationSceme()	const{

			return	(AllocationScheme)(_metaData	&&	0x00000003);
		}

		inline	void	_RPayload::init(){
		}

		inline	_RPayload::operator	payloads::_DynamicData	*()	const{

			return	NULL;
		}

		inline	_RPayload::operator	payloads::_CompressedData	*()	const{

			return	NULL;
		}
	}
}