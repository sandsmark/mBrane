//	object.h
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

#ifndef mBrane_sdk_object_h
#define mBrane_sdk_object_h

#include	<cstdlib>

#include	"types.h"
#include	"base.h"
#include	"config.h"


using	namespace	core;

namespace	mBrane{
	namespace	sdk{

		class	ClassRegister;
		//	Standard base class for all objects in mBrane.
		//	M: memory allocator class.
		//	S: base class.
		//	U: derived class.
		//	Usage:	template<class	C>	class	DaughterClass:public	Object<Memory,_Object,C>{ ... };
		//			class _DaughterClass:public DaughterClass<_DaughterClass>{};
		//			NB: Memory can be any Allocator class
		template<class	M,class	S,class	U>	class	Object:
		public	S{
		friend	class	ClassRegister;
		private:
			static	M	*_Allocator;
		protected:
			Object();
		public:
			static	size_t		Size();		//	Total size of an instance as defined by its class
			void	*operator	new(size_t	s);
			void	operator	delete(void	*o);
		};

		
		//	Template version of the well-known DP. Adapts C to Object<M,_Object,U>.
		template<class	C,class	M,class	U>	class	ObjectAdapter:
		public	C,
		public	Object<M,_Object,U>{
		protected:
			ObjectAdapter();
		};
	}
}


#include	"object.tpl.cpp"


#endif
