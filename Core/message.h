//	message.h
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

#ifndef	mBrane_sdk_message_h
#define	mBrane_sdk_message_h

#include	"payload.h"
#include	"memory.h"

//	Root classes for defining applicative message classes
namespace	mBrane{
	namespace	sdk{

		class	CommChannel;

		namespace	payloads{

			template<class	Data>	class	CrankData:	//	binary data to be migrated
			public	Payload<Memory,CrankData<Data> >,
			public	Data{
			public:
				CrankData();
				virtual	~CrankData();
				bool	isCrankData()	const;
			};

			class	dll	_ControlMessage{
			protected:
				uint32	_mid;	//	content identifer (for streams: stream identifier, as ooposed to frame identifier: to be defined in U classes)
				uint8	_priority;	//	the lower the number the higher the priority
				uint16	_senderNodeID;
				_ControlMessage(uint32	mid,uint8	priority);
			public:
				virtual	~_ControlMessage();
				operator	_Payload	*()	const;
				uint32	&mid();
				uint8	&priority();
				uint16	&senderNode_id();
			};

			template<class	U>	class	ControlMessage:
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
				StreamData(uint32	mid=0,uint8	priority=0);
			public:
				bool	isControlMessage();
				bool	isStreamData();
			};

			class	dll	_DynamicData{
			protected:
				_DynamicData();
			public:
				virtual	~_DynamicData();
				bool	isDynamicData()	const;
				virtual	size_t	dynamicSize()	const=0;	//	size of the dynamic data (not of the whole class)
			};

			class	dll	_CompressedPayload:
			public	_DynamicData{
			friend	class	CommChannel;
			protected:
				bool	isCompressed;	//	must be updated by compress() and any method changing the content of the frame, defined in U classes
				_CompressedPayload();
			public:
				virtual	~_CompressedPayload();
				bool	isCompressedPayload()	const;
				virtual	void	compress();
				virtual	void	decompress();
			};

			template<class	S,class	D,class	F>	class	CompressedPayload:	//	S: superclass, either Message<U>, StreamData<U> or Payload<U>; subclass U shall not define any data at all (already defined by class D); F: frame data
			public	S,
			public	_CompressedPayload,
			public	D{
			protected:
				F	compressedFrame;	//	the last data to be transmitted, i.e. anything declared afterwards will not be transmitted
				F	uncompressedFrame;
				CompressedPayload();
			public:
				static	const	size_t	CoreSize();
			};

			class	dll	_Message{
			protected:
				uint16	_senderModuleCID;
				uint16	_senderModuleIID;
				uint16	_senderCrankCID;
				uint16	_senderCrankIID;
				_Message();
			public:
				virtual	~_Message();
				operator	_ControlMessage	*()	const;
				operator	_Payload	*()	const;
				uint16	&senderModule_cid();
				uint16	&senderModule_iid();
				uint16	&senderCrank_cid();
				uint16	&senderCrank_iid();
			};

			template<class	U>	class	Message:
			public	ControlMessage<U>,
			public	_Message{
			protected:
				Message(uint32	mid=0,uint8	priority=0);
			public:
				bool	isControlMessage();
				bool	isMessage();
			};
		}
	}
}


#include	"message.tpl.cpp"


#endif
