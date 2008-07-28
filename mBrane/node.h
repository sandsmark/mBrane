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

#ifndef	mBrane_node_node_h
#define	mBrane_node_node_h

#include	"..\Core\node.h"
#include	"..\Core\connection.h"


namespace	mBrane{
	namespace	node{

		class	Node:
		public	sdk::Node{
		private:
			const	char	*application_configuration_file;
			shared_object	userLibrary;
			void	loadUserLibrary(const	char	*fileName);
			void	unloadUserLibrary();

			static	uint32	thread_function	ScanIDs(void	*args);
			static	uint32	thread_function	AcceptConnections(void	*args);
			
			typedef	struct{
				sdk::Connection::Init			init;
				sdk::Connection::Shutdown		shutdown;
				sdk::Connection::BroadcastID	broadcastID;
				sdk::Connection::ScanID			scanID;
			}NetworkDiscoveryInterface;

			typedef	struct{
				sdk::Connection::CanBroadcast		canBroadcast;
				sdk::Connection::Init				init;
				sdk::Connection::Shutdown			shutdown;
				sdk::Connection::GetIDSize			getIDSize;
				sdk::Connection::FillID				fillID;
				sdk::Connection::Connect			connect;
				sdk::Connection::AcceptConnection	acceptConnection;
			}NetworkInterface;

			NetworkDiscoveryInterface	networkDiscoveryInterface;
			typedef	enum{
				CTRL=0,
				DATA=1,
				STREAM=2
			}NetworkInterfaceType;
			NetworkInterface	networkInterfaces[3];

			typedef	struct{
				sdk::Connection	*data;
				sdk::Connection	*stream;
			}DataChannel;

			DataChannel	*dataChannels;
			uint16	dataChannelCount;

			class	ControlConnection{
			protected:
				Node	*node;
				ControlConnection(Node	*node);
			public:
				virtual	~ControlConnection();
				virtual	void	scan()=0;
				virtual	void	acceptConnections()=0;
				virtual	int16	send(sdk::_ControlMessage	*m)=0;	//	broadcast; return 0 if successfull, error code (>0) otherwise
				virtual	int16	recv(sdk::_ControlMessage	**m)=0;
				virtual	void	sendTime()=0;
				virtual	void	recvTime()=0;
			};

			class	BroadcastControlConnection:
			public	ControlConnection{
			private:
				sdk::Connection	*connection;
			public:
				BroadcastControlConnection(Node	*node,sdk::Connection	*c);
				~BroadcastControlConnection();
				void	scan();
				void	acceptConnections();
				int16	send(sdk::_ControlMessage	*m);
				int16	recv(sdk::_ControlMessage	**m);
				void	sendTime();
				void	recvTime();
			};

			class	ConnectedControlConnection:
			public	ControlConnection{
			private:
				sdk::Connection	*connections;
				uint16			connectionCount;
				void			addConnection(sdk::Connection	*c,uint16	nid);
				void			removeConnection(uint16	nid);
			public:
				ConnectedControlConnection(Node	*node);
				~ConnectedControlConnection();
				void	scan();
				void	acceptConnections();
				int16	send(sdk::_ControlMessage	*m);
				int16	recv(sdk::_ControlMessage	**m);
				void	sendTime();
				void	recvTime();
			};

			ControlConnection	*controlConnection;
			ControlConnection	*syncConnection;
			
			bool	_shutdown;

			bool	isTimeReference();
			int64	timeDrift;	//	in ms

			static	uint32	thread_function	SendTime(void	*args);
			static	uint32	thread_function	UpdateTime(void	*args);

			static	uint32	thread_function	CrankExecutionUnit(void	*args);
			//	TODO:	define crank exec units

			static	uint32	thread_function	ReceiveMessages(void	*args);
			static	uint32	thread_function	SendMessages(void	*args);
			//	TODO:	define routing structures
		public:
			Node(const	char	*configFileName);
			~Node();
			void	run();
			void	shutdown();
			void	dump(const	char	*fileName);	//	dumps the current system state
			void	load(const	char	*fileName);	//	initializes itself from a previously saved system state
			void	loadApplication(const	char	*fileName=NULL);
			void	unloadApplication();
			void	send(uint16	crankID,sdk::_ControlMessage	*m);
			void	send(uint16	crankID,sdk::_Message	*m);
			void	send(uint16	crankID,sdk::_StreamData	*m);
			int64	time();	//	in ms since 01/01/70
			void	buildCrank(uint16	CID);
		};
	}
}


#endif
