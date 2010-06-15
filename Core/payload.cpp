//	payload.cpp
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

#include	<memory>
#include	"payload.h"


namespace	mBrane{
	namespace	sdk{

		__Payload::__Payload(){
		}

		__Payload::~__Payload(){
		}

		void	__Payload::init(){
		}

		uint16	__Payload::ptrCount()	const{

			return	0;
		}

		__Payload	*__Payload::getPtr(uint16	i)	const{

			return	NULL;
		}

		void	__Payload::setPtr(uint16	i,__Payload	*p){
		}

		////////////////////////////////////////////////////////////////////////////////////

		_Payload::_Payload():__Payload(){
		}

		_Payload::~_Payload(){
		}

		_Payload::Category	_Payload::category()	const{

			return	(_Payload::Category)(_metaData	&	0x0000000000000003);
		}

		uint64	&_Payload::node_send_ts(){

			return	_node_send_ts;
		}

		uint64	&_Payload::node_recv_ts(){

			return	_node_recv_ts;
		}

		uint64	&_Payload::send_ts(){

			return	_send_ts;
		}
				
		uint64	&_Payload::recv_ts(){

			return	_recv_ts;
		}

		payloads::_Message	*_Payload::as_Message(){

			return	NULL;
		}

		payloads::_StreamData	*_Payload::as_StreamData(){

			return	NULL;
		}

		////////////////////////////////////////////////////////////////////////////////////

		_RPayload::_RPayload():__Payload(){
		}

		_RPayload::~_RPayload(){
		}
	}
}
