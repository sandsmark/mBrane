//	udp_channel.cpp
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

#include	"udp_channel.h"


using	namespace	mBrane;
using	namespace	mBrane::sdk;

UDPChannel::UDPChannel(mBrane::socket	s,uint32	port):BroadcastCommChannel(),s(s){

	bcast_address.sin_family=AF_INET;
	bcast_address.sin_port=port;
	bcast_address.sin_addr.s_addr=INADDR_BROADCAST;
}

UDPChannel::~UDPChannel(){
}

int16	UDPChannel::send(uint8	*b,size_t	s){

	if(::sendto(this->s,(char	*)b,(int)s,0,(SOCKADDR	*)&bcast_address,sizeof(sockaddr_in))==SOCKET_ERROR)
		return	1;

	return	0;
}

int16	UDPChannel::recv(uint8	*b,size_t	s,bool	peek){

	if(::recvfrom(this->s,(char	*)b,(int)s,peek?MSG_PEEK:0,NULL,0)==SOCKET_ERROR)
		return	1;

	return	0;
}