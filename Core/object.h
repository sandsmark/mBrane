// object.h
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

#ifndef mBrane_sdk_object_h
#define mBrane_sdk_object_h

#include	<cstdlib>

#include	"types.h"


namespace	mBrane{
	namespace	node{	class	Node;	}
	namespace	sdk{

		template<class	C,class	Pointer>	class	SP:	//	smart pointer: no circular refs (use std c++ ptrs), no passing in functions (cast P<C> into C*), nor can it be a return value (return C* instead)
		public	Pointer{
		protected:
			SP();
			SP(C	*o);
			virtual	~SP();
		public:
			C	*operator	->()	const;
			template<class	D>	operator	D	*()	const{

				return	(D	*)object;
			}
			bool	operator	==(C	*c)	const;
			bool	operator	!=(C	*c)	const;
			template<class	D,class	T>	bool	operator	==(SP<D,T>	&p)	const{

				return	object==p.object;
			}
			template<class	D,class	T>	bool	operator	!=(SP<D,T>	&p)	const{

				return	object!=p.object;
			}
			SP<C,Pointer>	&operator	=(C	*c);
			template<class	D,class	T>	SP<C,Pointer>	&operator	=(SP<D,T>	&p){

				return	this->operator	=(p.object);
			}
		};

		class	ClassRegister;
		template<class	M,class	S,class	U>	class	Object:
		public	S{
		friend	class	ClassRegister;
		private:
			static	M	*_Allocator;
		protected:
			Object();
		public:
			void	*operator	new(size_t	s);
			void	operator	delete(void	*o);
		};

		//	Usage:	template<class	C>	class	DaughterClass:public	Object<Memory,_Object,C>{ ... };
		//			class _DaughterClass:public DaughterClass<_DaughterClass>{};
		//			NB: Memory can be any Allocator class

		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		class	Node;
		class	dll	_Object{
		friend	class	mBrane::node::Node;
		template<class	C,class	Pointer>	friend	class	SP;
		private:
			uint32	refCount;
			void	incRef();
			void	decRef();
		protected:
			_Object();
		public:
			virtual	~_Object();
		};

		template<class	C>	class	_ObjectAdapter:
		public	C,
		public	_Object{
		protected:
			_ObjectAdapter();
		};

		template<class	C,class	M,class	U>	class	ObjectAdapter:
		public	C,
		public	Object<M,_Object,U>{
		protected:
			ObjectAdapter();
		};

		class	_LP;
		class	_PP;
		class	dll	__P{	//	lazy pointer
		protected:
			_Object	*object;
			__P();
			__P	&operator	=(_Object	*o);
			__P	&operator	=(_LP	&p);
			__P	&operator	=(_PP	&p);
		};

		class	dll	_LP:	//	lazy pointer on _Object
		public	__P{
		protected:
			_LP();
			_LP	&operator	=(_Object	*o);
			_LP	&operator	=(_LP	&p);
			_LP	&operator	=(_PP	&p);	//	asymetrical: there is no _PP::operator =(_LP&)
		};

		template<class	C>	class	P:public	SP<C,_LP>{
		public:
			using	SP<C,_LP>::operator =;
			P();
			P(C	*o);
			~P();
		};
	}
}


#include	"object.tpl.cpp"


#endif