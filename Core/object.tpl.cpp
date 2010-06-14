//	object.tpl.cpp
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

namespace	mBrane{
	namespace	sdk{

		template<class	M,class	S,class	U>	M	*Object<M,S,U>::_Allocator=M::GetStatic(U::Size());

		template<class	M,class	S,class	U>	inline	size_t	Object<M,S,U>::Size(){

			return	sizeof(U);
		}

		template<class	M,class	S,class	U>	inline	Object<M,S,U>::Object():S(){
		}

		template<class	M,class	S,class	U>	inline	void	*Object<M,S,U>::operator new(size_t s){

			return	_Allocator->alloc();
		}

		template<class	M,class	S,class	U>	inline	void	Object<M,S,U>::operator delete(void *o){

			_Allocator->dealloc(o);
		}

		////////////////////////////////////////////////////////////////////////////////////

		template<class	C,class	M,class	U>	inline	ObjectAdapter<C,M,U>::ObjectAdapter():Object<M,_Object,U>(),C(){
		}
	}
}
