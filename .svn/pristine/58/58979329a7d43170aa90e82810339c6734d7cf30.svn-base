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

#ifndef	mBrane_sdk_message_h
#define	mBrane_sdk_message_h

#include	"payload.h"
#include	"memory.h"
#include	"module_node.h"


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

			//	Shared objects are cached. The first 32 bits of their metadata is NID(7 bits)|ID(24 bits). The most significant bit of the OID is 0 (1 for constants).
			//	Shared objects are supposed not to change once created.
			//	Constant objects are duplicated on each node.
			template<class	U,class	M>	class	SharedObject:
			public	Message<U,M>{
			protected:
				void	decRef();	//	notifies the node when the ref count drops to 1: doomed for deletion.
				SharedObject();
			public:
				virtual	~SharedObject();
				bool	isShared();
				bool	isConstant();
			};

			////////////////////////////////////////////////////////////////////////////////////////////////

			//	CStorage stands for contiguous storage.
			//	S. superclass, T: type of the data to be stored contiguously.
			//	Typical use: data compacted dynamically, i.e. whose size is not an integral constant (e.g. that could not parameterize a template), e.g. archives, compressed images, etc.
			//	Usage sample:
			//		template<class	U>	class CoreData:public	Message<U,Memory>{...};
			//		class ACStorage:public CStorage<CoreData<ACStorage,word32> >{...};
			//		ACStorage	*acs=new(32) ACStorage(); // acs contains an array of 32 word32.
			//	Do not declare any data in subclasses of CStorage: such subclasses shall only contain logic, e.g. functions to exploit CoreData (if any) and the array of Ts.
			template<class	S,typename	T>	class	CStorage:
			public	S{
			protected:
				uint32	_size;		//	of the whole instance (not normalized)
				uint32	_capacity;	//	max number of elements in the array
				T		*_data;		//	points to ((T	*)(((uint8	*)this)+offsetof(CStorage<S,T>,_data)+sizeof(T	*)));
				CStorage();
			public:
				static	void		*New(uint32	size);					//	total size of the instance; called upon sending
				void	*operator	new(size_t	s);						//	unused; need for compilation of ___Payload<U,P,M>::New
				void	*operator	new(size_t	s,uint32	capacity);	//	overrides Object<U,M>::new
				void	operator	delete(void	*o);					//	overrides Object<U,M>::delete
				virtual	~CStorage();
				size_t	size()	const;
				uint32	getCapacity()	const;
				T		&data(uint32	i);
				T		&data(uint32	i)	const;
				T		*data();
			};
		}
	}
}


#include	"message.tpl.cpp"


#endif
