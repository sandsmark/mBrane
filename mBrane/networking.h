// networking.h
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

#ifndef	mBrane_networking_h
#define	mBrane_networking_h

#include	"..\Core\network_interface.h"


using	namespace	mBrane::sdk;
using	namespace	mBrane::sdk::daemon;

namespace	mBrane{

	class	Networking:
	public	Node{
	protected:
		char	hostName[255];
		uint8	hostNameSize;

		typedef	enum{
			CONTROL=0,
			DATA=1,
			STREAM=2,
			DISCOVERY=3
		}NetworkInterfaceType;

		DynamicClassLoader<NetworkInterface>	*networkInterfaceLoaders[4];
		NetworkInterface						*networkInterfaces[4];

		int32	bcastTimeout;
		uint8	*networkID;
		uint32	network_ID_size;
		uint32	network_ctrl_ID_size;
		uint32	network_data_ID_size;
		uint32	network_stream_ID_size;

		bool	startInterfaces();
		void	stopInterfaces();

		class	DataCommChannel{
		public:
			DataCommChannel();
			~DataCommChannel();
			ConnectedCommChannel	*data;
			ConnectedCommChannel	*stream;
			char	*name;
			uint8	nameSize;
		};
		Array<CommChannel	*>		controlChannels;	//	1 (bcast capable) or many (connected)
		Array<DataCommChannel	*>	dataChannels;
		CriticalSection				channelsCS;	//	protects controlChannels and dataChannels

		bool	isTimeReference;
		uint16	referenceNID;
		void	setNewReference();

		virtual	void	startReceivingThreads(uint16	NID)=0;
		virtual	void	notifyNodeJoined(uint16	NID,char	*name)=0;
		virtual	void	notifyNodeLeft(uint16	NID)=0;
		virtual	void	shutdown();

		Array<Thread	*>	commThreads;

		static	uint32	thread_function_call	ScanIDs(void	*args);
		typedef	struct{
			Networking				*n;
			int32					t;
			NetworkInterfaceType	type;
		}AcceptConnectionArgs;
		static	uint32	thread_function_call	AcceptConnections(void	*args);
		static	uint32	thread_function_call	Sync(void	*args);
		int64	timeDrift;	//	in ms
		int64	lastSyncTime;	//	in ms
		int64	syncPeriod;	//	in ms

		uint16	sendID(ConnectedCommChannel	*c,uint16	assignedNID);
		uint16	recvID(ConnectedCommChannel	*c,uint16	&NID,char	*&name,uint8	&nameSize,uint16	&assignedNID);
		void	processError(NetworkInterfaceType	type,uint16	entry);
		uint16	addNodeEntry();

		bool	init();
		virtual	void	start(uint16	assignedNID,uint16	remoteNID,bool	isTimeReference);

		Networking();
		~Networking();
		bool	loadInterface(XMLNode	&n,const	char	*name,NetworkInterfaceType	type);
		bool	loadConfig(XMLNode	&n);
	};
}


#endif