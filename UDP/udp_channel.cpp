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
	bcast_address.sin_port=htons((unsigned short)port);
	bcast_address.sin_addr.s_addr=INADDR_BROADCAST;
	bufferLen = 0;
	buffer = NULL;
	bufferContentLen = 0;
	bufferContentPos = 0;
	initialiseBuffer(4096);
}

UDPChannel::~UDPChannel(){
	udpCS.enter();
	if (buffer != NULL)
		delete(buffer);
	bufferLen = 0;
	bufferContentLen = 0;
	bufferContentPos = 0;
	buffer = NULL;
	udpCS.leave();
}

bool	UDPChannel::initialiseBuffer(uint32 len) {
	if (len < 128) return false;
	udpCS.enter();
	if (buffer != NULL)
		delete(buffer);
	bufferLen = len;
	buffer = new char[len];
	bufferContentLen = 0;
	bufferContentPos = 0;
	udpCS.leave();
	return true;
}

int16	UDPChannel::send(uint8	*b,size_t	s){

	if(::sendto(this->s,(char	*)b,(int)s,0,(SOCKADDR	*)&bcast_address,sizeof(sockaddr_in))==SOCKET_ERROR)
		return	1;

	return	0;
}

int16	UDPChannel::recv(uint8	*b,size_t	s,bool	peek){

	udpCS.enter();

	if (s > bufferLen) {
		udpCS.leave();
		initialiseBuffer(s * 2);
		udpCS.enter();
	}

	// Do we have enough data in the buffer already
	if (bufferContentLen - bufferContentPos < s) {
		// if not, discard what we have in the buffer now
		bufferContentLen = bufferContentPos = 0;
		// and read from the socket
		int count = ::recvfrom(this->s,buffer,bufferLen,0,NULL,0);
		if(count==SOCKET_ERROR) {
			printLastOSErrorMessage("Error: UDPChannel::recv");
			udpCS.leave();
			return	1;
		}
		bufferContentLen = count;
		// std::cout<<"Info: Not enough data in buffer, received "<<count<<" bytes from socket..."<<std::endl;
	}

	// Do we now have enough data in the buffer
	if (bufferContentLen - bufferContentPos < s) {
		// if not, we give up
		udpCS.leave();
		// std::cout<<"UDP Error: Not enough data in buffer, have "<<bufferContentLen-bufferContentPos<<" bytes, need "<<s<<" bytes..."<<std::endl;
		return	1;
	}

	// if yes, great
	memcpy(b, buffer+bufferContentPos, s);
	if (!peek)
		bufferContentPos += s;
	udpCS.leave();
//	std::cout<<"Info: Read "<<s<<" bytes from buffer, "<<bufferContentLen-bufferContentPos<<" bytes left..."<<std::endl;
	return 0;
}

