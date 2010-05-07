//	udp_interface.cpp
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

#include	"udp_interface.h"
#include	"udp_channel.h"

#if defined (WINDOWS)
#include	<iphlpapi.h>
#endif

#if defined (LINUX)
#include <sys/ioctl.h>
#include <net/if.h>
#endif

#include	<cstring>

uint32	UDPInterface::Intialized=0;

bool	UDPInterface::Init(){

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

void	UDPInterface::Shutdown(){

	if(!--Intialized) {
#if defined (WINDOWS)
		WSACleanup();
#endif
	}
}

UDPInterface	*UDPInterface::New(XMLNode &n){

	if(!Init())
		return	NULL;

	UDPInterface	*i=new	UDPInterface();
	if(!i->load(n)){

		delete	i;
		Shutdown();
		return	NULL;
	}
	return	i;
}

UDPInterface::UDPInterface():NetworkInterface(UDP){
}

UDPInterface::~UDPInterface(){

	Shutdown();
}

bool	UDPInterface::load(XMLNode	&n){

	const	char	*_nic=n.getAttribute("nic");
	bool	found=false;
	if(!_nic){

		std::cout<<"> Error: NodeConfiguration::Network::"<<n.getName()<<"::nic is missing"<<std::endl;
		return	false;
	}
	
#if defined (WINDOWS)
	uint32	size=sizeof(IP_ADAPTER_INFO);
    IP_ADAPTER_INFO	_adapters;
	IP_ADAPTER_INFO	*adapters=NULL;
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
		std::cout << "> Error: unable to determine address of "<< _nic<<" via socket "<< if_socket <<std::endl;
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

bool	UDPInterface::operator	==(NetworkInterface	&i){

	if(i.protocol()!=protocol())
		return	false;
	return	strcmp(inet_ntoa(address),inet_ntoa(((UDPInterface	*)&i)->address))==0	&&	port==((UDPInterface	*)&i)->port;
}

bool	UDPInterface::operator	!=(NetworkInterface	&i){

	return	!operator	==(i);
}

bool	UDPInterface::canBroadcast(){

	return	true;
}

uint16	UDPInterface::start(){

	return	0;
}

uint16	UDPInterface::stop(){

	Shutdown();
	return	0;
}

uint16	UDPInterface::getIDSize(){

	return	sizeof(struct	in_addr)+sizeof(uint32);
}

void	UDPInterface::fillID(uint8	*ID){	//	address|port

	memcpy(ID,&address,sizeof(struct	in_addr));
	memcpy(ID+sizeof(struct	in_addr),&port,sizeof(uint32));
}

uint16	UDPInterface::newChannel(uint8	*ID,CommChannel	**channel){

	core::socket	s;
	if((s=::socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP))==SOCKET_ERROR){

		closesocket(s);
		Shutdown();
		return	1;
	}

	//bool	on=true;
	int one = 1;
	setsockopt(s,SOL_SOCKET,SO_BROADCAST,(char	*)&one,sizeof(one));

	/* we also need to bind the listener to the right port */
	struct sockaddr_in if_addr;
	if_addr.sin_family = AF_INET;
	if_addr.sin_addr.s_addr = INADDR_ANY;		// Since this is a server, any address will do
	if_addr.sin_port = htons((unsigned short)port);
	if (bind(s, (struct sockaddr *)&if_addr, sizeof(struct sockaddr_in)) < 0) {
		std::cout<<"> Error: unable to bind to port " << port << std::endl;
		return false;
	}
	std::cout<<"> Info: UDP bound to port " << port << std::endl;


	*channel=new	UDPChannel(s,port);

	return	0;
}

uint16	UDPInterface::acceptConnection(ConnectedCommChannel	**channel,int32	timeout,bool	&timedout){

	return	1;
}
