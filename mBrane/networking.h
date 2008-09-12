//	networking.h
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

#ifndef	mBrane_networking_h
#define	mBrane_networking_h

#include	"..\Core\network_interface.h"
#include	"network_id.h"


using	namespace	mBrane::sdk;
using	namespace	mBrane::sdk::daemon;

namespace	mBrane{

	class	RecvThread;
	template<class	Engine>	class	Messaging;
	class	Networking:
	public	daemon::Node{
	friend	class	RecvThread;
	template<class	Engine>	friend	class	Messaging;
		//	boot one single node with a timeout (if it times out, it's the ref node)
		//	when ready (callback), boot all the other nodes
		//	algorithm:
		//		bcast its net ID on discovery channel
		//		accept connections:
		//			if timedout this is ref node, scan IDs on discovery channel
		//			else
		//				the ref node sends (on data channel if control channel is bcast, on control channel otherwise): its own net ID, an assigned NID and the net map (i.e. the list of ready nodes net ID)
		//				connect to each node in the list excepted the sender
		//		if(ref node) send time sync periodically on control channel
		//		start messages sending and receiving threads
		//	when at least one connection to a remote node dies, the node in question is considred dead and the other connections to it are terminated
		//	if the ref node dies, the node with the lowest NID is the new ref node
		//
		//	handles two isolated networks: primary (ex: core computation) and secondary (ex: I/O, signal processing)
		//	network IDs carry the primary, secondary or both identifications
		//	when receiving a bcast id bearing two, connect to the primary only
		//	when sending to a node, use the primary only if two are available
		//	receiving is agnostic
		//
		//	reference nodes must be on the primary network
	protected:
		Host::host_name	hostName;
		uint8			hostNameSize;

		typedef	void	(*BootCallback)();
		SharedLibrary	*callbackLibrary;
		BootCallback	bootCallback;

		DynamicClassLoader<NetworkInterface>	*networkInterfaceLoaders[7];
		NetworkInterface						*networkInterfaces[7];

		Network	network;
		bool	startInterfaces();
		void	stopInterfaces();

		int32	bcastTimeout;	//	in ms

		uint16	connectedNodeCount;

		NetworkID	*networkID;

		class	DataCommChannel{
		public:
			DataCommChannel();
			~DataCommChannel();
			typedef	struct{
				CommChannel	*data;
				CommChannel	*stream;
			}CommChannels;
			CommChannels	channels[2];	//	1 for each network
			NetworkID		*networkID;
		};
		CommChannel					*discoveryChannel;	//	bcast
		Array<CommChannel	*>		controlChannels[2];	//	for each network: 1 (bcast capable) or many (connected)
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
			Networking			*node;
			int32				timeout;
			Network				network;
			_Payload::Category	category;
		}AcceptConnectionArgs;
		static	uint32	thread_function_call	AcceptConnections(void	*args);
		static	uint32	thread_function_call	Sync(void	*args);
		int64	timeDrift;	//	in ms
		int64	syncPeriod;	//	in ms

		uint16	sendID(CommChannel	*c,NetworkID	*networkID);
		uint16	recvID(CommChannel	*c,NetworkID	*&networkID);
		uint16	sendMap(CommChannel	*c);
		uint16	recvMap(CommChannel	*c);
		uint16	connect(NetworkID	*networkID);
		uint16	connect(Network	network,NetworkID	*networkID);
		void	_broadcastControlMessage(_Payload	*p,Network	network);
		void	broadcastControlMessage(_Payload	*p,Network	network);
		void	sendData(uint16	NID,_Payload	*p,Network	network);
		void	sendStreamData(uint16	NID,_Payload	*p,Network	network);
		void	processError(uint16	entry);	//	upon send/recv error. Disconnect the node on both networks
		uint16	addNodeEntry();

		bool	init();
		virtual	void	start(uint16	assignedNID,NetworkID	*networkNID,bool	isTimeReference);

		Networking();
		~Networking();
		bool	loadInterface(XMLNode	&interfaces,XMLNode	&config,const	char	*name,InterfaceType	type);
		bool	loadConfig(XMLNode	&n);
	};
}


#endif