//	control_messages.h
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

#ifndef	mBrane_sdk_control_messages_h
#define	mBrane_sdk_control_messages_h

#include	"message.h"


namespace	mBrane{
	namespace	sdk{
		namespace	payloads{

			class	dll	SystemReady:	//	sent when all the nodes specified in the node cfg file are up and running
			public	ControlMessage<SystemReady>{
			};

			class	dll	SyncProbe:
			public	ControlMessage<SyncProbe>{
			};

			class	dll	SyncEcho:
			public	ControlMessage<SyncEcho>{
			public:
				int64	time;	//	us
			};

			class	dll	NodeJoined:
			public	ControlMessage<NodeJoined>{
			public:
				uint16	node_id;
			};

			class	dll	NodeLeft:
			public	ControlMessage<NodeLeft>{
			public:
				uint16	node_id;
			};

			class	dll	SetThreshold:
			public	ControlMessage<SetThreshold>{
			public:
				uint16	host_id;
				uint16	space_id;
				float32	threshold;
			};

			class	dll	ActivateModule:
			public	ControlMessage<ActivateModule>{
			public:
				uint16	host_id;
				uint16	module_cid;
				uint16	module_id;
				uint16	space_id;
				float32	activationLevel;
			};

			class	dll	ActivateSpace:
			public	ControlMessage<ActivateSpace>{
			public:
				uint16	host_id;
				uint16	space_id;	//	the space on which target space is projected onto
				uint16	target_sid;	//	the space to set the activation for
				float32	activationLevel;
			};

			template<class	U>	class	Subscribe:
			public	ControlMessage<U>{
			public:
				uint16	host_id;
				uint16	module_cid;
				uint16	module_id;
				uint16	space_id;
			};

			class	dll	SubscribeMessage:
			public	Subscribe<SubscribeMessage>{
			public:
				uint16	message_cid;
			};

			class	dll	UnsubscribeMessage:
			public	Subscribe<UnsubscribeMessage>{
			public:
				uint16	message_cid;
			};

			class	dll	SubscribeStream:
			public	Subscribe<SubscribeStream>{
			public:
				uint16	stream_id;
			};

			class	dll	UnsubscribeStream:
			public	Subscribe<UnsubscribeStream>{
			public:
				uint16	stream_id;
			};

			class	dll	CreateModule:
			public	ControlMessage<CreateModule>{
			public:
				uint16	sender_cid;
				uint16	sender_id;
				uint16	host_id;
				uint16	module_cid;
			};

			class	dll	DeleteModule:
			public	ControlMessage<DeleteModule>{
			public:
				uint16	host_id;
				uint16	module_cid;
				uint16	module_id;
			};

			class	dll	CreateSpace:
			public	ControlMessage<CreateSpace>{
			public:
				uint16	sender_cid;
				uint16	sender_id;
				uint16	host_id;
			};

			class	dll	DeleteSpace:
			public	ControlMessage<DeleteSpace>{
			public:
				uint16	host_id;
				uint16	space_id;
			};

			class	dll	KillModule:	//	internal message issued upon reception of DeleteModule
			public	ControlMessage<KillModule>{
			};

			#if defined(WINDOWS)
				#define	SystemReady_CID			0
				#define	SyncProbe_CID			1
				#define	SyncEcho_CID			2
				#define	NodeJoined_CID			3
				#define	NodeLeft_CID			4
				#define	SetThreshold_CID		5
				#define	ActivateModule_CID		6
				#define	ActivateSpace_CID		7
				#define	SubscribeMessage_CID	8
				#define	UnsubscribeMessage_CID	9
				#define	SubscribeStream_CID		10
				#define	UnsubscribeStream_CID	11
				#define	CreateModule_CID		12
				#define	DeleteModule_CID		13
				#define	CreateSpace_CID			14
				#define	DeleteSpace_CID			15
				#define	KillModule_CID			18
			#else
				#define SystemReady_CID			0
				#define SyncProbe_CID			14
				#define SyncEcho_CID			15
				#define NodeJoined_CID			1
				#define NodeLeft_CID			2
				#define SetThreshold_CID		9
				#define ActivateModule_CID		7
				#define ActivateSpace_CID		8
				#define SubscribeMessage_CID	10
				#define UnsubscribeMessage_CID	11
				#define SubscribeStream_CID		12
				#define UnsubscribeStream_CID	13
				#define CreateModule_CID		4
				#define DeleteModule_CID		6
				#define CreateSpace_CID			3
				#define DeleteSpace_CID			5
				#define KillModule_CID			16
			#endif
		}
	}
}


#endif
