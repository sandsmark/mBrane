// udp.h
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

#ifndef	mBrane_udp_h
#define	mBrane_udp_h

#include	"..\Core\types.h"
#include	"..\Core\xml_parser.h"
#include	"..\Core\network_interface.h"

using	namespace	mBrane;
using	namespace	mBrane::sdk;


extern	"C"{
uint64	dll_export	RTT();	//	round trip time estimate
bool	dll_export	CanBroadcast();	//	as opposed to connected mode
bool	dll_export	Start(XMLNode	&n);	//	initialize the network interface; loads parameters from XML file
void	dll_export	Stop();	//	the network interface
uint32	dll_export	GetIDSize();	//	node ID to be broadcast
void	dll_export	FillID(uint8	*ID);	//	with relevant parameters;
uint16	dll_export	BroadcastID(uint8	*ID,uint32	size);	//	broadcast the ID of the local node
uint16	dll_export	ScanID(uint8	*ID,uint32	size);	//	listen to IDs broadcast by remote nodes
uint16	dll_export	Connect(uint8	*ID,ConnectedCommChannel	*&channel);	//	create a new channel from the received remote IDs (ScanID)
uint16	dll_export	AcceptConnection(ConnectedCommChannel	*&channel);	//	listen
}


#endif