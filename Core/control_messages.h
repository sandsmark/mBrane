//	control_messages.h
//
//	Author: Eric Nivel
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

#ifndef	mBrane_sdk_control_messages_h
#define	mBrane_sdk_control_messages_h

#include	"message.h"


namespace	mBrane{
	namespace	sdk{
		namespace	payloads{

			class	dll	SystemReady:	//	sent when all the nodes specified in the node cfg file are up and running
			public	ControlMessage<SystemReady>{
			};

			class	dll	TimeSync:
			public	ControlMessage<TimeSync>{
			};

			class	dll	NodeID{
			protected:
				uint16	nodeID;
			public:
				uint16	&node_id();
			};

			class	dll	NodeJoined:
			public	ControlMessage<NodeJoined>,
			public	NodeID{
			};

			class	dll	NodeLeft:
			public	ControlMessage<NodeLeft>,
			public	NodeID{
			};

			class	dll	ModuleID{
			protected:
				uint16	moduleID;
			public:
				uint16	&module_id();
			};

			class	dll	SpaceID{
			protected:
				uint16	spaceID;
			public:
				uint16	&space_id();
			};

			class	dll	ActivationLevel{
			protected:
				float32	_activationLevel;
			public:
				float32	&activationLevel();
			};

			class	dll	Threshold{
			protected:
				float32	_threshold;
			public:
				float32	&threshold();
			};

			class	dll	SetThreshold:
			public	ControlMessage<SetThreshold>,
			public	SpaceID,
			public	Threshold{
			};

			class	dll	ActivateModule:
			public	ControlMessage<ActivateModule>,
			public	ModuleID,
			public	SpaceID,
			public	ActivationLevel{				
			};

			class	dll	ActivateSpace:
			public	ControlMessage<ActivateSpace>,
			public	SpaceID,
			public	ActivationLevel{
			private:
				uint16	_targetSID;
			public:
				uint16	&target_sid();
			};

			template<class	U>	class	Subscribe:
			public	ControlMessage<U>,
			public	ModuleID,
			public	SpaceID{
			};

			class	dll	MessageID{
			protected:
				uint16	messageCID;
			public:
				uint16	&message_cid();
			};

			class	dll	StreamID{
			protected:
				uint16	streamID;
			public:
				uint16	&stream_id();
			};

			class	dll	SubscribeMessage:
			public	Subscribe<SubscribeMessage>,
			public	MessageID{
			};

			class	dll	SubscribeStream:
			public	Subscribe<SubscribeStream>,
			public	StreamID{
			};

			class	dll	UnsubscribeMessage:
			public	Subscribe<UnsubscribeMessage>,
			public	MessageID{
			};

			class	dll	UnsubscribeStream:
			public	Subscribe<UnsubscribeStream>,
			public	StreamID{
			};

			#define	SystemReady_CID			0
			#define	TimeSync_CID			1
			#define	NodeJoined_CID			2
			#define	NodeLeft_CID			3
			#define	SetThreshold_CID		4
			#define	ActivateModule_CID		5
			#define	ActivateSpace_CID		6
			#define	SubscribeMessage_CID	7
			#define	SubscribeStream_CID		8
			#define	UnsubscribeMessage_CID	9
			#define	UnsubscribeStream_CID	10
		
		}
	}
}


#endif