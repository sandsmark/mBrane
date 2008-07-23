// object.tpl.cpp
//
// Author: Eric Nivel
//
//

namespace	mBrane{
	namespace	sdk{

		template<class	C,class	Pointer>	inline	SP<C,Pointer>::SP(){
		}

		template<class	C,class	Pointer>	inline	SP<C,Pointer>::~SP(){
		}

		template<class	C,class	Pointer>	inline	C	*SP<C,Pointer>::operator	->()	const{

			return	(C	*)object;
		}

		template<class	C,class	Pointer>	inline	bool	SP<C,Pointer>::operator	==(C	*c)	const{

			return	object==c;
		}

		template<class	C,class	Pointer>	inline	bool	SP<C,Pointer>::operator	!=(C	*c)	const{

			return	object!=c;
		}

		template<class	C,class	Pointer>	inline	SP<C,Pointer>&	SP<C,Pointer>::operator	=(C	*c){

			if(object)
				object->decRef();
			Pointer::operator	=(c);
			object->incRef();
			return	*this;
		}

		////////////////////////////////////////////////////////////////////////////////////

		template<class	M,class	S,class	U>	M	*Object<M,S,U>::_Allocator=M::Get(sizeof(U));

		template<class	M,class	S,class	U>	inline	Object<M,S,U>::Object():S(){
		}

		template<class	M,class	S,class	U>	inline	Object<M,S,U>::~Object(){
		}

		template<class	M,class	S,class	U>	inline	void	*Object<M,S,U>::operator new(size_t s){

			return	_Allocator->alloc();
		}

		template<class	M,class	S,class	U>	inline	void	Object<M,S,U>::operator delete(void *o){

			_Allocator->dealloc(o);
		}

		////////////////////////////////////////////////////////////////////////////////////

		template<class	C>	inline	_ObjectAdapter<C>::_ObjectAdapter():_Object(),C(){
		}

		template<class	C>	inline	_ObjectAdapter<C>::~_ObjectAdapter(){
		}

		////////////////////////////////////////////////////////////////////////////////////

		template<class	C,class	M,class	U>	inline	ObjectAdapter<C,M,U>::ObjectAdapter():Object<M,_Object,U>(),C(){
		}

		template<class	C,class	M,class	U>	inline	ObjectAdapter<C,M,U>::~ObjectAdapter(){
		}
	}
}