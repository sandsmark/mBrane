//	tcp_interface.cpp
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

#include	"tcp_interface.h"
#include	"tcp_channel.h"

#if defined (WINDOWS)
#include	<iphlpapi.h>
#endif

#include	<cstring>

#if defined (LINUX)
#include	<unistd.h>
#include	<fcntl.h>
#include	<netinet/tcp.h>
#include	<errno.h>
#include	<sys/ioctl.h>
#include	<net/if.h>
#endif

uint32	TCPInterface::Intialized=0;

bool	TCPInterface::Init(){

	if(Intialized){

		Intialized++;
		return	true;
	}

#if defined (WINDOWS)
	WSADATA	wsaData;
	int32	r;

	r=WSAStartup(MAKEWORD(2,2),&wsaData);
	if(r){

		std::cout<<"> Error: WSAStartup failed: "<<r<<std::endl;
		return	false;
	}
#endif
	Intialized++;
	return	true;
}

void	TCPInterface::Shutdown(){

	if(!--Intialized){
#if defined (WINDOWS)
		WSACleanup();
#endif
	}
}

TCPInterface	*TCPInterface::New(XMLNode &n){

	if(!Init())
		return	NULL;

	TCPInterface	*i=new	TCPInterface();
	if(!i->load(n)){

		delete	i;
		Shutdown();
		return	NULL;
	}
	return	i;
}

TCPInterface::TCPInterface():NetworkInterface(TCP){
}

TCPInterface::~TCPInterface(){
}

