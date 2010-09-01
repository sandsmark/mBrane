//	module_register.cpp
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
#include	"module_register.h"


namespace	mBrane{
	namespace	sdk{

		Array<ModuleRegister,1024>	*ModuleRegister::Modules=NULL;

		Array<ModuleRegister,1024>	*ModuleRegister::Get(){

			if(!Modules)
				Modules=new	Array<ModuleRegister,1024>();
			return	Modules;
		}

		ModuleRegister	*ModuleRegister::Get(uint16	CID){

			return	Get()->get(CID);
		}

		uint16	ModuleRegister::GetCID(const	char	*className){

			for(uint16	i=0;i<Modules->count();i++)
				if(strcmp(Modules->get(i)->class_name,className)==0)
					return	i;
			return	ClassRegister::NoClass;
		}

		inline	uint16	ModuleRegister::Count(){

			return	(uint16)Modules->count();
		}

		uint16	ModuleRegister::Load(ModuleBuilder	b,const	char	*className){

			ModuleRegister	*r=&Get()->operator	[](Get()->count());
			r->_builder=b;
			strcpy(r->class_name,className);
			return	(uint16)(Modules->count()-1);
		}

		ModuleRegister::ModuleRegister():_builder(NULL){
		}

		ModuleRegister::~ModuleRegister(){
		}

		module::_Module	*ModuleRegister::buildModule()	const{

			return	_builder();
		}

		const	char	*ModuleRegister::name()	const{

			return	class_name;
		}

		void	ModuleRegister::Cleanup(){

			delete	Modules;
		}
	}
}
