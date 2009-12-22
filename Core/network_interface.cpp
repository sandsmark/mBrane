//	network_interface.cpp
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

		CommChannel::CommChannel(){
		}

		CommChannel::~CommChannel(){
		}

		inline int16	CommChannel::_send(__Payload	*c){

			ClassRegister	*CR=ClassRegister::Get(c->cid());
			int16	r;

			std::cout<<"Info: Sending payload type '"<<CR->class_name<<"' ["<<c->cid()<<"] size '"<<CR->size()<<"'..."<<std::endl;

			switch(c->allocationScheme()){
			case	STATIC:
				if(r=send(((uint8	*)c)+CR->offset(),CR->size()))
					return	r;
				break;
			case	COMPRESSED:
				if(c->as_CompressedData()->needsCompression())
					c->as_CompressedData()->compress();
			case	RAW:
			case	DYNAMIC:
				if(r=send(((uint8	*)c)+CR->offset(),CR->coreSize()+c->as_DynamicData()->dynamicSize()))
					return	r;
				break;
			}

			uint8		ptrCount=c->ptrCount();
			__Payload	*p;
			for(uint8	i=0;i<ptrCount;i++){

				p=c->getPtr(i);
				if(!p)
					continue;
				if(r=_send(p))
					return	r;
			}
			return	0;
		}

		inline int16	CommChannel::_recv(__Payload	**c){

			uint64	metaData;
			int16	r;
			if(r=recv((uint8	*)&metaData,sizeof(uint64),true))	//	receive __Payload::_metaData
				return	r;
			//	allocate and initialize the payload (default ctor is called)
			AllocationScheme	a=(AllocationScheme)(((uint32)metaData)	&	0x00000003);
			ClassRegister		*CR=ClassRegister::Get(((uint32)metaData)>>16);
			uint32	size;
			if(a==RAW) {
				size=((uint32)metaData)>>2;
				std::cout<<"Info: Receiving payload type '"<<CR->class_name<<"' ["<<(((uint32)metaData)>>16)<<"] (RAW) size '"<<size<<"'..."<<std::endl;
			}
			else {
				size=CR->size();
				std::cout<<"Info: Receiving payload type '"<<CR->class_name<<"' ["<<(((uint32)metaData)>>16)<<"] size '"<<size<<"'..."<<std::endl;
			}
			*c=(__Payload*)(*CR->allocator())(size);

			switch(a){
			case	STATIC:
			case	RAW:
				if(r=recv(((uint8	*)*c)+CR->offset(),size))
					return	r;
				break;
			case	COMPRESSED:
			case	DYNAMIC:
				if(r=recv(((uint8	*)*c)+CR->offset(),CR->coreSize()+(*c)->as_DynamicData()->dynamicSize()))
					return	r;
				break;
			}
			if(a==COMPRESSED)
				(*c)->as_CompressedData()->decompress();

			uint8		ptrCount=(*c)->ptrCount();
			__Payload	*p;
			for(uint8	i=0;i<ptrCount;i++){

				if(r=_recv(&p)){

					delete	*c;
					return	r;
				}
				(*c)->setPtr(i,p);
			}
			(*c)->init();
			return	0;
		}

		int16	CommChannel::send(_Payload	*p){

			p->node_send_ts()=Time::Get();
			return	_send(p);
		}

		int16	CommChannel::recv(_Payload	**p){

			int16	r;
			if(r=_recv((__Payload	**)p))
				return	r;
			(*p)->node_recv_ts()=Time::Get();
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
