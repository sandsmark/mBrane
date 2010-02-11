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

#if defined (WINDOWS)
#include	<iphlpapi.h>
#endif

#if defined (LINUX)
#include	<unistd.h>
#include	<fcntl.h>
#include	<netinet/tcp.h>
#include	<errno.h>
#include	<sys/ioctl.h>
#include	<net/if.h>
#endif

using	namespace	mBrane;
using	namespace	mBrane::sdk;

TCPChannel::TCPChannel(mBrane::socket	s):ConnectedCommChannel(),s(s){
	bufferLen = 0;
	buffer = NULL;
	bufferPos = 0;
	initialiseBuffer(4096);

	struct linger ling = {1, 0};
	setsockopt(s, SOL_SOCKET, SO_LINGER, (char *)&ling, sizeof(ling));

	char delay = 1;
	setsockopt(s, IPPROTO_TCP, TCP_NODELAY, (char*) &delay, sizeof(delay));

	#if defined(WINDOWS)
		// Set non-blocking mode
		unsigned long parm = 1; // 1 = Non-blocking, 0 = Blocking
		ioctlsocket(s, FIONBIO, &parm);
		// Set no linger
	#else
		// Set non-blocking mode
		long parm = fcntl(s, F_GETFL);
		parm |= O_NONBLOCK;
		//parm &= (!O_NONBLOCK);
		fcntl(s, F_SETFL, parm);
	#endif
}

TCPChannel::~TCPChannel(){
	tcpCS.enter();
	if (buffer != NULL)
		delete(buffer);
	bufferLen = 0;
	bufferPos = 0;
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
	bufferPos = 0;
	tcpCS.leave();
	return true;
}

int16	TCPChannel::send(uint8	*b,size_t	s){

//	uint64 t1 = Time::Get();
//	PrintBinary(b, s, true, "TCP Sending");
	if(::send(this->s,(char	*)b,(int)s,0)==SOCKET_ERROR) {
//		int err = WSAGetLastError();
		return	1;
	}
//	printf("TCP Send time: %u\n", (uint32) (Time::Get() - t1));
	return	0;
}

int16	TCPChannel::recv(uint8	*b,size_t	s,bool	peek){

	int32 wsaError;
	tcpCS.enter();

	while (s > bufferLen) {
		tcpCS.leave();
		initialiseBuffer(s * 2);
		tcpCS.enter();
	}

	// Do we have enough data in the buffer already
	while (bufferPos < s) {
		// read from the socket
		int count = ::recv(this->s,buffer+bufferPos,bufferLen-bufferPos,0);
		//int count = ::recv(this->s,buffer,s,0);
		if (count==SOCKET_ERROR) {
			wsaError = getLastOSErrorNumber();
			if ((wsaError == SOCKETWOULDBLOCK) || (wsaError == EAGAIN))
				WaitForSocketReadability(this->s, 10);
			else {
				printLastOSErrorMessage("Error: TCPChannel::recv");
				tcpCS.leave();
				return	1;
			}
		}
		else if (count == 0) {
			WaitForSocketReadability(this->s, 10);
		}
		else
			bufferPos += count;
//		PrintBinary(buffer, bufferContentLen, true, "TCP Received");
		// std::cout<<"Info: Not enough data in buffer, received "<<count<<" bytes from socket..."<<std::endl;
	}

	//// Do we now have enough data in the buffer
	//if (bufferContentLen - bufferContentPos < s) {
	//	// if not, we give up
	//	tcpCS.leave();
	//	std::cout<<"TCP Error: Not enough data in buffer, have "<<bufferContentLen-bufferContentPos<<" bytes, need "<<s<<" bytes..."<<std::endl;
	//	return	1;
	//}

	// if yes, great
	memcpy(b, buffer, s);
	if (!peek) {
		memcpy(buffer, buffer+s, bufferLen - s);
		bufferPos -= s;
	}
	tcpCS.leave();
//	std::cout<<"Info: Read "<<s<<" bytes from buffer, "<<bufferContentLen-bufferContentPos<<" bytes left..."<<std::endl;
	return 0;

}

