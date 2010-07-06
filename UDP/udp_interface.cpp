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
	if(!_nic){

		std::cout<<"> Error: NodeConfiguration::Network::"<<n.getName()<<"::nic is missing"<<std::endl;
		return	false;
	}

	char ipAddressString[16];
	bool	found=false;
	bool	gotIPAddress=false;

#if defined (WINDOWS)

	unsigned char* socketAddr;
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
	if( (sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
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
		key = ifreq.ifr_name;

		// First check the IP Address
		socketAddr = (unsigned char*) saptr->sin_addr.s_addr
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
			memcpy(&if_addr, &ifreq.ifr_addr, sizeof(struct sockaddr_in));
			memcpy(&address, &if_addr.sin_addr, sizeof(struct in_addr));
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
