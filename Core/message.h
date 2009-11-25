//	message.h
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

#ifndef	mBrane_sdk_message_h
#define	mBrane_sdk_message_h

#include	"payload.h"
#include	"memory.h"

//	Root classes for defining applicative message classes
namespace	mBrane{
	namespace	sdk{

		class	CommChannel;

		namespace	payloads{

			//	Utility class
			template<class	U,class	D,class	F,class	P,class	M>	class	CompressedStreamData:	//	P: payload class, _Payload or _RPayload; subclass U shall not define any data at all (already defined by class D); F: frame data
			public	___Payload<CompressedData,P,U,M>,
			public	D{
			protected:
				F	compressedFrame;	//	the last data to be transmitted, i.e. anything declared afterwards will not be transmitted
				F	uncompressedFrame;
				CompressedStreamData();
			public:
				static	size_t	CoreSize();
				virtual	~CompressedStreamData();
			};

			////////////////////////////////////////////////////////////////////////////////////////////////

			template<class	U>	class	ControlMessage:
			public	Payload<U,StaticData,Memory>{
			protected:
				ControlMessage();
			public:
				virtual	~ControlMessage();
			};

			class	dll	_StreamData{
			protected:
				uint16	_sid;	//	stream identifer
				_StreamData(uint16	sid);
			public:
				virtual	~_StreamData();
				uint16	&sid();
			};
		
			template<class	U,template<class>	class	A,class	M>	class	StreamData:
			public	Payload<U,A,M>,
			public	_StreamData{
			protected:
				StreamData(uint16	sid=0);
			public:
				virtual	~StreamData();
				_StreamData	*as_StreamData();
			};

			class	dll	_Message{
			protected:
				uint16	_senderModuleCID;
				uint16	_senderModuleID;
				_Message();
			public:
				virtual	~_Message();
				uint16	&senderModule_cid();
				uint16	&senderModule_id();
			};

			template<class	U,template<class>	class	A,class	M>	class	Message:
			public	Payload<U,A,M>,
			public	_Message{
			protected:
				Message();
			public:
				virtual	~Message();
				_Message	*as_Message();
			};
		}
	}
}


#include	"message.tpl.cpp"


#endif
