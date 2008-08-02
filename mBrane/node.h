// node.h
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

#ifndef	mBrane_node_h
#define	mBrane_node_h

#include	"..\Core\node.h"
#include	"..\Core\network_interface.h"
#include	"..\Core\message.h"
#include	"..\Core\array.h"
#include	"..\Core\circular_buffer.h"
#include	"..\Core\utils.h"


//#define	MESSAGE_PRIORITY_LEVELS	128

namespace	mBrane{

	class	Node:
	public	sdk::Node{
	private:
		const	char	*application_configuration_file;
		SharedLibrary	*userLibrary;

		char	hostName[255];
		uint8	hostNameSize;

		static	uint32	thread_function_call	ScanIDs(void	*args);
		static	uint32	thread_function_call	AcceptConnections(void	*args);

		typedef	enum{
			CONTROL=0,
			DATA=1,
			STREAM=2
		}NetworkInterfaceType;

		//	Nodes must boot in sequence (Cf boot delay in main)
		//	accept connections
		//		bcast ID and name
		//		if accept timedout
		//			node is time reference
		//		else
		//			recv remote NID, name and assigned NID
		//			if assigned NID!=NO_ID
		//				remote NID=reference NID
		//	scan IDs and connect in reply to bcast: send local NID, name and assigned NID (if is time reference or NO_ID otherwise)
		int32	bcastTimeout;
		uint8	*networkID;
		uint32	network_ID_size;
		uint32	network_ctrl_ID_size;
		uint32	network_data_ID_size;
		uint32	network_stream_ID_size;
		sdk::NetworkInterfaceLoader	*networkDiscoveryInterfaceLoader;
		sdk::NetworkInterface		*networkDiscoveryInterface;
		sdk::NetworkInterfaceLoader	*networkCommInterfaceLoaders[3];
		sdk::NetworkInterface		*networkCommInterfaces[3];
		void	init(uint16	NID);

		bool	startInterfaces();
		void	stopInterfaces();

		class	DataCommChannel{
		public:
			DataCommChannel();
			~DataCommChannel();
			sdk::ConnectedCommChannel	*data;
			sdk::ConnectedCommChannel	*stream;
			char	*name;
			uint8	nameSize;
		};
		sdk::Array<sdk::CommChannel	*>	controlChannels;	//	1 (bcast capable) or many (connected)
		sdk::Array<DataCommChannel	*>	dataChannels;
		Mutex	m;	//	protects controlChannels and dataChannels

		bool	isTimeReference;
		uint16	referenceNID;
		void	setNewReference();
		void	notifyNodeJoined(uint16	NID,char	*name);
		void	notifyNodeLeft(uint16	NID);

		static	uint32	thread_function_call	Sync(void	*args);
		int64	timeDrift;	//	in ms
		int64	lastSyncTime;	//	in ms
		int64	syncPeriod;	//	in ms

		typedef	struct{
			Node		*n;
			sdk::_Crank	*c;
		}CrankThreadArgs;
		static	uint32	thread_function_call	CrankExecutionUnit(void	*args);	//	First step: 1 crank<->1 thread and evaluate performance; TODO (if first step satisfying):	1 thread<->many cranks => ctrl at instruction (asm) level...
		sdk::CircularBuffer<sdk::P<sdk::_Payload> >	timeGate;	//	First step: time granularity=0; TODO: increase granularity (2 ms) if possible and useful. In that case, typedef	sdk::CircularBuffer<sdk::P<sdk::_Payload> >	MessageBuffer[MESSAGE_PRIORITY_LEVELS]; sdk::CircularBuffer<MessageBuffer>	timeGate; maintain a time latch (2ms)

		void	processError(NetworkInterfaceType	type,uint16	entry);
		uint16	addNode();
		typedef	struct{
			Node					*n;
			sdk::CommChannel		*c;
			uint16					e;
			NetworkInterfaceType	t;
		}ReceiveThreadArgs;
		static	uint32	thread_function_call	ReceiveMessages(void	*args);
		static	uint32	thread_function_call	SendMessages(void	*args);
		static	uint32	thread_function_call	NotifyMessages(void	*args);
		//	TODO:	define pub-sub routing structures

		sdk::Array<Thread	*>	commThreads;
		sdk::Array<Thread	*>	crankThreads;
		bool	_shutdown;

		Node();
		Node	*loadConfig(const	char	*configFileName);

		void	sendLocal(sdk::_Payload	*message);
		void	sendLocal(const	sdk::_Crank	*sender,sdk::_Payload	*message);
		void	sendTo(uint16	NID,sdk::_Payload	*message);
	public:
		static	Node	*New(const	char	*configFileName);
		~Node();
		void	run();
		void	shutdown();
		void	dump(const	char	*fileName);	//	dumps the current system state; crank dump fileNames: crank_class_ID.bin: ex: CR1_123.bin
		void	load(const	char	*fileName);	//	initializes itself from a previously saved system state
		Node	*loadApplication(const	char	*fileName=NULL);	//	return NULL if unsuccessful; fileName overrides the fileName found in the node config file
		void	unloadApplication();
		void	send(const	sdk::_Crank	*sender,sdk::_Payload	*message);
		int64	time()	const;	//	in ms since 01/01/70
		sdk::_Crank	*buildCrank(uint16	CID);
		void	start(sdk::_Crank	*c);
		void	stop(sdk::_Crank	*c);
	};
}


#endif
