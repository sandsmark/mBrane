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
	if(!_nic){

		std::cout<<"> Error: NodeConfiguration::Network::"<<n.getName()<<"::nic is missing"<<std::endl;
		return	false;
	}

	unsigned char* socketAddr;
	char ipAddressString[16];
	bool	found=false;
	bool	gotIPAddress=false;

#if defined (WINDOWS)

	char adaptorString[128];
	PIP_ADAPTER_ADDRESSES pAddresses = NULL;
	ULONG iterations = 0;
	DWORD dwRetVal = 0;
	ULONG outBufLen = 15000;

	unsigned int i = 0;
	// Set the flags to pass to GetAdaptersAddresses
	ULONG flags = GAA_FLAG_INCLUDE_PREFIX;
	// default to unspecified address family (both)
	ULONG family = AF_UNSPEC;
	LPVOID lpMsgBuf = NULL;

	PIP_ADAPTER_ADDRESSES pCurrAddresses = NULL;
	PIP_ADAPTER_UNICAST_ADDRESS pUnicast = NULL;
	PIP_ADAPTER_ANYCAST_ADDRESS pAnycast = NULL;
	PIP_ADAPTER_MULTICAST_ADDRESS pMulticast = NULL;
	IP_ADAPTER_DNS_SERVER_ADDRESS *pDnServer = NULL;
	IP_ADAPTER_PREFIX *pPrefix = NULL;

	do {

		pAddresses = (IP_ADAPTER_ADDRESSES *) malloc(outBufLen);
		if (pAddresses == NULL) {
			std::cout<<"> Error: could not get adapter info"<<std::endl;
			return false;
		}

		dwRetVal = GetAdaptersAddresses(family, flags, NULL, pAddresses, &outBufLen);

		if (dwRetVal == ERROR_BUFFER_OVERFLOW) {
			free(pAddresses);
			pAddresses = NULL;
		} else {
			break;
		}

		iterations++;

	} while ((dwRetVal == ERROR_BUFFER_OVERFLOW) && (iterations < 3));


    if (dwRetVal == NO_ERROR) {
        // If successful, output some information from the data we received
        pCurrAddresses = pAddresses;
        while (pCurrAddresses) {
			gotIPAddress = false;
			if ( (pCurrAddresses->OperStatus == IfOperStatusUp) && (pCurrAddresses->IfIndex != 0)) {

				// First check the IP address
				pUnicast = pCurrAddresses->FirstUnicastAddress;
				if (pUnicast != NULL) {
					for (i = 0; pUnicast != NULL; i++) {
						socketAddr = ((unsigned char*) ((SOCKADDR*)(pUnicast->Address.lpSockaddr))->sa_data) + 2;
						if ( (socketAddr[0] != 0) && (socketAddr[0] != 127) ) {
							sprintf(ipAddressString, "%u.%u.%u.%u", socketAddr[0], socketAddr[1], socketAddr[2], socketAddr[3]);
							if(strstr(ipAddressString, _nic) != NULL)
								found=true;
							// For now only look at the first non-0 and non-127 address on each adaptor
							gotIPAddress = true;
							break;
						}
						pUnicast = pUnicast->Next;
					}
				}

				// Then check the adaptor description field
				if ((!found) && gotIPAddress) {
					if (WideCharToMultiByte( CP_ACP, 0, pCurrAddresses->Description, -1, adaptorString, 128, NULL, NULL ) != 0) {
						if(strstr(adaptorString, _nic) != NULL)
							found=true;
					}
				}

				if (found) {
					address.s_addr=inet_addr(ipAddressString);
					break;
				}

				//printf("\tAdapter name: %s\n", pCurrAddresses->AdapterName);
				//printf("\tDescription: %wS\n", pCurrAddresses->Description);
				//printf("\tFriendly name: %wS\n", pCurrAddresses->FriendlyName);
				//printf("\n");
			}

            pCurrAddresses = pCurrAddresses->Next;
        }
	} else {
		if (pAddresses)
			free(pAddresses);
		return false;
	}

	if (pAddresses)
		free(pAddresses);

#endif

