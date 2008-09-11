//	class_register.h
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

#ifndef mBrane_sdk_class_register_h
#define mBrane_sdk_class_register_h

#include	"array.h"
#include	"object.h"
#include	"memory.h"


namespace	mBrane{
	namespace	sdk{

		class	ClassRegister;
		template	class	dll	Array<ClassRegister>;

		class	__Payload;
		class	_RPayload;
		class	dll	ClassRegister{
		template<class	Register>	friend	class	Array;
		public:
			typedef	void	*(*Allocator)();
			typedef	P<_RPayload>	*(*Ptr)(__Payload	*,uint8);
		private:
			static	Array<ClassRegister>	Classes;
			Allocator	_allocator;
			size_t		_size;	//	transmission size
			size_t		_coreSize;	//	transmission size for compressed payloads: size of the payload minus the size of the compressed data
			size_t		_offset;	//	from this
			uint8		_ptrCount;
			Ptr			_ptr;
			char		class_name[255];
		public:
			static	const	uint16	NoClass;	//	max: 0xFFFE classes
			template<class	C,class	M>	static	uint32	Load();	//	returns class meta data (Cf payload.h)
			static	const	uint16	Load(const	char	*className);
			static	ClassRegister	*Get(uint16	CID);
			static	const	uint16	GetCID(const	char	*className);
			static	uint16	Count();
			ClassRegister();
			~ClassRegister();
			Allocator		allocator()		const;
			size_t			size()			const;
			size_t			coreSize()		const;
			size_t			offset()		const;
			uint8			ptrCount()		const;
			P<_RPayload>	*ptr(__Payload	*,uint8	i);
		};
	}
}


#include	"class_register.tpl.cpp"


#endif