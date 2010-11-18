//	dynamic_class_loader.h
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

#ifndef	mBrane_sdk_dynamic_class_loader_h
#define	mBrane_sdk_dynamic_class_loader_h

#include	"utils.h"
#include	"xml_parser.h"
#include	"mdaemon_node.h"


namespace	mBrane{
	namespace	sdk{

		namespace	mdaemon{
			class	Node;
		}

		//	Used for loading daemon and network interfaces classes.
		//	Can be used for any other class provided in a sharedLibrary exhibiting a static Load function, e.g.
		//	extern	"C"{
		//	AnyClass	dll_export	*Load(XMLNode	&n,mdaemon::Node	*node);
		//	}
		template<class	C>	class	DynamicClassLoader{
		private:
			SharedLibrary	*library;
			typename	C::Load	load;
			DynamicClassLoader(SharedLibrary	*library,typename	C::Load	load);
		public:
			static	DynamicClassLoader	*New(XMLNode	&n);
			~DynamicClassLoader();
			C	*getInstance(XMLNode	&n,mdaemon::Node	*node);
		};
	}
}


#include	"dynamic_class_loader.tpl.cpp"


#endif
