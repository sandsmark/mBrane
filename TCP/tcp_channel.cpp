//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
//_/_/
//_/_/ HUMANOBS - mBrane - TCP
//_/_/
//_/_/ Eric Nivel
//_/_/ Center for Analysis and Design of Intelligent Agents
//_/_/   Reykjavik University, Menntavegur 1, 101 Reykjavik, Iceland
//_/_/   http://cadia.ru.is
//_/_/ Copyright(c)2012
//_/_/
//_/_/ This software was developed by the above copyright holder as part of 
//_/_/ the HUMANOBS EU research project, in collaboration with the 
//_/_/ following parties:
//_/_/ 
//_/_/ Autonomous Systems Laboratory
//_/_/   Technical University of Madrid, Spain
//_/_/   http://www.aslab.org/
//_/_/
//_/_/ Communicative Machines
//_/_/   Edinburgh, United Kingdom
//_/_/   http://www.cmlabs.com/
//_/_/
//_/_/ Istituto Dalle Molle di Studi sull'Intelligenza Artificiale
//_/_/   University of Lugano and SUPSI, Switzerland
//_/_/   http://www.idsia.ch/
//_/_/
//_/_/ Institute of Cognitive Sciences and Technologies
//_/_/   Consiglio Nazionale delle Ricerche, Italy
//_/_/   http://www.istc.cnr.it/
//_/_/
//_/_/ Dipartimento di Ingegneria Informatica
//_/_/   University of Palermo, Italy
//_/_/   http://roboticslab.dinfo.unipa.it/index.php/Main/HomePage
//_/_/
//_/_/
//_/_/ --- HUMANOBS Open-Source BSD License, with CADIA Clause v 1.0 ---
//_/_/
//_/_/ Redistribution and use in source and binary forms, with or without 
//_/_/ modification, is permitted provided that the following conditions 
//_/_/ are met:
//_/_/
//_/_/ - Redistributions of source code must retain the above copyright 
//_/_/ and collaboration notice, this list of conditions and the 
//_/_/ following disclaimer.
//_/_/
//_/_/ - Redistributions in binary form must reproduce the above copyright 
//_/_/ notice, this list of conditions and the following
//_/_/ disclaimer in the documentation and/or other materials provided 
//_/_/ with the distribution.
//_/_/
//_/_/ - Neither the name of its copyright holders nor the names of its 
//_/_/ contributors may be used to endorse or promote products 
//_/_/ derived from this software without specific prior written permission.
//_/_/
//_/_/ - CADIA Clause: The license granted in and to the software under this 
//_/_/ agreement is a limited-use license. The software may not be used in 
//_/_/ furtherance of: 
//_/_/ (i) intentionally causing bodily injury or severe emotional distress 
//_/_/ to any person; 
//_/_/ (ii) invading the personal privacy or violating the human rights of 
//_/_/ any person; or 
//_/_/ (iii) committing or preparing for any act of war.
//_/_/
//_/_/ THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
//_/_/ "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
//_/_/ LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR 
//_/_/ A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
//_/_/ OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
//_/_/ SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
//_/_/ LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, 
//_/_/ DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY 
//_/_/ THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
//_/_/ (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
//_/_/ OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//_/_/
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

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