bool	TCPInterface::load(XMLNode	&n){

	const	char	*_nic=n.getAttribute("nic");
	bool	found=false;
	if(!_nic){

		std::cout<<"> Error: NodeConfiguration::Network::"<<n.getName()<<"::nic is missing"<<std::endl;
		return	false;
	}
	
#if defined (WINDOWS)
	uint32	size=sizeof(IP_ADAPTER_INFO);
    IP_ADAPTER_INFO	_adapters;
	IP_ADAPTER_INFO	*adapters = NULL;
	// Make an initial call to GetAdaptersInfo to get
	// the necessary size
	uint32	r=GetAdaptersInfo(&_adapters,&size);	//	initial call to get the actual size
	if(r==ERROR_BUFFER_OVERFLOW){
		// Now we know the size, allocate and call again
		adapters=(IP_ADAPTER_INFO *) new uint8[size];
		r=GetAdaptersInfo(adapters,&size);
		if(r!=ERROR_SUCCESS) {
			std::cout<<"> Error: could not get adapter info"<<std::endl;
			return	false;
		}
	}else
		adapters=&_adapters;

	IP_ADAPTER_INFO	*a;
	uint8	i;
	for(i=0,a=adapters;a;a=a->Next){
		if(strcmp(_nic,a->Description)==0){
			address.s_addr=inet_addr(a->IpAddressList.IpAddress.String);
			found=true;
			break;
		}
	}
	if(size>sizeof(IP_ADAPTER_INFO))
		delete[]	adapters;
#endif

#if defined (LINUX)
	mBrane::socket if_socket = 0;
	struct ifreq ifreq;
	struct sockaddr_in if_addr;

	if ((if_socket = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
		std::cout << "> Error: unable to obtain a socket"<<std::endl;
		return false;
	}

	strncpy(ifreq.ifr_name, _nic, IFNAMSIZ);

	if (ioctl(if_socket, SIOCGIFADDR, &ifreq) < 0) {
		std::cout << "> Error: unable to determine address of "<< _nic<<" via socket "<< if_socket<<std::endl;
		return false;
	}

	memcpy(&if_addr, &ifreq.ifr_addr, sizeof(struct sockaddr_in));

	memcpy(&address, &if_addr.sin_addr, sizeof(struct in_addr));

	found = true;
#endif


	if(!found){

		std::cout<<"> Error: NodeConfiguration::Network::"<<n.getName()<<"::nic "<<_nic<<"does not exist"<<std::endl;
		return	false;
	}

	const	char	*_port=n.getAttribute("port");
	if(!_port){

		std::cout<<"> Error: NodeConfiguration::Network::"<<n.getName()<<"::port is missing"<<std::endl;
		return	false;
	}
	port=atoi(_port);

	return	true;
}

bool	TCPInterface::operator	==(NetworkInterface	&i){

	if(i.protocol()!=protocol())
		return	false;
	return	strcmp(inet_ntoa(address),inet_ntoa(((TCPInterface	*)&i)->address))==0	&&	port==((TCPInterface	*)&i)->port;
}

bool	TCPInterface::operator	!=(NetworkInterface	&i){

	return	!operator	==(i);
}

bool	TCPInterface::canBroadcast(){

	return	false;
}

uint16	TCPInterface::start(){

	if((s=::socket(AF_INET,SOCK_STREAM,IPPROTO_TCP))==INVALID_SOCKET){

		Shutdown();
		return	1;
	}
  
	#ifndef WINDOWS
		/*
			This socket option tells the kernel that even if this port is busy (in
			the TIME_WAIT state), go ahead and reuse it anyway.  If it is busy,
			but with another state, you will still get an address already in use
			error.  It is useful if your server has been shut down, and then
			restarted right away while sockets are still active on its port.  You
			should be aware that if any unexpected data comes in, it may confuse
			your server, but while this is possible, it is not likely.
		*/
		int one = 1;
		setsockopt(s,SOL_SOCKET,SO_REUSEADDR,&one,sizeof(one));
	#endif

	struct	sockaddr_in	addr;
	addr.sin_family= AF_INET;
	addr.sin_addr.s_addr=address.s_addr;
	addr.sin_port=htons((uint16)port);

	if(bind(s,(SOCKADDR	*)&addr,sizeof(struct	sockaddr_in))==SOCKET_ERROR){
		std::cout<<"> Error: unable to bind to port " << port << std::endl;
		closesocket(s);
		Shutdown();
		return	1;
	}
	std::cout<<"> Info: TCP bound to port " << port << std::endl;

	return	0;
}

uint16	TCPInterface::stop(){

	std::cout<<"> Info: Closing TCP bound to port " << port << std::endl;
	closesocket(s);
	Shutdown();
	return	0;
}

uint16	TCPInterface::getIDSize(){

	return	sizeof(struct	in_addr)+sizeof(uint32);
}

void	TCPInterface::fillID(uint8	*ID){	//	address|port

	memcpy(ID,&address,sizeof(struct	in_addr));
	memcpy(ID+sizeof(struct	in_addr),&port,sizeof(uint32));
}

uint16	TCPInterface::newChannel(uint8	*ID,CommChannel	**channel){	//	connect to a server

	mBrane::socket	s;
	if((s=::socket(AF_INET,SOCK_STREAM,IPPROTO_TCP))==INVALID_SOCKET){
		std::cout<<"> Error: Could not create TCP socket to "<<inet_ntoa(*(struct in_addr *)ID)<<
			":" << (unsigned short)*((uint32 *)(ID+sizeof(struct in_addr))) << std::endl;
		Shutdown();
		return	1;
	}

	struct linger ling = {1, 0};
	setsockopt(s, SOL_SOCKET, SO_LINGER, (char *)&ling, sizeof(ling));

	char delay = 1;
	setsockopt(s, IPPROTO_TCP, TCP_NODELAY, (char*) &delay, sizeof(delay));

	char errbuf[1024];
	struct sockaddr_in	addr;
	addr.sin_family=AF_INET;
	addr.sin_addr=*((struct	in_addr	*)ID);
	addr.sin_port=htons((unsigned short)*((uint32 *)(ID+sizeof(struct in_addr))));
	if(connect(s,(SOCKADDR	*)&addr,sizeof(struct sockaddr_in))==SOCKET_ERROR){
		getOSErrorMessage(errbuf, 1024);
		std::cout<<"> Error: Could not connect TCP socket to "<<inet_ntoa(*(struct in_addr *)ID)<<
			":" << (unsigned short)*((uint32 *)(ID+sizeof(struct in_addr))) << " - " << errbuf << std::endl;
		closesocket(s);
		Shutdown();
		return	1;
	}

	std::cout<<"> Info: Opened TCP connection to "<<inet_ntoa(*(struct in_addr *)ID)<<
		":" << (unsigned short)*((uint32 *)(ID+sizeof(struct in_addr))) << std::endl;
	*channel=new	TCPChannel(s);
	
	return	0;
}

uint16	TCPInterface::acceptConnection(ConnectedCommChannel	**channel,int32	timeout,bool	&timedout){

	std::cout<<"> Info: Listening for TCP connection on port " << port << std::endl;

	// Set blocking mode
	#if defined(WINDOWS)
		unsigned long parm = ( (timeout >= 0) ? 1 : 0); // 1 = Non-blocking, 0 = Blocking
		ioctlsocket(s, FIONBIO, &parm);
	#else
		long parm = fcntl(s, F_GETFL);
		if (timeout >= 0)
			parm |= O_NONBLOCK;
		else
			parm &= (!O_NONBLOCK);
		fcntl(s, F_SETFL, parm);
	#endif

	if(listen(s,1)==SOCKET_ERROR){
		closesocket(s);
		Shutdown();
		return 1;
	}

	mBrane::socket _s = accept(s,NULL,NULL);

	if ( ((int) _s < 0) && (timeout > 0) ) {
		int wsaError = getLastOSErrorNumber();
		if ((wsaError == WSAEWOULDBLOCK) || (wsaError == EINPROGRESS)) {
			if (!waitForReadability(timeout)) {
				timedout = true;
				return 0;
			}
			// Try again...
			_s = accept(s,NULL,NULL);
			if ((int) _s < 0) {
				timedout = true;
				return 0;
			}
		}
	}

	if ((int) _s < 0) {
		closesocket(s);
		Shutdown();
		return 1;
	}

	//char buffer[1024];
	//int count = ::recv(_s,buffer,1,0);
	//if(count==SOCKET_ERROR) {
	//	std::cout<<"Error: TCPChannel::recv ["<<(int)_s<<"] code "<< getLastOSErrorNumber()<<std::endl;
	//	return	1;
	//}

	timedout = false;
	*channel=new	TCPChannel(_s);
      
	std::cout<<"> Info: Accepted TCP connection ["<<(int)_s<<"] to port " << port << std::endl;

	return	0;
}


bool TCPInterface::waitForReadability(int32 timeout) {

	int maxfd = 0;

	struct timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = 0;

	fd_set rdds;
	// create a list of sockets to check for activity
	FD_ZERO(&rdds);
	// specify mySocket
	FD_SET(s, &rdds);

	#ifdef WINDOWS
	#else
		maxfd = s + 1;
	#endif

	if (timeout > 0) {
		ldiv_t d = ldiv(timeout*1000, 1000000);
		tv.tv_sec = d.quot;
		tv.tv_usec = d.rem;
	}

	// Check for readability
	int ret = select(maxfd, &rdds, NULL, NULL, &tv);
	return(ret > 0);
}

