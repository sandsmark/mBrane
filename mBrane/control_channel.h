// control_channel.h
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

#ifndef	mBrane_network_control_channel_h
#define	mBrane_network_control_channel_h

//#include	"..\Core\node.h"
//#include	"..\Core\message.h"
#include	"..\Core\network_interface.h"


namespace	mBrane{
	namespace	node{
		class	Node;
	}
	namespace	network{

		class	ControlChannel{
		protected:
			node::Node	*node;
			ControlChannel(node::Node	*node);
		public:
			virtual	~ControlChannel();
			virtual	void	scan()=0;
			virtual	void	acceptConnections()=0;
			virtual	int16	send(sdk::_Payload	*m)=0;	//	broadcast; return 0 if successfull, error code (>0) otherwise
			virtual	int16	recv(sdk::_Payload	**m)=0;
			virtual	void	sendTime()=0;
			virtual	void	recvTime()=0;
		};

		class	BroadcastControlChannel:
		public	ControlChannel{
		private:
			sdk::Connection	*connection;
		public:
			BroadcastControlChannel(node::Node	*node,sdk::Connection	*c);
			~BroadcastControlChannel();
			void	scan();
			void	acceptConnections();
			int16	send(sdk::_Payload	*m);
			int16	recv(sdk::_Payload	**m);
			void	sendTime();
			void	recvTime();
		};

		class	ConnectedControlChannel:
		public	ControlChannel{
		private:
			sdk::Connection	*connections;
			uint16			connectionCount;
			void			addConnection(sdk::Connection	*c,uint16	nid);
			void			removeConnection(uint16	nid);
		public:
			ConnectedControlChannel(node::Node	*node);
			~ConnectedControlChannel();
			void	scan();
			void	acceptConnections();
			int16	send(sdk::_Payload	*m);
			int16	recv(sdk::_Payload	**m);
			void	sendTime();
			void	recvTime();
		};

		typedef	struct{
			sdk::ConnectedCommChannel	*data;
			sdk::ConnectedCommChannel	*stream;
		}DataCommChannel;
	}
}


#endif
