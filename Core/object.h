// object.h
//
// Author: Eric Nivel
//
//

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
			virtual	~Object();
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
			virtual	~_Object();
		};

		template<class	C>	class	_ObjectAdapter:
		public	C,
		public	_Object{
		protected:
			_ObjectAdapter();
			virtual	~_ObjectAdapter();
		};

		template<class	C,class	M,class	U>	class	ObjectAdapter:
		public	C,
		public	Object<M,_Object,U>{
		protected:
			ObjectAdapter();
			virtual	~ObjectAdapter();
		};

		class	_LP;
		class	_PP;
		class	dll	__P{	//	lazy pointer
		protected:
			_Object	*object;
			__P();
			virtual	~__P();
			__P	&operator	=(_Object	*o);
			__P	&operator	=(_LP	&p);
			__P	&operator	=(_PP	&p);
		};

		class	dll	_LP:	//	lazy pointer on _Object
		public	__P{
		protected:
			_LP();
			virtual	~_LP();
			operator	_Object	*();
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