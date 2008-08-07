//	udp_interface.cpp
//
//	Author: Eric Nivel
//
//	BSD license:
//	Copyright (c) 2008, Eric Nivel, Thor List
//	All rights reserved.
//	Redistribution and use in source and binary forms, with or without
//	modification, are permitted provided that the following conditions are met:
//
//   - Redistributions of source code must retain the above copyright
//     notice, this list of conditions and the following disclaimer.
//   - Redistributions in binary form must reproduce the above copyright
//     notice, this list of conditions and the following disclaimer in the
//     documentation and/or other materials provided with the distribution.
//   - Neither the name of Eric Nivel, Thor List nor the
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


uint32	UDPInterface::Intialized=0;

bool	UDPInterface::Init(){

	if(Intialized){

		Intialized++;
		return	true;
	}

	WSADATA	wsaData;
	int32	r;

	r=WSAStartup(MAKEWORD(2,2),&wsaData);
	if(r){

		std::cout<<"Error: WSAStartup failed: "<<r<<std::endl;
		return	false;
	}
	Intialized++;
	return	true;
}

void	UDPInterface::Shutdown(){

	if(!--Intialized)
		WSACleanup();
}

UDPInterface	*UDPInterface::New(XMLNode &n){

	UDPInterface	*i=new	UDPInterface();
	if(!i->load(n)){

		delete	i;
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

	if(!Init())
		return	false;

	const	char	*_nic=n.getAttribute("nic");
	if(!_nic){

		std::cout<<"Error: NodeConfiguration::Network::"<<n.getName()<<"::nic is missing\n";
		goto	err;
	}
	//	TODO:	set address to the addr of the eth interface specified by nic: see Psyclone::CoreLibrary::getLocalInterfaceAddresses
	const	char	*_port=n.getAttribute("port");
	if(!_port){

		std::cout<<"Error: NodeConfiguration::Network::"<<n.getName()<<"::port is missing\n";
		goto	err;
	}
	port=atoi(_port);

	return	true;
err:Shutdown();
	return	false;
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

uint16	UDPInterface::start(){	//	TODO

	return	0;
}

uint16	UDPInterface::stop(){	//	TODO

	return	0;
}

uint16	UDPInterface::getIDSize(){	//	TODO

	return	0;
}

void	UDPInterface::fillID(uint8	*ID){	//	TODO

	
}

uint16	UDPInterface::newChannel(uint8	*ID,CommChannel	**channel){	//	TODO

	return	0;
}

uint16	UDPInterface::acceptConnection(ConnectedCommChannel	**channel,int32	timeout,bool	&timedout){

	return	1;
}