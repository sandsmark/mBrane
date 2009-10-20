//	module_register.h
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

#ifndef mBrane_sdk_module_register_h
#define mBrane_sdk_module_register_h

#include	"array.h"
#include	"module.h"


namespace	mBrane{
	namespace	sdk{

		//	Allows to retrieve module class data from module class IDs.
		//	Allows to retrieve module class IDs from module class names.
		class	dll	ModuleRegister{
		public:
			typedef	module::_Module	*(*ModuleBuilder)();
		private:
			static	Array<ModuleRegister,1024>	*Modules;
			static	Array<ModuleRegister,1024>	*Get();
			//	Module class data
			ModuleBuilder	_builder;
			char			class_name[255];
		public:
			static	uint16	Load(ModuleBuilder	b,const	char	*className);
			static	ModuleRegister	*Get(uint16	CID);
			static	uint16	GetCID(const	char	*className);
			static	uint16	Count();
			ModuleRegister();
			~ModuleRegister();
			module::_Module	*buildModule()	const;
			const	char	*name()	const;

			static	void	Cleanup();
		};
	}
}


#endif
