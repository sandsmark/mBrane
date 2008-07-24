// payload.h
//
// Author: Eric Nivel
//
//

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
			uint16	cid()	const{	return	_cid;	}
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
			static	const	uint16	CID();
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