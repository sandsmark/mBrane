// dynamic_class_loader.tpl.cpp
//
// Author: Eric Nivel
//
//	BSD license:
//	Copyright (c) 2008, Eric Nivel, Thor List
//	All rights reserved.
//	Redistribution and use in source and binary forms, with or without
//	modification, are permitted provided that the following conditions are met:
//
//   - Redistributions of source code must retain the above copyright
//     notice, this list of conditions and the following disclaimer.
//   - Redistributions in binary form must reproduce the above copyright
//     notice, this list of conditions and the following disclaimer in the
//     documentation and/or other materials provided with the distribution.
//   - Neither the name of Eric Nivel, Thor List nor the
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

namespace	mBrane{
	namespace	sdk{

		template<class	C>	DynamicClassLoader<C>	*DynamicClassLoader<C>::New(XMLNode	&n){

			const	char	*l=n.getAttribute("shared_library");
			if(!l){

				std::cout<<"Error: "<<n.getName()<<"::shared_library is missing\n";
				return	NULL;
			}
			SharedLibrary	*library;
			if(library=SharedLibrary::New(l)){

				C::Load	load;
				if(!(load=library->getFunction<C::Load>("Load"))){

					std::cout<<"Error: "<<n.getName()<<": could not find function Load\n";
					return	NULL;
				}
				return	new	DynamicClassLoader(library,load);
			}
			return	NULL;
		}

		template<class	C>	DynamicClassLoader<C>::DynamicClassLoader(SharedLibrary	*library,typename	C::Load	load):library(library),load(load){
		}

		template<class	C>	DynamicClassLoader<C>::~DynamicClassLoader(){

			if(library)
				delete	library;
		}

		template<class	C>	C	*DynamicClassLoader<C>::getInstance(XMLNode	&n,NodeAPI	*node){

			if(load)
				return	load(n,node);
			return	NULL;
		}
	}
}