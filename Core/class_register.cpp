//	class_register.cpp
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

#include	<memory>
#include	"class_register.h"


namespace	mBrane{
	namespace	sdk{

		Array<ClassRegister>	ClassRegister::Classes;

		const	uint16	ClassRegister::NoClass=0xFFFF;

		inline	ClassRegister	*ClassRegister::Get(uint16	CID){

			return	Classes.get(CID);
		}

		inline	uint16	ClassRegister::Count(){

			return	(uint16)Classes.count();
		}

		const	uint16	ClassRegister::Load(const char *className){

			static	uint16	I=0;
			strcpy(Get(I)->class_name,className);
			return	I++;
		}

		const	uint16	ClassRegister::GetCID(const	char	*className){

			for(uint16	i=0;i<Classes.count();i++)
				if(strcmp(Classes.get(i)->class_name,className)==0)
					return	i;
			return	NoClass;
		}

		ClassRegister::ClassRegister():_allocator(NULL){
		}

		ClassRegister::~ClassRegister(){
		}

		inline	ClassRegister::Allocator	ClassRegister::allocator()	const{

			return	_allocator;
		}

		inline	size_t	ClassRegister::size()	const{

			return	_size;
		}

		inline	size_t	ClassRegister::coreSize()	const{

			return	_coreSize;
		}

		inline	size_t	ClassRegister::offset()	const{

			return	_offset;
		}

		inline	uint8	ClassRegister::ptrCount()	const{

			return	_ptrCount;
		}

		inline	P<_RPayload>	*ClassRegister::ptr(__Payload	*p,uint8	i){

			return	_ptr(p,i);
		}
	}
}