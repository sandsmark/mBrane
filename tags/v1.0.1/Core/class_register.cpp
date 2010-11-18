//	class_register.cpp
//
//	Author: Eric Nivel
//
//	BSD license:
//	Copyright (c) 2010, Eric Nivel
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
#include	<cstring>
#include	"class_register.h"


namespace	mBrane{
	namespace	sdk{

		Array<ClassRegister,128>	*ClassRegister::Classes=NULL;

		Array<ClassRegister,128>	*ClassRegister::Get(){

			if(!Classes)
				Classes=new	Array<ClassRegister,128>();
			return	Classes;
		}

		const	uint16	ClassRegister::NoClass=0xFFFF;

		inline	ClassRegister	*ClassRegister::Get(uint16	CID){

			return	Classes->get(CID);
		}

		inline	uint16	ClassRegister::Count(){

			return	(uint16)Classes->count();
		}

		uint16	ClassRegister::Load(const char *className){

			static	uint16	I=0;
			strcpy(Get(I)->class_name,className);
			return	I++;
		}

		uint16	ClassRegister::GetCID(const	char	*className){

			for(uint16	i=0;i<Classes->count();i++)
				if(strcmp(Classes->get(i)->class_name,className)==0)
					return	i;
			return	NoClass;
		}

		ClassRegister::ClassRegister():_allocator(NULL){
		}

		ClassRegister::~ClassRegister(){
		}

		ClassRegister::Allocator	ClassRegister::allocator()	const{

			return	_allocator;
		}

		size_t	ClassRegister::offset()	const{

			return	_offset;
		}

		void	ClassRegister::Cleanup(){

			delete	Classes;
		}
	}
}
