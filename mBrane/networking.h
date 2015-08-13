//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
//_/_/
//_/_/ HUMANOBS - mBrane
//_/_/
//_/_/ Eric Nivel
//_/_/ Center for Analysis and Design of Intelligent Agents
//_/_/   Reykjavik University, Menntavegur 1, 101 Reykjavik, Iceland
//_/_/   http://cadia.ru.is
//_/_/ Copyright(c)2012
//_/_/
//_/_/ This software was developed by the above copyright holder as part of 
//_/_/ the HUMANOBS EU research project, in collaboration with the 
//_/_/ following parties:
//_/_/ 
//_/_/ Autonomous Systems Laboratory
//_/_/   Technical University of Madrid, Spain
//_/_/   http://www.aslab.org/
//_/_/
//_/_/ Communicative Machines
//_/_/   Edinburgh, United Kingdom
//_/_/   http://www.cmlabs.com/
//_/_/
//_/_/ Istituto Dalle Molle di Studi sull'Intelligenza Artificiale
//_/_/   University of Lugano and SUPSI, Switzerland
//_/_/   http://www.idsia.ch/
//_/_/
//_/_/ Institute of Cognitive Sciences and Technologies
//_/_/   Consiglio Nazionale delle Ricerche, Italy
//_/_/   http://www.istc.cnr.it/
//_/_/
//_/_/ Dipartimento di Ingegneria Informatica
//_/_/   University of Palermo, Italy
//_/_/   http://roboticslab.dinfo.unipa.it/index.php/Main/HomePage
//_/_/
//_/_/
//_/_/ --- HUMANOBS Open-Source BSD License, with CADIA Clause v 1.0 ---
//_/_/
//_/_/ Redistribution and use in source and binary forms, with or without 
//_/_/ modification, is permitted provided that the following conditions 
//_/_/ are met:
//_/_/
//_/_/ - Redistributions of source code must retain the above copyright 
//_/_/ and collaboration notice, this list of conditions and the 
//_/_/ following disclaimer.
//_/_/
//_/_/ - Redistributions in binary form must reproduce the above copyright 
//_/_/ notice, this list of conditions and the following
//_/_/ disclaimer in the documentation and/or other materials provided 
//_/_/ with the distribution.
//_/_/
//_/_/ - Neither the name of its copyright holders nor the names of its 
//_/_/ contributors may be used to endorse or promote products 
//_/_/ derived from this software without specific prior written permission.
//_/_/
//_/_/ - CADIA Clause: The license granted in and to the software under this 
//_/_/ agreement is a limited-use license. The software may not be used in 
//_/_/ furtherance of: 
//_/_/ (i) intentionally causing bodily injury or severe emotional distress 
//_/_/ to any person; 
//_/_/ (ii) invading the personal privacy or violating the human rights of 
//_/_/ any person; or 
//_/_/ (iii) committing or preparing for any act of war.
//_/_/
//_/_/ THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
//_/_/ "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
//_/_/ LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR 
//_/_/ A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
//_/_/ OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
//_/_/ SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
//_/_/ LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, 
//_/_/ DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY 
//_/_/ THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
//_/_/ (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
//_/_/ OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//_/_/
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/ 

#ifndef	mBrane_networking_h
#define	mBrane_networking_h

#include	"../Core/network_interface.h"
#include	"network_id.h"

#include	"pipe.h"
#include	"../Core/list.h"
#include	"../Core/control_messages.h"
#include	"messaging.h"

using	namespace	mBrane::sdk;
using	namespace	mBrane::sdk::mdaemon;

namespace	mBrane{

	#define CONTROL_PRIMARY_INITIALISED		0x0001
	#define DATA_PRIMARY_INITIALISED		0x0002
	#define STREAM_PRIMARY_INITIALISED		0x0004
	#define CONTROL_SECONDARY_INITIALISED	0x0008
	#define DATA_SECONDARY_INITIALISED		0x0010
	#define STREAM_SECONDARY_INITIALISED	0x0020
	#define CONTROL_PRIMARY_CONNECTED		0x0040
	#define DATA_PRIMARY_CONNECTED			0x0080
	#define STREAM_PRIMARY_CONNECTED		0x0100
	#define CONTROL_SECONDARY_CONNECTED		0x0200
	#define DATA_SECONDARY_CONNECTED		0x0400
	#define STREAM_SECONDARY_CONNECTED		0x0800

	class	NodeCon;
	struct ReceiveThreadInfo {
		NodeCon* con;
		CommChannel* channel;
	};

	class	Networking;
	class	NodeCon {
	public:
		NodeCon(Networking* node);
		virtual ~NodeCon();

		bool setSourceNID(uint8 sourceNID);
		bool setName(const char* name);

		bool isInUse();
		uint32 getConnectionStatus();
		bool isConnected(module::Node::Network network = module::Node::EITHER);
		bool disconnect();

		bool startNetworkChannel(CommChannel* c, uint8 type, bool isCopy = false);
		CommChannel* getNetworkChannel(uint8 type);

		Networking		*node;
		NetworkID		*networkID;
		uint8_t 		sourceNID;
		char*			name;
		bool			joined;
		bool			ready;

		Array<CommChannel	*,6>		commChannels;
		Array<Thread	*,6>	commThreads;
		Thread	*				pushThread;

		Pipe11<P<_Payload>,MESSAGE_INPUT_BLOCK_SIZE>	buffer;	//	incoming messages from remote nodes
		static	thread_ret thread_function_call	ReceiveMessages(void	*args);
		static	thread_ret thread_function_call	PushJobs(void	*args);
	};

