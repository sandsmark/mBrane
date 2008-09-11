//	network_id.h
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

#ifndef	mBrane_network_id_h
#define	mBrane_network_id_h

#include	"..\Core\module_node.h"


using	namespace	mBrane::sdk::module;

namespace	mBrane{

	typedef	enum{
		CONTROL_PRIMARY=0,
		DATA_PRIMARY=1,
		STREAM_PRIMARY=2,
		CONTROL_SECONDARY=3,
		DATA_SECONDARY=4,
		STREAM_SECONDARY=5,
		DISCOVERY=6
	}InterfaceType;

	class	NetworkID{	//	total size depends on network: headerSize+Size[PRIMARY]+Size[SECONDARY] or headerSize+Size[PRIMARY] or headerSize+Size[SECONDARY]
	public:
		static	uint16	DiscoveryIDSize;
		static	uint16	Size;
		static	uint16	CtrlIDSize[2];	//	1 for each network
		static	uint16	DataIDSize[2];
		static	uint16	StreamIDSize[2];
		uint8	headerSize;	//	sizeof(NID)+sizeof(name size)+name size+sizeof(network)
		uint8	*data;	//	[NID(16)|network(8)|name size(8)|name(name size*8)|discovery ID|control ID|data ID|stream ID|(control ID|data ID|stream ID) optional]
		NetworkID();
		NetworkID(uint16	NID,mBrane::sdk::module::Node::Network	description,uint8	nameSize,char	*name);
		~NetworkID();
		uint16								NID()	const;
		mBrane::sdk::module::Node::Network	network()	const;
		char								*name()	const;
		uint8	*at(InterfaceType	t)		const;
	};
}


#endif