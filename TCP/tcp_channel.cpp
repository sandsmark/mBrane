//	tcp_channel.cpp
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

#include	"tcp_channel.h"


using	namespace	mBrane;
using	namespace	mBrane::sdk;

TCPChannel::TCPChannel(mBrane::socket	s):ConnectedCommChannel(),s(s){
	bufferLen = 0;
	buffer = NULL;
	bufferContentLen = 0;
	bufferContentPos = 0;
	initialiseBuffer(4096);
}

TCPChannel::~TCPChannel(){
	tcpCS.enter();
	if (buffer != NULL)
		delete(buffer);
	bufferLen = 0;
	bufferContentLen = 0;
	bufferContentPos = 0;
	buffer = NULL;
	tcpCS.leave();
	closesocket(s);
}

bool	TCPChannel::initialiseBuffer(uint32 len) {
	if (len < 128) return false;
	tcpCS.enter();
	if (buffer != NULL)
		delete(buffer);
	bufferLen = len;
	buffer = new char[len];
	bufferContentLen = 0;
	bufferContentPos = 0;
	tcpCS.leave();
	return true;
}

int16	TCPChannel::send(uint8	*b,size_t	s){

//	TCPPrintBinary(b, s, true, "TCP Sending");
	if(::send(this->s,(char	*)b,(int)s,0)==SOCKET_ERROR) {
//		int err = WSAGetLastError();
		return	1;
	}

	return	0;
}

int16	TCPChannel::recv(uint8	*b,size_t	s,bool	peek){

	tcpCS.enter();

	if (s > bufferLen) {
		tcpCS.leave();
		initialiseBuffer(s * 2);
		tcpCS.enter();
	}

	// Do we have enough data in the buffer already
	if (bufferContentLen - bufferContentPos < s) {
		// if not, discard what we have in the buffer now
		bufferContentLen = bufferContentPos = 0;
		// and read from the socket
		int count = ::recv(this->s,buffer,bufferLen,0);
		if(count==SOCKET_ERROR) {
			std::cout<<"Error: TCPChannel::recv "<< getLastOSErrorNumber()<<std::endl;
			tcpCS.leave();
			return	1;
		}
		bufferContentLen = count;
//		TCPPrintBinary(buffer, bufferContentLen, true, "TCP Received");
		// std::cout<<"Info: Not enough data in buffer, received "<<count<<" bytes from socket..."<<std::endl;
	}

	// Do we now have enough data in the buffer
	if (bufferContentLen - bufferContentPos < s) {
		// if not, we give up
		tcpCS.leave();
		std::cout<<"Error: Not enough data in buffer, have "<<bufferContentLen-bufferContentPos<<" bytes, need "<<s<<" bytes..."<<std::endl;
		return	1;
	}

	// if yes, great
	memcpy(b, buffer+bufferContentPos, s);
	if (!peek)
		bufferContentPos += s;
	tcpCS.leave();
//	std::cout<<"Info: Read "<<s<<" bytes from buffer, "<<bufferContentLen-bufferContentPos<<" bytes left..."<<std::endl;
	return 0;

}

int32 TCPChannel::getLastOSErrorNumber() {
	#ifdef WINDOWS
		int32 err = WSAGetLastError();
		WSASetLastError(0);
		return err;
	#else
		return (int32) errno;
	#endif
}

void TCPPrintBinary(void* p, uint32 size, bool asInt, const char* title) {
	if (title != NULL)
		printf("--- %s %u ---\n", title, size);
	unsigned char c;
	for (uint32 n=0; n<size; n++) {
		c = *(((unsigned char*)p)+n);
		if (asInt)
			printf("[%u] ", (unsigned int)c);
		else
			printf("[%c] ", c);
		if ( (n > 0) && ((n+1)%10 == 0) )
			printf("\n");
	}
	printf("\n");
}
