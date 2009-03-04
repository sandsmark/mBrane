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

#include	<iphlpapi.h>


uint32	TCPInterface::Intialized=0;

bool	TCPInterface::Init(){

	if(Intialized){

		Intialized++;
		return	true;
	}

	WSADATA	wsaData;
	int32	r;

	r=WSAStartup(MAKEWORD(2,2),&wsaData);
	if(r){

		std::cout<<"> Error: WSAStartup failed: "<<r<<std::endl;
		return	false;
	}
	Intialized++;
	return	true;
}

void	TCPInterface::Shutdown(){

	if(!--Intialized)
		WSACleanup();
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
	if(!_nic){

		std::cout<<"> Error: NodeConfiguration::Network::"<<n.getName()<<"::nic is missing\n";
		return	false;
	}
	
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
			std::cout<<"> Error: could not get adapter info\n";
			return	false;
		}
	}else
		adapters=&_adapters;

	IP_ADAPTER_INFO	*a;
	uint8	i;
	bool	found=false;
	for(i=0,a=adapters;a;a=a->Next){
		if(strcmp(_nic,a->Description)==0){
			address.s_addr=inet_addr(a->IpAddressList.IpAddress.String);
			found=true;
			break;
		}
	}
	if(size>sizeof(IP_ADAPTER_INFO))
		delete[]	adapters;

	if(!found){

		std::cout<<"> Error: NodeConfiguration::Network::"<<n.getName()<<"::nic "<<_nic<<"does not exist\n";
		return	false;
	}

	const	char	*_port=n.getAttribute("port");
	if(!_port){

		std::cout<<"> Error: NodeConfiguration::Network::"<<n.getName()<<"::port is missing\n";
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
  
	struct	sockaddr_in	addr;
	addr.sin_family= AF_INET;
	addr.sin_addr.s_addr=address.s_addr;
	addr.sin_port=htons((uint16)port);

	if(bind(s,(SOCKADDR	*)&addr,sizeof(struct	sockaddr_in))==SOCKET_ERROR){

		closesocket(s);
		Shutdown();
		return	1;
	}

	// Set non blocking mode
	#if defined(WIN32)
		unsigned long parm = 1;
		// For Blocking:
		// unsigned long parm = 0;
		ioctlsocket(s, FIONBIO, &parm);
	#else
		long parm = fcntl(s, F_GETFL);
		parm |= O_NONBLOCK;
		// For Blocking:
		// parm &= (!O_NONBLOCK);
		fcntl(s, F_SETFL, parm);
	#endif

	return	0;
}

uint16	TCPInterface::stop(){

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

		Shutdown();
		return	1;
	}
	
	struct sockaddr_in	addr;
	addr.sin_family=AF_INET;
	addr.sin_addr=*((struct	in_addr	*)ID);
	addr.sin_port=(uint16)*((uint32	*)(ID+sizeof(struct	in_addr)));
	if(connect(s,(SOCKADDR	*)&addr,sizeof(struct sockaddr_in))==SOCKET_ERROR){

		closesocket(s);
		Shutdown();
		return	1;
	}

	BOOL	off=true;
	setsockopt(s,IPPROTO_TCP,TCP_NODELAY,(char	*)&off,sizeof(BOOL));

	*channel=new	TCPChannel(s);
	
	return	0;
}

uint16	TCPInterface::acceptConnection(ConnectedCommChannel	**channel,int32	timeout,bool	&timedout){

	if(listen(s,1)==SOCKET_ERROR){
		closesocket(s);
		Shutdown();
		return 1;
	}

	mBrane::socket _s = accept(s,NULL,NULL);
	if ((int) _s < 0) {
		int wsaError = getLastOSErrorNumber();
		if ((wsaError == WSAEWOULDBLOCK) || (wsaError == EINPROGRESS)) {
			if (!waitForReadability(timeout)) {
				timedout = true;
				return 0;
			}
		}
		else {
			closesocket(s);
			Shutdown();
			return 1;
		}
	}

	_s = accept(s,NULL,NULL);

	if(_s == INVALID_SOCKET) {
	    closesocket(s);
		Shutdown();
		return 1;
	}

	*channel=new	TCPChannel(_s);
      
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

	#ifdef WIN32
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

int32 TCPInterface::getLastOSErrorNumber() {
	#ifdef WIN32
		int32 err = WSAGetLastError();
		WSASetLastError(0);
		return err;
	#else
		return (int32) errno;
	#endif
}
