//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
//_/_/
//_/_/ HUMANOBS - mBrane
//_/_/
//_/_/ Eric Nivel
//_/_/ Center for Analysis and Design of Intelligent Agents
//_/_/   Reykjavik University, Menntavegur 1, 101 Reykjavik, Iceland
//_/_/   http://cadia.ru.is
//_/_/ Copyright(c)2012
//_/_/
//_/_/ This software was developed by the above copyright holder as part of 
//_/_/ the HUMANOBS EU research project, in collaboration with the 
//_/_/ following parties:
//_/_/ 
//_/_/ Autonomous Systems Laboratory
//_/_/   Technical University of Madrid, Spain
//_/_/   http://www.aslab.org/
//_/_/
//_/_/ Communicative Machines
//_/_/   Edinburgh, United Kingdom
//_/_/   http://www.cmlabs.com/
//_/_/
//_/_/ Istituto Dalle Molle di Studi sull'Intelligenza Artificiale
//_/_/   University of Lugano and SUPSI, Switzerland
//_/_/   http://www.idsia.ch/
//_/_/
//_/_/ Institute of Cognitive Sciences and Technologies
//_/_/   Consiglio Nazionale delle Ricerche, Italy
//_/_/   http://www.istc.cnr.it/
//_/_/
//_/_/ Dipartimento di Ingegneria Informatica
//_/_/   University of Palermo, Italy
//_/_/   http://roboticslab.dinfo.unipa.it/index.php/Main/HomePage
//_/_/
//_/_/
//_/_/ --- HUMANOBS Open-Source BSD License, with CADIA Clause v 1.0 ---
//_/_/
//_/_/ Redistribution and use in source and binary forms, with or without 
//_/_/ modification, is permitted provided that the following conditions 
//_/_/ are met:
//_/_/
//_/_/ - Redistributions of source code must retain the above copyright 
//_/_/ and collaboration notice, this list of conditions and the 
//_/_/ following disclaimer.
//_/_/
//_/_/ - Redistributions in binary form must reproduce the above copyright 
//_/_/ notice, this list of conditions and the following
//_/_/ disclaimer in the documentation and/or other materials provided 
//_/_/ with the distribution.
//_/_/
//_/_/ - Neither the name of its copyright holders nor the names of its 
//_/_/ contributors may be used to endorse or promote products 
//_/_/ derived from this software without specific prior written permission.
//_/_/
//_/_/ - CADIA Clause: The license granted in and to the software under this 
//_/_/ agreement is a limited-use license. The software may not be used in 
//_/_/ furtherance of: 
//_/_/ (i) intentionally causing bodily injury or severe emotional distress 
//_/_/ to any person; 
//_/_/ (ii) invading the personal privacy or violating the human rights of 
//_/_/ any person; or 
//_/_/ (iii) committing or preparing for any act of war.
//_/_/
//_/_/ THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
//_/_/ "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
//_/_/ LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR 
//_/_/ A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
//_/_/ OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
//_/_/ SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
//_/_/ LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, 
//_/_/ DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY 
//_/_/ THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
//_/_/ (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
//_/_/ OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//_/_/
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/ 

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

			virtual	bool	isShared()		const;	//	called upon reception and transmission.
			virtual	bool	isConstant()	const;	//	called upon reception and transmission.
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
		private:
			static	uint32	LastConstantOID;
			static	uint32	LastSharedOID;
		protected:
			uint64	_node_recv_ts;	//	not transmitted
			uint64	_recv_ts;		//	not transmitted
			uint64	_metaData;		//	[oid(32)|cid(16)|reserved(14)|category(2)]; oid==0x00FFFFFF means non shared object, or shared object not sent yet.
			uint64	_node_send_ts;
			uint64	_send_ts;
			_Payload();
		public:
			virtual	~_Payload();
			Category	category()	const;
			uint64		&node_send_ts();	//	send timestamp: time of emission from a node
			uint64		&node_recv_ts();	//	recv timestamp: time of reception by a node
			uint64		&send_ts();			//	send timestamp: time of emission from a module (< than node_send_ts)
			uint64		&recv_ts();			//	recv timestamp: time of reception by a module (> than node_recv_ts)
			//	down_casting; return NULL by default
			virtual	payloads::_Message		*as_Message();
			virtual	payloads::_StreamData	*as_StreamData();
			//	caching.
			void	setOID(uint8	NID);	//	for shared objects.
			void	setOID();				//	for constant objects.
			uint32	getOID()	const;		//	full OID: 32 bits.
			uint32	getID()		const;		//	object ID: 24 bits.
			uint8	getNID()	const;		//	0x80 for constant objects.
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
