//	network_interface.cpp
//
//	Author: Eric Nivel
//
//	BSD license:
//	Copyright (c) 2010, Eric Nivel
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
#include	"module_node.h"

#include	<iostream>


using	namespace	mBrane::sdk::module;

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
			sendBufferPos = 0;
			sendBufferLen = 4096;
			sendBuffer = (uint8*) malloc(sendBufferLen);
		}

		CommChannel::~CommChannel(){
			free(sendBuffer);
			sendBuffer = NULL;
			sendBufferPos = 0;
			sendBufferLen = 0;
		}

		inline int16	CommChannel::_send(__Payload	*c,uint8	destinationNID){

			// uint64 t1 = Time::Get();
			ClassRegister	*CR=ClassRegister::Get(c->cid());
			int16	r;

			uint32	size = (uint32)c->size();

			std::cout<<"Info: Sending payload type '"<<CR->class_name<<"' ["<<c->cid()<<"] size '"<<size<<"'..."<<std::endl;

			commSendCS.enter();

			if(r=bufferedSend((uint8	*)&size,sizeof(uint32))) {	//	send the total size first (includes the size of the non transmitted data): will be used to alloc on the recv side
				commSendCS.leave();
				return	r;
			}

			if(destinationNID!=0xFF	&&	(c->isConstant()	||	c->isShared())){

				if(((_Payload	*)c)->getOID()==0x00FFFFFF)	// object is shared and has never been sent and is not in the cache yet.
					Node::Get()->addSharedObject((_Payload	*)c);

				if(r=bufferedSend(((uint8	*)c)+CR->offset(),sizeof(uint64))) {	//	send the metadata.	
					commSendCS.leave();
					return	r;
				}
				if(c->isConstant()) {
					if (r=bufferedSend(NULL, 0, true)) {
						commSendCS.leave();
						return r;
					}
					commSendCS.leave();
					return	0;
				}

				if(!Node::Get()->hasLookup(destinationNID,((_Payload	*)c)->getOID())){	//	the destination node does not have the object already.

					if(r=bufferedSend(((uint8	*)c)+CR->offset(),size-CR->offset()-sizeof(uint64))) {	//	send the rest of the object.
						commSendCS.leave();
						return	r;
					}
					Node::Get()->addLookup(destinationNID,((_Payload	*)c)->getOID());	//	we now know that the receiver has the object.
																							//	the receiver also knows now that we have it.
				}
			}else	if(r=bufferedSend(((uint8	*)c)+CR->offset(),size-CR->offset())) {	//	send in full.
				commSendCS.leave();
				return	r;
			}
			
			uint8		ptrCount=(uint8)c->ptrCount();
			__Payload	*p;
			for(uint8	i=0;i<ptrCount;i++){

				p=c->getPtr(i);
				if(!p)
					continue;
				if(r=_send(p,destinationNID)) {
					commSendCS.leave();
					return	r;
				}
			}
		//	printf("CommChannel Send time:    %u\n", (uint32) (Time::Get() - t1));
			if (r=bufferedSend(NULL, 0, true)) {
				commSendCS.leave();
				return r;
			}
			commSendCS.leave();
			return	0;
		}

		int16	CommChannel::bufferedSend(uint8 *b,size_t s, bool sendNow) {
			if (b && s) {
				while (sendBufferLen-sendBufferPos < s) {
					sendBufferLen += (uint32)(s<4096 ? 4096 : s);
					sendBuffer = (uint8*)realloc(sendBuffer, sendBufferLen);
				}
				memcpy(sendBuffer+sendBufferPos, b, s);
				sendBufferPos += (uint32)s;
			}

			if (!sendNow)
				return 0;

		//	uint64 t = Time::Get();
			int16 res = send(sendBuffer, sendBufferPos);
		//	uint64 d = Time::Get() - t;
		//	if (d > 2000) {
		//		printf("********* Network send %u bytes took %u us**********\n\n",
		//			sendBufferPos, (uint32)d);
		//	}
			sendBufferPos = 0;

			return res;
		}

		inline int16	CommChannel::_recv(__Payload	**c,uint8	sourceNID){

			uint64	metaData;
			int16	r;
			uint32	size;
			commRecvCS.enter();
			if(r=recv((uint8	*)&size,sizeof(uint32))) {	//	receive the total size (includes the size of the non transmitted data)
				commRecvCS.leave();
				return	r;
			}
			//Error::PrintBinary((char*)&size, sizeof(uint32), true, "Received Size");
			if(r=recv((uint8	*)&metaData,sizeof(uint64),true)) {	//	receive __Payload::_metaData
				commRecvCS.leave();
				return	r;
			}
			//Error::PrintBinary((char*)&metaData,sizeof(uint64), true, "Received metaData");
			//	allocate and initialize the payload (default ctor is called)
			ClassRegister		*CR=ClassRegister::Get((uint16)(metaData >> 16));
			if(CR==NULL) {
				commRecvCS.leave();
				return	-1;
			}

			printf("Received Class: '%s' [%u] size '%u'...\n", CR->class_name, (uint16)(metaData >> 16), size);

			if(sourceNID!=0xFF){

				uint32	OID=metaData>>32;
				if(OID	&	0x80000000){	//	constant object.

					*c=Node::Get()->getConstantObject(OID);
					commRecvCS.leave();
					return	0;
				}

				if(OID!=0x00FFFFFF){	//	shared object.

					if(Node::Get()->hasLookup(sourceNID,OID)){	//	object is already there and the sender knows: we know that because we have sent it to the sender previously.
																//	no need to recv anything.
						*c=Node::Get()->getSharedObject(OID);
						Node::Get()->consolidate((_Payload	*)*c);	//	handles the case where c has been doomed after being sent by the source node: ressuscitate it if no advertisement has been made yet.
						commRecvCS.leave();
						return	0;
					}

					Node::Get()->addLookup(sourceNID,OID);	//	the sender obviously has the object.
															//	the sender now knows we have it.
				}
			}

			*c=(__Payload*)(*CR->allocator())(size);	//	calls UserDefinedClass::New(size)

			if(r=recv(((uint8	*)*c)+CR->offset(),size-CR->offset())) {	//	metadata only peeked: read from the offset, and not from offset+sizeof(_metaData)
				commRecvCS.leave();
				return	r;
			}

			if(sourceNID!=0xFF){
				
				uint32	OID=(metaData>>32)	&	0x00FFFFFF;
				if(OID!=0x00FFFFFF){	//	shared object..

					_Payload	*s=Node::Get()->getSharedObject(OID);
					if(s){	//	object was already there but the sender didn't know: discard what has been received and use the known object instead.

						delete	c;
						*c=s;
						commRecvCS.leave();
						return	0;
					}

					Node::Get()->addSharedObject((_Payload	*)*c);
				}
			}
			
			uint8		ptrCount=(uint8)(*c)->ptrCount();
			__Payload	*p;
			for(uint8	i=0;i<ptrCount;i++){

				if(r=_recv(&p,sourceNID)){

					delete	*c;
					commRecvCS.leave();
					return	r;
				}
				(*c)->setPtr(i,p);
			}
			(*c)->init();
			commRecvCS.leave();
			return	0;
		}

		int16	CommChannel::send(_Payload	*p,uint8	destinationNID){

		//	p->node_send_ts()=Time::Get();
			return	_send(p,destinationNID);
		}

		int16	CommChannel::recv(_Payload	**p,uint8	sourceNID){

			int16	r;
			if(r=_recv((__Payload	**)p,sourceNID))
				return	r;
		//	(*p)->node_recv_ts()=Time::Get();
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
