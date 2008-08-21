//	payload.h
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

#ifndef mBrane_sdk_payload_h
#define mBrane_sdk_payload_h

#include	"object.h"


namespace	mBrane{
	namespace	sdk{

		namespace	payloads{
			class	_DynamicData;
			class	_CompressedData;
			class	_ModuleData;
			class	_Message;
			class	_StreamData;
		}

		typedef	enum{
			STATIC=0,
			DYNAMIC=1,
			COMPRESSED=2
		}AllocationScheme;

		class	dll	__Payload{
		};

		class	dll	_Payload:
		public	_Object,
		public	__Payload{
		public:
			typedef	enum{
				CONTROL=0,
				DATA=1,
				STREAM=2
			}Category;
		protected:
			int64	_node_recv_ts;	//	non transmitted
			int64	_recv_ts;		//	non transmitted
			uint32	_metaData;		//	offset points here; metadata: [cid(16)|reserved(12)|category(2)|allocation scheme(2)]
			int64	_node_send_ts;
			int64	_send_ts;
			_Payload();
		public:
			static	const	size_t	Offset();
			virtual	~_Payload();
			uint16				cid()				const;
			Category			category()			const;
			AllocationScheme	allocationScheme()	const;
			virtual	void	init();	//	called upon reception
			int64	&node_send_ts();	//	send timestamp: time of emission from a node
			int64	&node_recv_ts();	//	recv timestamp: time of reception by a node
			int64	&send_ts();	//	send timestamp: time of emission from a module (< than node_send_ts)
			int64	&recv_ts();	//	recv timestamp: time of reception by a module (> than node_recv_ts)
			virtual	operator	payloads::_DynamicData		*()	const;
			virtual	operator	payloads::_CompressedData	*()	const;
			virtual	operator	payloads::_Message			*()	const;
			virtual	operator	payloads::_StreamData		*()	const;
		};

		class	_RPayload;
		template<class	M,class	U>	class	Payload:
		public	Object<M,_Payload,U>{
		private:
			static	const	uint32	_MetaData;
		protected:
			Payload();
		public:
			static	void	*New();
			void	*operator	new(size_t	s);
			void	operator	delete(void	*o);
			static	const	uint16				CID();
			static	const	AllocationScheme	_AllocationScheme();
			static	const	uint8				PtrCount();
			static	P<_RPayload>				*Ptr(__Payload	*p,uint8	i);
		};

		//	Usage:	template<class	C>	class	DaughterClass: public Payload<Memory,C>{ ... };
		//			class _DaughterClass:public DaughterClass<_DaughterClass>{};
		//			NB: Memory can be any Allocator class

		template<class	C,class	M,class	U>	class	PayloadAdapter:
		public	C,
		public	Payload<M,U>{
		protected:
			PayloadAdapter();
		};

		//	Convenience for writing Ptr(_RPayload	*p,uint8	i)
#define	PTR(Class,Instance,Member)	(P<_RPayload>	*)(((uint8	*)Instance)+offsetof(Class,Member));

		//	Usage:	class	Some3rdPartyClass{ ... };
		//			class Some3rdPartyClassAdapted:public PayloadAdapter<Some3rdPartyClass,Memory,Some3rdPartyClassAdapted>{ ... };
		//			NB: Memory can be any Allocator class

		class	dll	_RPayload:	//	raw payload (i.e. without send/recv time stamps) to embed (P<>)in payloads
		public	_Object,
		public	__Payload{
		protected:
			uint32	_metaData;	//	offset points here; metadata: [cid(16)|reserved(14)|allocation scheme(2)]
			_RPayload(AllocationScheme	a);
		public:
			static	const	size_t	Offset();
			virtual	~_RPayload();
			uint16				cid()				const;
			AllocationScheme	allocationScheme()	const;
			virtual	void	init();	//	called upon reception
			virtual	operator	payloads::_DynamicData		*()	const;
			virtual	operator	payloads::_CompressedData	*()	const;
		};

		template<class	M,class	U>	class	RPayload:
		public	Object<M,_RPayload,U>{
		private:
			static	const	uint16	_CID;
		protected:
			RPayload();
		public:
			static	void	*New();
			void	*operator	new(size_t	s);
			void	operator	delete(void	*o);
			static	const	uint16				CID();
			static	const	AllocationScheme	_AllocationScheme();
			static	const	uint8				PtrCount();
			static	P<_RPayload>				*Ptr(__Payload	*p,uint8	i);
		};

		template<class	C,class	M,class	U>	class	RPayloadAdapter:
		public	C,
		public	RPayload<M,U>{
		protected:
			RPayloadAdapter();
		};
	}
}


#include	"payload.tpl.cpp"


#endif