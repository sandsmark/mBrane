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

			template<class	U>	class	ControlMessage:
			public	Payload<U,Memory>{
			protected:
				ControlMessage();
			public:
				virtual	~ControlMessage();
			};

			////////////////////////////////////////////////////////////////////////////////////////////////

			class	mBrane_dll	_StreamData{
			protected:
				uint16	_sid;	//	stream identifer
				_StreamData(uint16	sid);
			public:
				virtual	~_StreamData();
				uint16	&sid();
			};

			////////////////////////////////////////////////////////////////////////////////////////////////
		
			template<class	U,class	M>	class	StreamData:
			public	Payload<U,M>,
			public	_StreamData{
			protected:
				StreamData(uint16	sid=0);
			public:
				virtual	~StreamData();
				_StreamData	*as_StreamData();
			};

			////////////////////////////////////////////////////////////////////////////////////////////////

			class	mBrane_dll	_Message{
			protected:
				uint16	_senderModuleCID;
				uint16	_senderModuleID;
				uint16	_senderNodeID;
				_Message();
			public:
				virtual	~_Message();
				uint16	&senderModule_cid();
				uint16	&senderModule_id();
				uint16	&senderNodeID();
			};

			////////////////////////////////////////////////////////////////////////////////////////////////

			template<class	U,class	M>	class	Message:
			public	Payload<U,M>,
			public	_Message{
			protected:
				Message();
			public:
				virtual	~Message();
				_Message	*as_Message();
			};

			////////////////////////////////////////////////////////////////////////////////////////////////

			//	Compound messages are built from a user-defined core C, followed by a continous array of elements of type T.
			//	Usage sample: class CoreData{...}; class ACompound:public CompoundMessage<ACompound,Memory,CoreData,word32>{};
			//	ACompound	*ac=new(32) ACompound(); // ac contains the data from CoreData followed by an array of 32 word32.
			//	Do not declare any data in subclasses of CompoundMessage: such subclasses shall only contain logic, e.g. functions to exploit C and the array of Ts.
			//	Typical use: data compacted dynamically, i.e. whose size is not an integral constant (e.g. that could not parameterize a template), e.g. archives, compressed images, etc.
			template<class	U,class	M,class	C,typename	T>	class	CompoundMessage:
			public	Message<U,M>,
			public	C{
			protected:
				uint32	_size;		//	of the whole instance (not normalized)
				uint32	_capacity;	//	max number of elements in the array
				T		*_data;		//	points to ((T	*)(((uint8	*)this)+offsetof(CompoundMessage<U,M,C,T>,_data)+sizeof(T	*)));
				CompoundMessage();
			public:
				static	void		*New(uint32	size);					//	total size of the instance; called upon sending
				void	*operator	new(size_t	s,uint32	capacity);	//	overrides Object<U,M>::new
				void	operator	delete(void	*o);					//	overrides Object<U,M>::delete
				virtual	~CompoundMessage();
				size_t	size()	const;
				uint32	getCapacity()	const;
				T		&operator	[](uint32	i);
				T		*data();
			};

			//	Variant with no Core.
			template<class	U,class	M,typename	T>	class	ArrayMessage:
			public	Message<U,M>{
			protected:
				uint32	_size;		//	of the whole instance (not normalized)
				uint32	_capacity;	//	max number of elements in the array
				T		*_data;		//	points to ((T	*)(((uint8	*)this)+offsetof(CompoundMessage<U,M,C,T>,_data)+sizeof(T	*)));
				ArrayMessage();
			public:
				static	void		*New(uint32	size);					//	total size of the instance; called upon sending
				void	*operator	new(size_t	s,uint32	capacity);	//	overrides Object<U,M>::new
				void	operator	delete(void	*o);					//	overrides Object<U,M>::delete
				virtual	~ArrayMessage();
				size_t	size()	const;
				uint32	getCapacity()	const;
				T		&operator	[](uint32	i);
				T		*data();
			};
		}
	}
}


#include	"message.tpl.cpp"


#endif