#if defined (LINUX)
	int sock = 0;
	struct ifreq ifreq;
	struct sockaddr_in *saptr = NULL;
	struct if_nameindex *iflist = NULL, *listsave = NULL;

	//need a socket for ioctl()
	if( (sock = ::socket(AF_INET, SOCK_STREAM, 0)) < 0)
		return false;

	//returns pointer to dynamically allocated list of structs
	iflist = listsave = if_nameindex();

	if (iflist == NULL) {
		close(sock);
		return false;
	}

	//walk thru the array returned and query for each
	//interface's address
	for(iflist; iflist->if_index != 0; iflist++) {
		gotIPAddress = false;
		//copy in the interface name to look up address of
		strncpy(ifreq.ifr_name, iflist->if_name, IF_NAMESIZE);
		//get the address for this interface
		if(ioctl(sock, SIOCGIFADDR, &ifreq) != 0) {
			// ignore;
			continue;
		}
		saptr = (struct sockaddr_in *)&ifreq.ifr_addr;

		// First check the IP Address
		socketAddr = (unsigned char*) &(saptr->sin_addr.s_addr);
		if ( (socketAddr[0] != 0) && (socketAddr[0] != 127) ) {
			sprintf(ipAddressString, "%u.%u.%u.%u", socketAddr[0], socketAddr[1], socketAddr[2], socketAddr[3]);
			if(strstr(ipAddressString, _nic) != NULL)
				found=true;
			// For now only look at the first non-0 and non-127 address on each adaptor
			gotIPAddress = true;
		}

		// Then check the adaptor name
		if ((!found) && gotIPAddress) {
			if(strstr(ifreq.ifr_name, _nic) != NULL)
				found=true;
		}

		if (found) {
			memcpy(&address, &saptr->sin_addr, sizeof(struct in_addr));
			break;
		}
	}

	//free the dynamic memory kernel allocated for us
	if_freenameindex(listsave);
	close(sock); 

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

	core::socket	s;
	if((s=::socket(AF_INET,SOCK_STREAM,IPPROTO_TCP))==INVALID_SOCKET){
		std::cout<<"> Error: Could not create TCP socket to "<<inet_ntoa(*(struct in_addr *)ID)<<
			":" << (unsigned short)*((uint32 *)(ID+sizeof(struct in_addr))) << std::endl;
		Shutdown();
		return	1;
	}

	//struct linger ling = {1, 0};
	//setsockopt(s, SOL_SOCKET, SO_LINGER, (char *)&ling, sizeof(ling));

	//char delay = 1;
	//setsockopt(s, IPPROTO_TCP, TCP_NODELAY, (char*) &delay, sizeof(delay));

	//char buffsize = 1;
	//setsockopt(s, SOL_SOCKET, SO_SNDBUF, &buffsize, sizeof(buffsize));

	char errbuf[1024];
	struct sockaddr_in	addr;
	addr.sin_family=AF_INET;
	addr.sin_addr=*((struct	in_addr	*)ID);
	addr.sin_port=htons((unsigned short)*((uint32 *)(ID+sizeof(struct in_addr))));
	if(connect(s,(SOCKADDR	*)&addr,sizeof(struct sockaddr_in))==SOCKET_ERROR){
		Error::GetOSErrorMessage(errbuf, 1024);
		std::cout<<"> Error: Could not connect TCP socket to "<<inet_ntoa(*(struct in_addr *)ID)<<
			":" << (unsigned short)*((uint32 *)(ID+sizeof(struct in_addr))) << " - " << errbuf << std::endl;
		closesocket(s);
		Shutdown();
		return	1;
	}

	//std::cout<<"> Info: Opened TCP connection to "<<inet_ntoa(*(struct in_addr *)ID)<<
	//	":" << (unsigned short)*((uint32 *)(ID+sizeof(struct in_addr))) << std::endl;
	*channel=new	TCPChannel(s);
	
	return	0;
}

uint16	TCPInterface::acceptConnection(ConnectedCommChannel	**channel,int32	timeout,bool	&timedout){

	// std::cout<<"> Info: Listening for TCP connection on port " << port << std::endl;

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

	core::socket _s = accept(s,NULL,NULL);

	if ((int) _s < 0) {
		int wsaError = Error::GetLastOSErrorNumber();
		if ((wsaError == WSAEWOULDBLOCK) || (wsaError == EINPROGRESS)) {
			if (!WaitForSocketReadability(s, timeout)) {
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
		else {
			closesocket(s);
			Shutdown();
			return 1;
		}
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


