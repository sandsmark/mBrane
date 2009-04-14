//	network_interface.tpl.cpp
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

namespace	mBrane{
	namespace	sdk{

		template<class	C>	int16	CommChannel::_send(C	*c){

			ClassRegister	*CR=ClassRegister::Get(c->cid());
			int16	r;
			AllocationScheme	a=c->allocationScheme();
			if(a>=DYNAMIC){
				
				if(a==COMPRESSED	&&	!c->operator	payloads::_CompressedData	*()->isCompressed)
					c->operator	payloads::_CompressedData	*()->compress();
				if(r=send(((uint8	*)c)+CR->offset(),CR->coreSize()+c->operator	payloads::_DynamicData	*()->dynamicSize()))
					return	r;
			}else	if(r=send(((uint8	*)c)+CR->offset(),CR->size()))
				return	r;
			uint8	ptrCount=CR->ptrCount();
			P<_RPayload>	*p;
			for(uint8	i=0;i<ptrCount;i++){

				p=CR->ptr(c,i);
				if(!*p)
					continue;
				if(r=_send((_RPayload	*)*p))
					return	r;
			}
			return	0;
		}

		template<class	C>	int16	CommChannel::_recv(C	**c){

			uint16	cid;
			int16	r;
			if(r=recv((uint8	*)&cid,sizeof(uint16),true))
				return	r;
			ClassRegister	*CR=ClassRegister::Get(cid);
			*c=(C	*)CR->allocator();
			AllocationScheme	a=(*c)->allocationScheme();
			if(a>=DYNAMIC){

				if(r=recv((uint8	*)*c,CR->coreSize())){

					delete	*c;
					return	r;
				}
				if(r=recv(((uint8	*)*c)+CR->coreSize(),(*c)->operator	payloads::_DynamicData	*()->dynamicSize())){

					delete	*c;
					return	r;
				}
				if(a==COMPRESSED)
					(*c)->operator	payloads::_CompressedData	*()->decompress();
			}else	if(r=recv((uint8	*)*c,CR->size())){

				delete	*c;
				return	r;
			}
			uint8	ptrCount=CR->ptrCount();
			P<_RPayload>	*p;
			_RPayload		*ptr;
			for(uint8	i=0;i<ptrCount;i++){

				p=CR->ptr(*c,i);
				if(!*p)
					continue;
				if(r=_recv(&ptr)){

					delete	*c;
					return	r;
				}
				*p=ptr;
			}
			(*c)->init();
			return	0;
		}
	}
}