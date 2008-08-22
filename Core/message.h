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

			class	dll	_DynamicData{
			protected:
				_DynamicData();
			public:
				static	const	AllocationScheme	_AllocationScheme();
				virtual	~_DynamicData();
				virtual	size_t	dynamicSize()	const=0;	//	size of the dynamic data (not of the whole class)
			};

			template<class	S>	class	DynamicData:
			public	S,
			public	_DynamicData{
			};

			class	dll	_CompressedData:
			public	_DynamicData{
			friend	class	CommChannel;
			protected:
				bool	isCompressed;	//	must be updated by compress() and any method changing the content of the frame, defined in U classes
				_CompressedData();
			public:
				static	const	AllocationScheme	_AllocationScheme();
				virtual	~_CompressedData();
				virtual	void	compress();
				virtual	void	decompress();
			};

			template<class	S,class	D,class	F>	class	CompressedData:	//	S: superclass, either Message<U>, StreamData<U>, Payload<U> or RPayload<U>; subclass U shall not define any data at all (already defined by class D); F: frame data
			public	S,
			public	_CompressedData,
			public	D{
			protected:
				F	compressedFrame;	//	the last data to be transmitted, i.e. anything declared afterwards will not be transmitted
				F	uncompressedFrame;
				CompressedData();
			public:
				static	const	size_t	CoreSize();
			};

			////////////////////////////////////////////////////////////////////////////////////////////////

			template<class	Data>	class	ModuleData:	//	binary data to be migrated
			public	Payload<Memory,ModuleData<Data> >,
			public	Data{
			public:
				ModuleData();
				virtual	~ModuleData();
			};

			template<class	U>	class	ControlMessage:
			public	Payload<Memory,U>{
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
				operator	_Payload	*()	const;
			};

			template<class	U>	class	StreamData:
			public	Payload<Memory,U>,
			public	_StreamData{
			protected:
				StreamData(uint16	sid);
			public:
				operator	_StreamData		*()	const;
			};

			class	dll	_Message{
			protected:
				uint16	_senderModuleCID;
				uint16	_senderModuleID;
				uint16	_senderClusterCID;
				uint16	_senderClusterID;
				_Message();
			public:
				virtual	~_Message();
				operator	_Payload	*()	const;
				uint16	&senderModule_cid();
				uint16	&senderModule_id();
				uint16	&senderCluster_cid();
				uint16	&senderCluster_id();
			};

			template<class	U>	class	Message:
			public	Payload<Memory,U>,
			public	_Message{
			protected:
				Message();
			public:
				operator	_Message	*()	const;
			};
		}
	}
}


#include	"message.tpl.cpp"


#endif