bool	TCPChannel::initialiseBuffer(uint32_t len) {
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


int16_t	TCPChannel::send(uint8_t	*b,size_t	s){

	setBlockingMode(true);
//	uint64_t t1 = Time::Get();
//	PrintBinary(b, s, true, "TCP Sending");
	if(::send(this->s,(char	*)b,(int)s,0)==SOCKET_ERROR) {
//		int err = WSAGetLastError();
		return	1;
	}
//	printf("TCP Sent %u bytes...\n", (uint32_t)s);
	WaitForSocketWriteability(this->s, 1000);
	setBlockingMode(false);
//	printf("TCP Send %u bytes time: %u [%llu]...\n", s, (uint32_t) (Time::Get() - t1), Time::Get());
	return	0;
}

int16_t	TCPChannel::recv(uint8_t	*b,size_t	s,bool	peek){

	int32_t err;
	tcpCS.enter();

	while (s > bufferLen) {
		tcpCS.leave();
		initialiseBuffer((uint32_t)s * 2);
		tcpCS.enter();
	}

	uint64_t t = Time::Get();
	uint32_t r = 0;
	uint32_t tw = 0;
	uint32_t tc = 0;
	// Do we have enough data in the buffer already
	while (bufferPos < s) {
		// read from the socket
		int count = ::recv(this->s,buffer+bufferPos,bufferLen-bufferPos,0);
		#ifndef WINDOWS
		//	setsockopt(this->s, IPPROTO_TCP, TCP_QUICKACK, (int[]){0}, sizeof(int));
		#endif
		//int count = ::recv(this->s,buffer,s,0);
		if (count==SOCKET_ERROR) {
			err = Error::GetLastOSErrorNumber();
			if ((err == SOCKETWOULDBLOCK) || (err == EAGAIN)) {
				uint64_t t2 = Time::Get();
				WaitForSocketReadability(this->s, 1000);
				tw += (uint32_t)(Time::Get() - t2);
				tc++;
			}
			else {
			//	Error::PrintLastOSErrorMessage("Error: TCPChannel::recv");
				tcpCS.leave();
				return	1;
			}
		}
		else if (count == 0) {
			WaitForSocketReadability(this->s, 10);
		}
		else {
			bufferPos += count;
			r += count;
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
		bufferPos -= (uint32_t)s;
	}
	tcpCS.leave();
//	std::cout<<"Info: Read "<<s<<" bytes from buffer, "<<bufferLen-bufferPos<<" bytes left..."<<std::endl;

	//if (r)
	//	printf("Took %u us to read %u bytes into buffer (%ux = %u) [%llu]...\n", Time::Get()-t, r, tc, tw, Time::Get());
	//else
	//	printf("   Got %u bytes from buffer [%llu]...\n", s, Time::Get());

	return 0;

}

bool	TCPChannel::isConnected() {

	if (s == INVALID_SOCKET)
		return false;

	uint16_t timeout = 10;

	char peekBuffer[1];
	// First use the socket a bit
	int res = ::recv(s, peekBuffer, 1, MSG_PEEK);
	if (res <= 0) {
		int err = Error::GetLastOSErrorNumber();
		if (err != SOCKETWOULDBLOCK) {
			// Error will be handled by the called
			disconnect();
			return false;
		}
	}

	struct timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = 10;

	int maxfd = 0;
	fd_set wfds;
	// create a list of sockets to check for activity
	FD_ZERO(&wfds);
	// specify socket
	FD_SET(s, &wfds);

	#ifdef WINDOWS
		int len;
	#else
		#ifdef Darwin
			#if GCC_VERSION < 40000
				int len;
				maxfd = s + 1;
			#else
				socklen_t len;
				maxfd = s + 1;
			#endif // GCC_VERSION < 40000
		#else
			socklen_t len;
			maxfd = s + 1;
		#endif
	#endif

	if (timeout > 0) {
		ldiv_t d = ldiv(timeout*1000, 1000000);
		tv.tv_sec = d.quot;
		tv.tv_usec = d.rem;
	}

	// Check for writability
	res = select(maxfd, NULL, &wfds, NULL, &tv);

	if (res <= 0)
		return false;

	int error;
	len = sizeof(error);

	if (FD_ISSET(s, &wfds) != 0) {
		if (getsockopt(s, SOL_SOCKET, SO_ERROR, (char*)&error, &len) != 0) {

			int err = Error::GetLastOSErrorNumber();
			if (err == 0) {
				// No error, just unable to send...
				return true;
			}
			#ifdef WINDOWS
				else if (err == WSAENETDOWN ) {
					return false;
				}
				else if (err == WSAEFAULT  ) {
					return false;
				}
				else if (err == WSAEINPROGRESS  ) {
					return false;
				}
				else if (err == WSAEINVAL  ) {
					return false;
				}
				else if (err == WSAENOPROTOOPT  ) {
					if (error == 0) {
						return true;
					}
				}
				else if (err == WSAENOTSOCK  ) {
					return false;
				}
			#endif			
			return false;
		}
		if (error == 0)
			return true;
	}

	return false;
}

bool	TCPChannel::disconnect() {
	shutdown(s, SD_BOTH);
	closesocket(s);
	s = INVALID_SOCKET;
	return true;
}
