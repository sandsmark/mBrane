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
	namespace	sdk{

		template<class	C,class	Pointer>	class	SP:	//	smart pointer: no circular refs (use std c++ ptrs), no passing in functions (automatic cast of P<C> to C*), nor can it be a return value (return C* instead)
		public	Pointer{
		private:
		public:
			SP();
			~SP();
			C	*operator	->()	const;
			template<class	D>	operator	D	*()	const{

				return	(D	*)object;
			}
			bool	operator	==(C	*c)	const;
			bool	operator	!=(C	*c)	const;
			template<class	T>	bool	operator	==(SP<C,T>	&p)	const{

				return	object==p.object;
			}
			template<class	T>	bool	operator	!=(SP<C,T>	&p)	const{

				return	object!=p.object;
			}
			SP<C,Pointer>	&operator	=(C	*c);
			template<class	T>	SP<C,Pointer>	&operator	=(SP<C,T>	&p){

				if(object)
					object->decRef();

				Pointer::operator	=(p.object);
				object->incRef();
				return	*this;
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
			~Object();
		public:
			void	*operator	new(size_t	s);
			void	operator	delete(void	*o);
		};

		//	Usage:	template<class	C>	class	DaughterClass:public	Object<Memory,_Object,C>{ ... };
		//			class _DaughterClass:public DaughterClass<_DaughterClass>{};
		//			NB: Memory can be any Allocator class

		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		class	_Object{
		private:
			uint32	refCount;
			void	incRef();
			void	decRef();
		protected:
			_Object();
			~_Object();
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
		class	__P{	//	lazy pointer on _Object
		protected:
			_Object	*object;
			__P();
			~__P();
			__P	&operator	=(_Object	*o);
			__P	&operator	=(_LP	&p);
			__P	&operator	=(_PP	&p);
		};

		class	_LP:	//	local pointer
		public	__P{
		protected:
			_LP();
			~_LP();
		public:
			operator	_Object	*();
			_LP	&operator	=(_Object	*o);
			_LP	&operator	=(_LP	&p);
			_LP	&operator	=(_PP	&p);	//	asymetrical: there is no _PP::operator =(_LP&)
		};

		template<class	C>	class	P:public	SP<C,_LP>{};
	}
}


#include	"object.tpl.cpp"


#endif