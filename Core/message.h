// message.h
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

#ifndef	mBrane_sdk_message_h
#define	mBrane_sdk_message_h

#include	"payload.h"
#include	"memory.h"


namespace	mBrane{
	namespace	sdk{

		template<class	Data>	class	CrankData:
		public	Payload<Memory,CrankData<Data> >,
		public	Data{
		public:
			CrankData();
			virtual	~CrankData();
			bool	isCrankData()	const;
		};

		class	dll	_ControlMessage{
		protected:
			uint32	_mid;	//	content identifer
			uint8	_priority;
			uint16	_senderNodeID;
			_ControlMessage(uint32	mid=0,uint8	priority=0);
		public:
			virtual	~_ControlMessage();
			operator	_Payload	*()	const;
			uint32	&mid();
			uint8	&priority();
			uint16	&senderNode_id();
		};

		template<class	U>	class	ControlMessage:	//	subclasses shall have no embedded pointers
		public	Payload<Memory,U>,
		public	_ControlMessage{
		protected:
			ControlMessage(uint32	mid=0,uint8	priority=0);
		public:
			virtual	bool	isControlMessage();
		};

		class	dll	_StreamData{
		protected:
			_StreamData();
		public:
			virtual	~_StreamData();
			operator	_ControlMessage	*()	const;
			operator	_Payload	*()	const;
		};

		template<class	U>	class	StreamData:
		public	ControlMessage<U>,
		public	_StreamData{
		protected:
			StreamData();
		public:
			bool	isControlMessage();
			bool	isStreamData();
		};

		class	dll	_Message{
		protected:
			uint16	_senderEntityCID;
			uint16	_senderEntityIID;
			uint16	_senderCrankCID;
			uint16	_senderCrankIID;
			_Message();
		public:
			virtual	~_Message();
			operator	_ControlMessage	*()	const;
			operator	_Payload	*()	const;
			uint16	&senderEntity_cid();
			uint16	&senderEntity_iid();
			uint16	&senderCrank_cid();
			uint16	&senderCrank_iid();
		};

		template<class	U>	class	Message:
		public	ControlMessage<U>,
		public	_Message{
		protected:
			Message();
		public:
			bool	isControlMessage();
			bool	isMessage();
		};
	}
}


#include	"message.tpl.cpp"


#endif
