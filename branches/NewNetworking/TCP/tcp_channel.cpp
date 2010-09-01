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

TCPChannel::TCPChannel(core::socket	s):ConnectedCommChannel(),s(s){
	bufferLen = 0;
	buffer = NULL;
	bufferPos = 0;
	initialiseBuffer(4096);

	struct linger ling = {1, 0};
	setsockopt(s, SOL_SOCKET, SO_LINGER, (char *)&ling, sizeof(ling));

	char delay = 1;
	setsockopt(s, IPPROTO_TCP, TCP_NODELAY, (char*) &delay, sizeof(delay));

	char buffsize = 1;
	setsockopt(s, SOL_SOCKET, SO_SNDBUF, &buffsize, sizeof(buffsize));

	setBlockingMode(false);
}

bool TCPChannel::setBlockingMode(bool blocking) {
	#if defined(WINDOWS)
		// Set non-blocking mode
		unsigned long parm = 1; // 1 = Non-blocking, 0 = Blocking
		if (blocking) parm = 0;
		ioctlsocket(s, FIONBIO, &parm);
	#else
		// Set non-blocking mode
		long parm = fcntl(s, F_GETFL);
		if (blocking)
			parm &= ~O_NONBLOCK;
		else
			parm |= O_NONBLOCK;
		fcntl(s, F_SETFL, parm);
	#endif
	return true;
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

	setBlockingMode(true);
//	uint64 t1 = Time::Get();
//	PrintBinary(b, s, true, "TCP Sending");
	if(::send(this->s,(char	*)b,(int)s,0)==SOCKET_ERROR) {
//		int err = WSAGetLastError();
		return	1;
	}
//	printf("TCP Sent %u bytes...\n", (uint32)s);
	WaitForSocketWriteability(this->s, 1000);
	setBlockingMode(false);
//	printf("TCP Send %u bytes time: %u [%llu]...\n", s, (uint32) (Time::Get() - t1), Time::Get());
	return	0;
}

int16	TCPChannel::recv(uint8	*b,size_t	s,bool	peek){

	int32 wsaError;
	tcpCS.enter();

	while (s > bufferLen) {
		tcpCS.leave();
		initialiseBuffer((uint32)s * 2);
		tcpCS.enter();
	}

//	uint64 t = Time::Get();
	uint32 r = 0;
	uint32 tw = 0;
	uint32 tc = 0;
	// Do we have enough data in the buffer already
	while (bufferPos < s) {
		// read from the socket
		int count = ::recv(this->s,buffer+bufferPos,bufferLen-bufferPos,0);
		#ifndef WINDOWS
		//	setsockopt(this->s, IPPROTO_TCP, TCP_QUICKACK, (int[]){0}, sizeof(int));
		#endif
		//int count = ::recv(this->s,buffer,s,0);
		if (count==SOCKET_ERROR) {
			wsaError = Error::GetLastOSErrorNumber();
			if ((wsaError == SOCKETWOULDBLOCK) || (wsaError == EAGAIN)) {
//				uint64 t2 = Time::Get();
				WaitForSocketReadability(this->s, 1000);
//				tw += (uint32)(Time::Get() - t2);
//				tc++;
			}
			else {
				Error::PrintLastOSErrorMessage("Error: TCPChannel::recv");
				tcpCS.leave();
				return	1;
			}
		}
		else if (count == 0) {
			WaitForSocketReadability(this->s, 10);
		}
		else {
			bufferPos += count;
//			r += count;
		}
//		Error::PrintBinary(buffer+bufferPos, count, true, "TCP Received");
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
		bufferPos -= (uint32)s;
	}
	tcpCS.leave();
//	std::cout<<"Info: Read "<<s<<" bytes from buffer, "<<bufferLen-bufferPos<<" bytes left..."<<std::endl;

//	if (r)
//		printf("Took %u us to read %u bytes into buffer (%ux = %u) [%llu]...\n", Time::Get()-t, r, tc, tw, Time::Get());
//	else
//		printf("   Got %u bytes from buffer [%llu]...\n", s, Time::Get());

	return 0;

}

