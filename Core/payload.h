//	payload.h
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

#ifndef mBrane_sdk_payload_h
#define mBrane_sdk_payload_h

#include	"object.h"


namespace	mBrane{
	namespace	sdk{

		namespace	payloads{
			class	_Message;
			class	_StreamData;
		}

		//	Base interface for payloads
		class	mBrane_dll	__Payload:
		public	_Object{
		protected:
			__Payload();
		public:
			virtual	~__Payload();
			virtual	uint16		cid()	const=0;
			virtual	void		init();								//	called upon reception
			virtual	size_t		size()	const=0;					//	returns the size of the whole instance; called upon sending
			virtual	uint16		ptrCount()	const;					//	number of pointers to payloads
			virtual	__Payload	*getPtr(uint16	i)	const;			//	iterates the pointers to payloads
			virtual	void		setPtr(uint16	i,__Payload	*p);	//	iterates the pointers to payloads
		};

		//	Convenience for writing getPtr and setPtr
		#define	PTR(Class,Member)	(__Payload	*)(((uint8	*)this)+offsetof(Class,Member));

		class	payloads::_StreamData;
		class	payloads::_Message;

		class	mBrane_dll	_Payload:
		public	__Payload{
		public:
			typedef	enum{
				CONTROL=0,
				DATA=1,
				STREAM=2
			}Category;
		protected:
			int64	_node_recv_ts;	//	not transmitted
			int64	_recv_ts;		//	not transmitted
			uint64	_metaData;		//	[reserved(32)|cid(16)|reserved(14)|category(2)]
			int64	_node_send_ts;
			int64	_send_ts;
			_Payload();
		public:
			virtual	~_Payload();
			Category	category()	const;
			int64		&node_send_ts();	//	send timestamp: time of emission from a node
			int64		&node_recv_ts();	//	recv timestamp: time of reception by a node
			int64		&send_ts();			//	send timestamp: time of emission from a module (< than node_send_ts)
			int64		&recv_ts();			//	recv timestamp: time of reception by a module (> than node_recv_ts)
			//	down_casting; return NULL by default
			virtual	payloads::_Message		*as_Message();
			virtual	payloads::_StreamData	*as_StreamData();
		};

		class	mBrane_dll	_RPayload:
		public	__Payload{
		protected:
			uint64	_metaData;	//	[reserved(32)|cid(16)|reserved(16)]
			_RPayload();
		public:
			virtual	~_RPayload();
		};

		//	Base class for all payloads.
		template<class	P,class	U,class	M>	class	___Payload:	//	P: payload class, U: final class, M: memory
		public	Object<M,P,U>{
		protected:
			//	convenience for accessing the Memory from subclasses unaware of M
			static	void	*Alloc(uint32	requested_size,uint32	&normalized_size);
			static	void	Dealloc(uint32	requested_size,void	*o);
			___Payload();
		public:
			static	uint64		_MetaData;
			static	void		*New(uint32	size);	//	to initialize the _vftable on recv(); size used for non-standard cases (like Storage<T>), i.e. when the actual size is not sizeof the class
			static	uint16		CID();
			static	size_t		Offset();	//	to metadata from this
			void	*operator	new(size_t	s);
			void	operator	delete(void	*o);
			virtual	~___Payload();
			uint16				cid()	const;
			virtual	size_t		size()	const;	//	default; returns sizeof(U)
		};

		template<class	U,class	M>	class	Payload:
		public	___Payload<_Payload,U,M>{
		protected:
			Payload();
		public:
			virtual	~Payload();
		};

		//	Template variant of the well-known DP. Adapts C to Payload<U,M>.
		//	Usage:	class	Some3rdPartyClass{ ... };
		//			class Some3rdPartyClassAdapted:public PayloadAdapter<Some3rdPartyClass,Some3rdPartyClassAdapted>{ ... };
		template<class	C,class	U,class	M>	class	PayloadAdapter:
		public	C,
		public	Payload<U,M>{
		protected:
			PayloadAdapter();
		public:
			virtual	~PayloadAdapter();
		};

		//	Standard raw payload (no transmission information) to embed in any payload.
		template<class	U,class	M>	class	RPayload:
		public	___Payload<_RPayload,U,M>{
		protected:
			RPayload();
		public:
			virtual	~RPayload();
		};

		//	Template variant of the well-known DP. Adapts C to RPayload<U,M>.
		template<class	C,class	U,class	M>	class	RPayloadAdapter:
		public	C,
		public	RPayload<U,M>{
		protected:
			RPayloadAdapter();
		public:
			virtual	~RPayloadAdapter();
		};
	}
}


#include	"payload.tpl.cpp"


#endif
