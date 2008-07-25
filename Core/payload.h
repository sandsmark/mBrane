// payload.h
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

#ifndef mBrane_sdk_payload_h
#define mBrane_sdk_payload_h

#include	"object.h"


namespace	mBrane{
	namespace	sdk{

		template<class	C>	class	PP;
		class	dll	_Payload:
		public	_Object{
		protected:
			int64	_recv_ts;
			uint16	_cid;
			int64	_send_ts;
			_Payload();
		public:
			virtual	~_Payload();
			uint16	cid()	const;
			virtual	uint8		ptrCount()	const;
			virtual	_Payload	**ptr(uint8	i);
			int64	&send_ts();
			int64	&recv_ts();
		};

		template<class	M,class	U>	class	Payload:
		public	Object<M,_Payload,U>{
		private:
			static	const	uint16	_CID;
		protected:
			Payload();
			virtual	~Payload();
		public:
			void	*operator	new(size_t	s);
			void	operator	delete(void	*o);
			static	const	uint16	cid();
		};

		//	Usage:	template<class	C>	class	DaughterClass: public Payload<Memory,C>{ ... };
		//			class _DaughterClass:public DaughterClass<_DaughterClass>{};
		//			NB: Memory can be any Allocator class

		template<class	C,class	M,class	U>	class	PayloadAdapter:
		public	C,
		public	Payload<M,U>{
		protected:
			PayloadAdapter();
			virtual	~PayloadAdapter();
		};

		//	Usage:	class	Some3rdPartyClass{ ... };
		//			class Some3rdPartyClassAdapted:public PayloadAdapter<Some3rdPartyClass,Memory,Some3rdPartyClassAdapted>{ ... };
		//			NB: Memory can be any Allocator class

		class	dll	_PP:	//	lazy pointer to payload
		public	__P{
		protected:
			_PP();
			virtual	~_PP();
			operator	_Payload	*();
			_PP	&operator	=(_Payload	*o);
			_PP	&operator	=(_PP	&p);
		};

		template<class	C>	class	PP:		public	SP<C,_PP>{
		public:
			using	SP<C,_PP>::operator =;
			PP();
			PP(C	*o);
			~PP();
			_Payload	**objectAddr();
		};
	}
}


#include	"payload.tpl.cpp"


#endif