	class	Messaging;
	//	Handles network initialization and connection.
	//	Handles two isolated networks: primary (ex: core computation) and secondary (ex: I/O, signal processing)
	//	Network IDs carry the primary, secondary or both identifications
	//	When receiving a bcast id bearing two, connect to the primary only
	//	When sending to a node, use the primary only if two are available
	//	Receiving is agnostic
	//
	//	Reference nodes must be on the primary network
	//
	//	Node boot sequence:
	//
	//		1 boot one single node with a timeout (if it times out, it's the ref node)
	//		2 when ready (callback), boot all the other nodes
	//
	//	Algorithm for node connection:
	//
	//		bcast its net ID on discovery channel
	//		accept connections:
	//			if timedout this is ref node, scan IDs on discovery channel
	//			else
	//				the ref node sends (on data channel if control channel is bcast, on control channel otherwise): its own net ID, an assigned NID and the net map (i.e. the list of ready nodes net ID)
	//				connect to each node in the list excepted the sender
	//		if(ref node) send time sync periodically on control channel
	//		start messages sending and receiving threads
	//
	//	When at least one connection to a remote node dies, the node in question is considred dead and the other connections to it are terminated
	//	if the ref node dies, the node with the lowest NID is the new ref node
	class	mbrane_dll	Networking:
	public	mdaemon::Node, public	Messaging
	{
	friend	class	Messaging;
	friend	class	RecvThread;
//	friend	class	Messaging;
	friend	class	NodeCon;
	protected:
		Host::host_name	hostName;
		uint8_t 		hostNameSize;

		typedef	void	(*BootCallback)();
		SharedLibrary	*callbackLibrary;
		BootCallback	bootCallback;

		DynamicClassLoader<NetworkInterface>	*networkInterfaceLoaders[7];
		NetworkInterface						*networkInterfaces[7];

		Network	network;
		CriticalSection	acceptConnectionCS;

		bool	startInterfaces();
		void	stopInterfaces();

		int32_t bcastTimeout;	//	in ms

		uint8_t connectedNodeCount;

		NetworkID	*networkID;

		//class	DataCommChannel{
		//public:
		//	DataCommChannel();
		//	~DataCommChannel();
		//	typedef	struct{
		//		CommChannel	*data;
		//		CommChannel	*stream;
		//	}CommChannels;
		//	CommChannels	channels[2];	//	1 for each network
		//	NetworkID		*networkID;
		//};
		CommChannel									*discoveryChannel;	//	bcast
		CommChannel									*broadcastChannel[2];	//	bcast
//		Array<CommChannel	*,32>					controlChannels[2];	//	for each network: 1 (bcast capable) or many (connected)
//		Array<DataCommChannel	*,32,ArrayManaged>	dataChannels;
		CriticalSection								channelsCS;	//	protects controlChannels and dataChannels
		UNORDERED_MAP<uint8, NodeCon*>				nodes;

		bool	isTimeReference;
		uint8_t referenceNID;
		void	setNewReference();

		virtual	void	startReceivingThreads(uint8_t NID)=0;
		virtual	void	notifyNodeJoined(uint8_t NID,NetworkID	*networkID)=0;
		virtual	void	notifyNodeLeft(uint8_t NID)=0;
		virtual	void	shutdown();

		Array<Thread	*,32>	commThreads;

		bool checkSyncProbe(uint8 syncNodeID);
		void systemReady();

		uint8 nodeCount;
		bool addNodeName(const char* name, bool myself = false);
		uint8 getNodeID(const char* name);
		bool allNodesJoined();
		bool allNodesReady();

		static	thread_ret thread_function_call	ScanIDs(void	*args);
		typedef	struct{
			Networking			*node;
			int32_t 			timeout;
			Network				network;
			_Payload::Category	category;
		}AcceptConnectionArgs;
		static	thread_ret thread_function_call	AcceptConnections(void	*args);
		static	thread_ret thread_function_call	Sync(void	*args);
		int64_t timeDrift;	//	in ms
		int64_t syncPeriod;	//	in ms

		uint16_t sendID(CommChannel	*c,NetworkID	*networkID);
		uint16_t recvID(CommChannel	*c,NetworkID	*&networkID, bool expectToken = true);
		uint16_t sendMap(CommChannel	*c);
		uint16_t recvMap(CommChannel	*c, NetworkID	*fromNetworkID);
		uint16_t connect(NetworkID	*networkID);
		uint16_t connect(Network	network,NetworkID	*networkID);
		void	_broadcastControlMessage(_Payload	*p,Network	network);
		void	broadcastControlMessage(_Payload	*p,Network	network);
		void	_sendControlMessage(_Payload	*p,uint8_t destinationNID,Network	network);
		void	sendControlMessage(_Payload	*p,uint8_t destinationNID,Network	network);
		void	sendData(uint8_t NID,_Payload	*p,Network	network);
		void	sendStreamData(uint8_t NID,_Payload	*p,Network	network);
		void	processError(uint8_t NID);	//	upon send/recv error. Disconnect the node on both networks
		uint8_t addNodeEntry();

		bool	init();
		virtual	void	start(uint8_t assignedNID,NetworkID	*networkNID,bool	isTimeReference);
		bool	startSync();

		Networking();
		~Networking();
		bool	loadInterface(XMLNode	&interfaces,XMLNode	&config,const	char	*name,InterfaceType	type);
		bool	loadConfig(XMLNode	&n);
	};
}


#endif
