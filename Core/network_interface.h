//	network_interface.h
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

#ifndef	mBrane_sdk_network_interface_h
#define	mBrane_sdk_network_interface_h

#include	"utils.h"
#include	"xml_parser.h"
#include	"mdaemon_node.h"


namespace	mBrane{
	namespace	sdk{

		class	CommChannel;
		class	ConnectedCommChannel;
		class	mBrane_dll	NetworkInterface{
		public:
			typedef	enum{
				UDP=0,
				TCP=1,
				RM=2,
				IB=3	//	MPI2
			}Protocol;
		protected:
			Protocol	_protocol;
			NetworkInterface(Protocol	_protocol);
		public:
			typedef	NetworkInterface	*(*Load)(XMLNode	&,mdaemon::Node	*n);	//	function exported by the shared library
			virtual	~NetworkInterface();
			Protocol	protocol()	const;
			virtual	bool	operator	==(NetworkInterface	&i)=0;
			virtual	bool	operator	!=(NetworkInterface	&i)=0;
			virtual	bool	canBroadcast()=0;	//	as opposed to connected mode
			virtual	uint16	start()=0;	//	initialize the network interface; returns 0 if successful
			virtual	uint16	stop()=0;	//	the network interface; returns 0 if successful
			virtual	uint16	getIDSize()=0;	//	node net ID to be broadcast
			virtual	void	fillID(uint8	*ID)=0;	//	with relevant parameters (different from Node::_ID; ex: IP addr and port)
			virtual	uint16	newChannel(uint8	*ID,CommChannel	**channel)=0;	//	create a new channel (bcast ID=local node net ID, or connected ID=remote node net ID); returns 0 if successful
			virtual	uint16	acceptConnection(ConnectedCommChannel	**channel,int32	timeout,bool	&timedout)=0;	//	listen to connect attempts and creates a new channel accordingly; returns 0 if successful
		};

		class	_Payload;
		class	mBrane_dll	CommChannel{
		protected:
			CommChannel();	//	initialization to be performed in subclasses' constructors
			int16	_send(__Payload	*c,uint8	destinationNID);
			int16	_recv(__Payload	**c,uint8	sourceNID);
		public:
			virtual	~CommChannel();	//	shutdown to be performed in subclasses' destructors
			virtual	int16	send(uint8	*b,size_t	s)=0;	//	return 0 if successfull, error code (>0) otherwise
			virtual	int16	recv(uint8	*b,size_t	s,bool	peek=false)=0;
			int16	send(_Payload	*p,uint8	destinationNID);	//	return 0 if successfull, error code (>0) otherwise. destinationNID used only for connected comm channels.
			int16	recv(_Payload	**p,uint8	sourceNID);
		};

		class	mBrane_dll	ConnectedCommChannel:
		public	CommChannel{
		protected:
			ConnectedCommChannel();
		public:
			virtual	~ConnectedCommChannel();
		};

		class	mBrane_dll	BroadcastCommChannel:
		public	CommChannel{
		protected:
			BroadcastCommChannel();
		public:
			virtual	~BroadcastCommChannel();
		};
	}
}


#endif
