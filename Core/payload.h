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

		class	_Payload:
		public	_Object{
		protected:
			uint8	*_buffer;
			size_t	_size;
			uint16	_cid;
			int64	_send_ts;
			int64	_recv_ts;
			int8	_send();
			int8	_recv();
			_Payload();
			~_Payload();
			void	init(uint8	*_buffer,size_t	_size);
		public:
			uint16	cid()	const{	return	_cid;	}
			uint8	*buffer()	const;
			size_t	size()	const;
			virtual	uint8		ptrCount()	const;
			virtual	_Payload	*ptr(uint8	i)	const;
			int64	&send_ts();
			int64	&recv_ts();
			int8	send();
			int8	recv();
		};

		template<class	M,class	U>	class	Payload:
		public	Object<M,_Payload,U>{
		private:
			static	const	uint16	_CID;
			_Payload	*p;
		protected:
			Payload();
			~Payload();
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
			~PayloadAdapter();
		};

		//	Usage:	class	Some3rdPartyClass;
		//			class Some3rdPartyClassAdapted:public PayloadAdapter<Some3rdPartyClass,Memory,Some3rdPartyClassAdapted>{ ... };
		//			NB: Memory can be any Allocator class

		class	_PP:	//	pointer to payload
		public	__P{
		protected:
			_PP();
			~_PP();
		public:
			operator	_Payload	*();
			_PP	&operator	=(_Payload	*o);
			_PP	&operator	=(_PP	&p);
		};

		template<class	C>	class	PP:		public	SP<C,_PP>{};
	}
}


#include	"payload.tpl.cpp"


#endif