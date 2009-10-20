//	network_id.cpp
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

#include	"network_id.h"

#include	<cstring>

namespace	mBrane{

	uint16	NetworkID::DiscoveryIDSize=0;

	uint16	NetworkID::Size=0;
	
	uint16	NetworkID::CtrlIDSize[2]={	0,	0	};
	
	uint16	NetworkID::DataIDSize[2]={	0,	0	};
	
	uint16	NetworkID::StreamIDSize[2]={	0,	0	};
	
	NetworkID::NetworkID():data(NULL){
	}

	NetworkID::NetworkID(uint16	NID,Node::Network	description,uint8	nameSize,char	*name){

		uint8	s=sizeof(uint16)+sizeof(uint8)+sizeof(uint8);
		Size=DiscoveryIDSize;
		headerSize=s+nameSize;
		switch(description){
		case	Node::PRIMARY:	Size+=CtrlIDSize[Node::PRIMARY]+DataIDSize[Node::PRIMARY]+StreamIDSize[Node::PRIMARY];	break;
		case	Node::SECONDARY:	Size+=CtrlIDSize[Node::SECONDARY]+DataIDSize[Node::SECONDARY]+StreamIDSize[Node::SECONDARY];	break;
		case	Node::BOTH:		Size+=CtrlIDSize[Node::PRIMARY]+DataIDSize[Node::PRIMARY]+StreamIDSize[Node::PRIMARY]+CtrlIDSize[Node::SECONDARY]+DataIDSize[Node::SECONDARY]+StreamIDSize[Node::SECONDARY];	break;
		}
		data=new	uint8[headerSize+Size];
		((uint16	*)data)[0]=NID;
		data[sizeof(uint16)]=description;
		data[sizeof(uint16)+sizeof(uint8)]=nameSize;
		memcpy(data+s,name,data[sizeof(uint16)+sizeof(uint8)]);
	}

	NetworkID::~NetworkID(){

		if(data)
			delete	data;
	}

	uint16	NetworkID::NID()	const{

		return	((uint16	*)data)[0];
	}

	uint16	NetworkID::setNID(uint16 newNID)	{
		((uint16	*)data)[0]=newNID;
		return 0;
	}

	Node::Network	NetworkID::network()	const{

		return	(Node::Network)data[sizeof(uint16)];
	}
	
	char	*NetworkID::name()	const{

		return	(char	*)(data+sizeof(uint16)+sizeof(uint8)+sizeof(uint8));
	}

	uint8	*NetworkID::at(InterfaceType	t)	const{

		switch(t){
		case	DISCOVERY:			return	data+headerSize;
		case	CONTROL_PRIMARY:
			switch(data[2]){
			case	Node::BOTH:
			case	Node::PRIMARY:	return	data+headerSize+DiscoveryIDSize;
			case	Node::SECONDARY:	return	NULL;
			}
		case	DATA_PRIMARY:
			switch(data[2]){
			case	Node::BOTH:
			case	Node::PRIMARY:	return	data+headerSize+DiscoveryIDSize+CtrlIDSize[Node::PRIMARY];
			case	Node::SECONDARY:	return	NULL;
			}
		case	STREAM_PRIMARY:
			switch(data[2]){
			case	Node::BOTH:
			case	Node::PRIMARY:	return	data+headerSize+DiscoveryIDSize+CtrlIDSize[Node::PRIMARY]+DataIDSize[Node::PRIMARY];
			case	Node::SECONDARY:	return	NULL;
			}
		case	CONTROL_SECONDARY:
			switch(data[2]){
			case	Node::PRIMARY:	return	NULL;
			case	Node::SECONDARY:	return	data+headerSize+DiscoveryIDSize;
			case	Node::BOTH:		return	data+headerSize+DiscoveryIDSize+CtrlIDSize[Node::PRIMARY]+DataIDSize[Node::PRIMARY]+StreamIDSize[Node::PRIMARY];
			}
		case	DATA_SECONDARY:
			switch(data[2]){
			case	Node::PRIMARY:	return	NULL;
			case	Node::SECONDARY:	return	data+headerSize+DiscoveryIDSize+CtrlIDSize[Node::SECONDARY];
			case	Node::BOTH:		return	data+headerSize+DiscoveryIDSize+CtrlIDSize[Node::PRIMARY]+DataIDSize[Node::PRIMARY]+StreamIDSize[Node::PRIMARY]+CtrlIDSize[Node::SECONDARY];
			}
		case	STREAM_SECONDARY:
			switch(data[2]){
			case	Node::PRIMARY:	return	NULL;
			case	Node::SECONDARY:	return	data+headerSize+DiscoveryIDSize+CtrlIDSize[Node::SECONDARY]+DataIDSize[Node::SECONDARY];
			case	Node::BOTH:		return	data+headerSize+DiscoveryIDSize+CtrlIDSize[Node::PRIMARY]+DataIDSize[Node::PRIMARY]+StreamIDSize[Node::PRIMARY]+CtrlIDSize[Node::SECONDARY]+DataIDSize[Node::SECONDARY];
			}
		}
		return	NULL;
	}
}
