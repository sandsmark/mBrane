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

#ifndef mBrane_sdk_payload_utils_h
#define mBrane_sdk_payload_utils_h

#include	"message.h"


namespace	mBrane{
	namespace	sdk{
		namespace	payloads{
/*
			template<typename	T>	class	Array;
			//	Leads a contiguous raw data storage; the allocated size is not sizeof(Storage<T>), but size; size <=2^30-1
			//	_metaData is [size(30)|allocation scheme(2)]
			//	CoreSize is sizeof(Storage<T>); Object<>::CoreSize() is used
			//	Storage<T> is registered in the ClassRegister (allocator=Storage<T>::New)
			//	Specialize for any particular T holding pointers: redefine ptrCount, getPtr and setPtr
			template<typename	T>	class	Storage:
			public	RPayload<Storage<T>,Memory>{
			template<typename	TA>	friend	class	Array;
			private:
				uint32_t normalizedSize;	//	currently allocated size, in bytes; normalized, i.e. _size = 64*s where s is a power of 2
				uint32_t count;	//	current max index; maintained by Array<T>
				T		*data;	//	points to data+sizeof(T*)
				void	setNormalizedSize(uint32_t size);
			public:
				static	void	*New(uint32_t size);	//	to initialize the _vftable on recv()
				Storage();
				~Storage();
				void	*operator	new(size_t	s,uint32_t size,uint32_t &normalizedSize);
				void	operator	delete(void	*storage);
				uint32_t getNormalizedSize();
				T		&operator	[](uint32_t i);
				size_t	size()	const;
			};
 
			//	Specialization for pointer storage
			//	T must be a subclass of __Payload
			template<typename	T>	class	Storage<P<T> >:
			public	RPayload<Storage<T>,Memory>{
			template<typename	TA>	friend	class	Array;
			private:
				uint32_t normalizedSize;
				uint32_t count;
				P<T>	*data;
				void	setNormalizedSize(uint32_t size);
			public:
				static	void	*New(uint32_t size);
				Storage();
				~Storage();
				void	*operator	new(size_t	s,uint32_t size,uint32_t &normalizedSize);
				void	operator	delete(void	*storage);
				uint32_t getNormalizedSize();
				P<T>	&operator	[](uint32_t i);
				size_t	size()	const;
				uint16_t 	ptrCount()	const;
				__Payload	*getPtr(uint16_t i)	const;
				void		setPtr(uint16_t i,__Payload	*p);
			};

			//	To embed in a payload
			template<typename	T>	class	Array{
			protected:
				uint32_t 		_maxCount;	//	capacity as a number of objects
				P<Storage<T> >	_data;		//	contiguous data storage allocated via Memory::GetDynamic(needed_size)->alloc()
			public:
				Array();
				~Array();
				void	ensure(uint32_t count);	//	number of objects
				uint32_t count()	const;			//	number of objects
				T		&operator	[](uint32_t i);		//	reallocates if necessary (using ensure())
				uint8_t *asBytes(uint32_t i);				//	returns the raw data from (and including) i (an index on Ts, not on bytes); no capacity check
				//	functions to be invoked by the embedding payload class
				uint16_t 	ptrCount()	const;
				__Payload	*getPtr(uint16_t i)	const;
				void		setPtr(uint16_t i,__Payload	*p);
			};*/
		}
	}
}


#include	"payload_utils.tpl.cpp"


#endif
