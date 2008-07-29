// network_interface.cpp
//
// Author: Eric Nivel
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

#include	"network_interface.h"
#include	"payload.h"

#include	<iostream>


namespace	mBrane{
	namespace	sdk{

		NetworkInterface::NetworkInterface(){
		}

		NetworkInterface::~NetworkInterface(){

			if(library)
				delete	library;
		}

		NetworkInterface	*NetworkInterface::load(XMLNode	&n){

			const	char	*l=n.getAttribute("shared_library");
			if(!l){

				std::cout<<"Error: "<<n.getName()<<"::shared_library is missing\n";
				return	NULL;
			}
			if(library=SharedLibrary::New(l)){

				if(!(start=library->getFunction<Start>("Start"))){

					std::cout<<"Error: "<<n.getName()<<": could not find function Start\n";
					return	NULL;
				}
				if(!(stop=library->getFunction<Stop>("Stop"))){

					std::cout<<"Error: "<<n.getName()<<": could not find function Stop\n";
					return	NULL;
				}
				return	this;
			}
			return	NULL;
		}

		////////////////////////////////////////////////////////////////////////////////////////////////

		NetworkDiscoveryInterface::NetworkDiscoveryInterface():NetworkInterface(){
		}

		NetworkDiscoveryInterface::~NetworkDiscoveryInterface(){
		}

		NetworkInterface	*NetworkDiscoveryInterface::load(XMLNode	&n){

			if(!NetworkInterface::load(n))
				return	NULL;
			if(!(broadcastID=library->getFunction<BroadcastID>("BroadcastID"))){

				std::cout<<"Error: "<<n.getName()<<": could not find function BroadcastID\n";
				return	NULL;
			}
			if(!(scanID=library->getFunction<ScanID>("ScanID"))){

				std::cout<<"Error: "<<n.getName()<<": could not find function ScanID\n";
				return	NULL;
			}
			return	this;
		}

		////////////////////////////////////////////////////////////////////////////////////////////////

		NetworkCommInterface::NetworkCommInterface():NetworkInterface(){
		}
				
		NetworkCommInterface::~NetworkCommInterface(){
		}

		NetworkInterface	*NetworkCommInterface::load(XMLNode	&n){

			if(!NetworkInterface::load(n))
				return	NULL;
			if(!(rtt=library->getFunction<RTT>("RTT"))){

				std::cout<<"Error: "<<n.getName()<<": could not find function RTT\n";
				return	NULL;
			}
			if(!(canBroadcast=library->getFunction<CanBroadcast>("CanBroadcast"))){

				std::cout<<"Error: "<<n.getName()<<": could not find function CanBroadcast\n";
				return	NULL;
			}
			if(!(getIDSize=library->getFunction<GetIDSize>("GetIDSize"))){

				std::cout<<"Error: "<<n.getName()<<": could not find function GetIDSize\n";
				return	NULL;
			}
			if(!(fillID=library->getFunction<FillID>("FillID"))){

				std::cout<<"Error: "<<n.getName()<<": could not find function FillID\n";
				return	NULL;
			}
			if(!(connect=library->getFunction<Connect>("Connect"))){

				std::cout<<"Error: "<<n.getName()<<": could not find function Connect\n";
				return	NULL;
			}
			if(!(acceptConnection=library->getFunction<AcceptConnection>("AcceptConnection"))){

				std::cout<<"Error: "<<n.getName()<<": could not find function AcceptConnection\n";
				return	NULL;
			}
			return	this;
		}

		////////////////////////////////////////////////////////////////////////////////////////////////

		CommChannel::CommChannel(){
		}

		CommChannel::~CommChannel(){
		}

		int16	CommChannel::send(_Payload	*p){

			ClassRegister	*CR=ClassRegister::Get(p->cid());
			int16	r;
			if(r=send(((uint8	*)p)+CR->offset(),CR->size()))
				return	r;
			for(uint8	i=0;i<p->ptrCount();i++){

				if(r=send(*p->ptr(i)))
					return	r;
			}
			return	0;
		}

		int16	CommChannel::recv(_Payload	**p){

			uint16	cid;
			int16	r;
			if(r=recv((uint8	*)&cid,sizeof(uint16),true))
				return	r;
			ClassRegister	*CR=ClassRegister::Get(cid);
			*p=(_Payload	*)CR->allocator()->alloc();
			if(r=recv((uint8	*)*p,CR->size()))
				return	r;
			_Payload	*ptr;
			P<_Payload>	*_ptr;
			for(uint8	i=0;i<(*p)->ptrCount();i++){

				if(r=recv(&ptr)){

					delete	*p;
					return	r;
				}
				_ptr=(*p)->ptr(i);
				*_ptr=ptr;
			}
			(*p)->init();
			return	0;
		}

		////////////////////////////////////////////////////////////////////////////////////////////////

		ConnectedCommChannel::ConnectedCommChannel(uint16	remoteNID):CommChannel(),remoteNID(remoteNID){
		}

		ConnectedCommChannel::~ConnectedCommChannel(){
		}

		////////////////////////////////////////////////////////////////////////////////////////////////

		BroadcastCommChannel::BroadcastCommChannel():CommChannel(){
		}

		BroadcastCommChannel::~BroadcastCommChannel(){
		}
	}
}