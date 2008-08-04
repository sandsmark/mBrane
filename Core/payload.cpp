// payload.cpp
//
// Author: Eric Nivel
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
#include	"node.h"


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
			
			return	_cid;
		}

		inline	uint8	_Payload::ptrCount()	const{

			return	0;
		}

		inline	P<_Payload>	*_Payload::ptr(uint8	i){

			return	NULL;
		}

		inline	void	_Payload::init(){
		}

		inline	bool	_Payload::isCrankData()	const{

			return	false;
		}

		inline	bool	_Payload::isControlMessage()	const{

			return	false;
		}

		inline	bool	_Payload::isMessage()	const{

			return	false;
		}

		inline	bool	_Payload::isStreamData()	const{

			return	false;
		}

		inline	bool	_Payload::isCompressedPayload()	const{

			return	false;
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
	}
}