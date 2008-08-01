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
#include	"message.h"

#include	<iostream>


namespace	mBrane{
	namespace	sdk{

		NetworkInterface::NetworkInterface(Protocol	_protocol):_protocol(_protocol){
		}

		NetworkInterface::~NetworkInterface(){
		}

		NetworkInterface::Protocol	NetworkInterface::protocol()	const{

			return	_protocol;
		}

		////////////////////////////////////////////////////////////////////////////////////////////////

		NetworkInterfaceLoader	*NetworkInterfaceLoader::New(XMLNode	&n){

			const	char	*l=n.getAttribute("shared_library");
			if(!l){

				std::cout<<"Error: "<<n.getName()<<"::shared_library is missing\n";
				return	NULL;
			}
			SharedLibrary	*library;
			if(library=SharedLibrary::New(l)){

				NetworkInterface::Load	load;
				if(!(load=library->getFunction<NetworkInterface::Load>("Load"))){

					std::cout<<"Error: "<<n.getName()<<": could not find function Load\n";
					return	NULL;
				}
				return	new	NetworkInterfaceLoader(library,load);
			}
			return	NULL;
		}

		NetworkInterfaceLoader::NetworkInterfaceLoader(SharedLibrary	*library,NetworkInterface::Load	load):library(library),load(load){
		}

		NetworkInterfaceLoader::~NetworkInterfaceLoader(){

			if(library)
				delete	library;
		}

		NetworkInterface	*NetworkInterfaceLoader::getInterface(XMLNode	&n){

			if(load)
				return	load(n);
			return	NULL;
		}

		////////////////////////////////////////////////////////////////////////////////////////////////

		CommChannel::CommChannel(){
		}

		CommChannel::~CommChannel(){
		}

		int16	CommChannel::send(_Payload	*p){

			p->node_send_ts()=Time::Get();
			ClassRegister	*CR=ClassRegister::Get(p->cid());
			int16	r;
			if(p->isStreamData()	&&	((_StreamData	*)p)->hasCodec()){

				((_StreamData	*)p)->compress();
				uint32	s=((_StreamData	*)p)->compressedSize();
				if(r=send(((uint8	*)&s,sizeof(uint32)))
					return	r;
				if(r=send(((uint8	*)p)+CR->offset(),s>CR->size()?s:CR->size()))
					return	r;
			}else	if(r=send(((uint8	*)p)+CR->offset(),CR->size()))
				return	r;
			for(uint8	i=0;i<p->ptrCount();i++){

				if(r=send(*p->ptr(i)))
					return	r;
			}
			return	0;
		}

		int16	CommChannel::recv(_Payload	**p){	//	TODO:	recv compressed size; decompress; BTW: generalize hascodec/co/dec to _Payload

			uint16	cid;
			int16	r;
			if(r=recv((uint8	*)&cid,sizeof(uint16),true))
				return	r;
			ClassRegister	*CR=ClassRegister::Get(cid);
			*p=(_Payload	*)CR->allocator()->alloc();
			if(r=recv((uint8	*)*p,CR->size()))
				return	r;
			(*p)->node_recv_ts()=Time::Get();
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

		ConnectedCommChannel::ConnectedCommChannel():CommChannel(){
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