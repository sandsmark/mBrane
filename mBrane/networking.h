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
		//	boot one single node with a timeout (if it times out, it's the ref node)
		//	when ready (callback), boot all the other nodes
		//	algorithm:
		//		bcast its net ID
		//		accept connections:
		//			if timedout this is ref node, scan IDs
		//			else
		//				the ref node sends: an assigned NID and the net map (i.e. the list of ready nodes NID and net ID, including the sender's)
		//				connect to each node in the list excepted the sender
		//		if(ref node) send time sync periodically
		//		start messages sending and receiving threads
		//	when at least one connection to a remote node dies, the node in question is considred dead and the other connections to it are terminated
		//	if the ref node dies, the node with the lowest NID is the new ref node
	protected:
		char	hostName[255];
		uint8	hostNameSize;

		typedef	void	(*BootCallback)();
		SharedLibrary	*callbackLibrary;
		BootCallback	bootCallback;

		typedef	enum{
			CONTROL=0,
			DATA=1,
			STREAM=2,
			DISCOVERY=3
		}NetworkInterfaceType;

		DynamicClassLoader<NetworkInterface>	*networkInterfaceLoaders[4];
		NetworkInterface						*networkInterfaces[4];

		int32	bcastTimeout;

		uint16	connectedNodeCount;

		class	NetworkID{
		public:
			static	uint16	Size;
			static	uint16	CtrlIDSize;
			static	uint16	DataIDSize;
			static	uint16	StreamIDSize;
			static	uint16	DiscoveryIDSize;
			uint8	headerSize;
			uint8	*data;
			NetworkID();
			NetworkID(uint16	NID,uint8	nameSize,char	*name);
			~NetworkID();
			uint16	NID()	const;
			char	*name()	const;
			uint8	*at(NetworkInterfaceType	t)	const;
		};
		NetworkID	*networkID;

		bool	startInterfaces();
		void	stopInterfaces();

		class	DataCommChannel{
		public:
			DataCommChannel();
			~DataCommChannel();
			ConnectedCommChannel	*data;
			ConnectedCommChannel	*stream;
			NetworkID				*networkID;
		};
		BroadcastCommChannel		*discoveryChannel;
		Array<CommChannel	*>		controlChannels;	//	1 (bcast capable) or many (connected)
		Array<DataCommChannel	*>	dataChannels;
		CriticalSection				channelsCS;	//	protects controlChannels and dataChannels

		bool	isTimeReference;
		uint16	referenceNID;
		void	setNewReference();

		virtual	void	startReceivingThreads(uint16	NID)=0;
		virtual	void	notifyNodeJoined(uint16	NID,NetworkID	*networkID)=0;
		virtual	void	notifyNodeLeft(uint16	NID)=0;
		virtual	void	shutdown();

		Array<Thread	*>	commThreads;

		static	uint32	thread_function_call	ScanIDs(void	*args);
		typedef	struct{
			Networking				*node;
			int32					timeout;
			NetworkInterfaceType	type;
		}AcceptConnectionArgs;
		static	uint32	thread_function_call	AcceptConnections(void	*args);
		static	uint32	thread_function_call	Sync(void	*args);
		int64	timeDrift;	//	in ms
		int64	lastSyncTime;	//	in ms
		int64	syncPeriod;	//	in ms

		uint16	sendID(CommChannel	*c,NetworkID	*networkID);
		uint16	recvID(CommChannel	*c,NetworkID	*&networkID);
		uint16	sendMap(ConnectedCommChannel	*c);
		uint16	recvMap(ConnectedCommChannel	*c);
		uint16	connect(NetworkID	*networkID);
		void	processError(NetworkInterfaceType	type,uint16	entry);
		uint16	addNodeEntry();

		bool	init();
		virtual	void	start(uint16	assignedNID,NetworkID	*networkNID,bool	isTimeReference);

		Networking();
		~Networking();
		bool	loadInterface(XMLNode	&n,const	char	*name,NetworkInterfaceType	type);
		bool	loadConfig(XMLNode	&n);
	};
}


#endif