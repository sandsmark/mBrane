//	payload_utils.h
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

#ifndef mBrane_sdk_payload_utils_h
#define mBrane_sdk_payload_utils_h

#include	"message.h"


namespace	mBrane{
	namespace	sdk{
		namespace	payloads{

			template<typename	T>	class	Array;
			//	Leads a contiguous raw data storage; the allocated size is not sizeof(Storage<T>), but size; size <=2^30-1
			//	_metaData is [size(30)|allocation scheme(2)]
			//	CoreSize is sizeof(Storage<T>); Object<>::CoreSize() is used
			//	Storage<T> is registered in the ClassRegister (allocator=Storage<T>::New)
			//	Specialize for any particular T holding pointers: redefine ptrCount, getPtr and setPtr
			template<typename	T>	class	Storage:
			public	RPayload<Storage<T>,RawStorage,Memory>{
			template<typename	TA>	friend	class	Array;
			private:
				uint32	size;	//	currently allocated size, in bytes; normalized, i.e. _size = 64*s where s is a power of 2
				uint32	count;	//	current max index; maintained by Array<T>
				T		*data;	//	points to data+sizeof(T*)
				void	setSize(uint32	size);
			public:
				static	void	*New(uint32	size);	//	to initialize the _vftable on recv()
				Storage();
				~Storage();
				void	*operator	new(size_t	s,uint32	size,uint32	&normalizedSize);
				void	operator	delete(void	*storage);
				uint32	getSize();
				T		&operator	[](uint32	i);
				size_t	dynamicSize()	const;
			};
 
			//	Specialization for pointer storage
			//	T must be a subclass of __Payload
			template<typename	T>	class	Storage<P<T> >:
			public	RPayload<Storage<T>,RawStorage,Memory>{
			template<typename	TA>	friend	class	Array;
			private:
				uint32	size;
				uint32	count;
				P<T>	*data;
				void	setSize(uint32	size);
			public:
				static	void	*New(uint32	size);
				Storage();
				~Storage();
				void	*operator	new(size_t	s,uint32	size,uint32	&normalizedSize);
				void	operator	delete(void	*storage);
				uint32	getSize();
				P<T>	&operator	[](uint32	i);
				size_t	dynamicSize()	const;
				uint16		ptrCount()	const;
				__Payload	*getPtr(uint16	i)	const;
				void		setPtr(uint16	i,__Payload	*p);
			};

			//	To embed in a payload
			template<typename	T>	class	Array{
			protected:
				uint32			_maxCount;	//	capacity as a number of objects
				P<Storage<T> >	_data;		//	contiguous data storage allocated via Memory::Get(needed_size)->alloc()
			public:
				Array();
				~Array();
				void	ensure(uint32	count);	//	number of object
				uint32	count()	const;
				T		&operator	[](uint32	i);		//	reallocates if necessary (ensure())
				uint8	*asBytes(uint32	i);				//	returns the raw data from (and including) i (an index on Ts, not on bytes); no capacity check
				Array<T>	&pushBack(const	T	&t);	//	convenience
				//	functions to be invoked by the embedding payload class to have _data point to a storage and update Storage<T>::count
				uint16		ptrCount()	const;
				__Payload	*getPtr(uint16	i)	const;
				void		setPtr(uint16	i,__Payload	*p);
			};
		}
	}
}


#include	"payload_utils.tpl.cpp"


#endif
