// network_interface.h
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

#ifndef	mBrane_sdk_network_interface_h
#define	mBrane_sdk_network_interface_h

#include	"utils.h"
#include	"xml_parser.h"


namespace	mBrane{
	namespace	sdk{

		class	BroadcastCommChannel;
		class	ConnectedCommChannel;
		class	dll	NetworkInterface{
		public:
			typedef	uint64	(*RTT)();	//	round trip time estimate
			typedef	bool	(*CanBroadcast)();	//	as opposed to connected mode
			typedef	uint16	(*Start)(XMLNode	&);	//	initialize the network interface; loads parameters from XML file; returns 0 if successful
			typedef	uint16	(*Stop)();	//	the network interface; returns 0 if successful
			typedef	uint32	(*GetIDSize)();	//	node ID to be broadcast
			typedef	void	(*FillID)(uint8	*);	//	with relevant parameters (different from Node::_ID; ex: IP addr and port)
			typedef	uint16	(*BroadcastID)(uint8	*,uint32);	//	broadcast the ID of the local node; returns 0 if successful
			typedef	uint16	(*ScanID)(uint8	*,uint32);	//	listen to IDs broadcast by remote nodes; returns 0 if successful
			typedef	uint16	(*Bind)(uint8	*,BroadcastCommChannel	*&);	//	create a new channel from the received remote IDs (ScanID); returns 0 if successful
			typedef	uint16	(*Connect)(uint8	*,ConnectedCommChannel	*&);	//	create a new channel from the received remote IDs (ScanID); returns 0 if successful
			typedef	uint16	(*AcceptConnection)(ConnectedCommChannel	*&,int32,bool	&);	//	listen to connect attempts and creates a new channel accordingly; returns 0 if successful
		protected:
			SharedLibrary	*library;
			NetworkInterface();
			virtual	NetworkInterface	*load(XMLNode	&n);
		public:
			template<class	N>	static	N	*New(XMLNode	&n);
			virtual	~NetworkInterface();
			Start	start;
			Stop	stop;			
		};

		class	dll	NetworkDiscoveryInterface:
		public	NetworkInterface{
		friend	class	NetworkInterface;
		protected:
			NetworkInterface	*load(XMLNode	&n);
		public:
			NetworkDiscoveryInterface();
			~NetworkDiscoveryInterface();
			BroadcastID	broadcastID;
			ScanID		scanID;
		};

		class	dll	NetworkCommInterface:
		public	NetworkInterface{
		friend	class	NetworkInterface;
		protected:
			NetworkInterface	*load(XMLNode	&n);
		public:
			NetworkCommInterface();
			~NetworkCommInterface();
			RTT					rtt;
			CanBroadcast		canBroadcast;
			GetIDSize			getIDSize;
			FillID				fillID;
			Bind				bind;
			Connect				connect;
			AcceptConnection	acceptConnection;
		};

		class	_Payload;
		class	dll	CommChannel{
		protected:
			CommChannel();	//	initialization to be performed in subclasses' constructors
		public:
			virtual	~CommChannel();	//	shutdown to be performed in subclasses' destructors
			virtual	int16	send(uint8	*b,size_t	s)=0;	//	return 0 if successfull, error code (>0) otherwise
			virtual	int16	recv(uint8	*b,size_t	s,bool	peek=false)=0;
			int16	send(_Payload	*p);	//	return 0 if successfull, error code (>0) otherwise
			int16	recv(_Payload	**p);
		};

		class	dll	ConnectedCommChannel:
		public	CommChannel{
		private:
			const	uint16	remoteNID;
		protected:
			ConnectedCommChannel(uint16	remoteNID);
		public:
			virtual	~ConnectedCommChannel();
			const	uint16	nid()	const;
		};

		class	dll	BroadcastCommChannel:
		public	CommChannel{
		protected:
			BroadcastCommChannel();
		public:
			virtual	~BroadcastCommChannel();
		};
	}
}


#include	"network_interface.tpl.cpp"


#endif
