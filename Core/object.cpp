// object.cpp
//
// Author: Eric Nivel
//
//

#include	<memory>
#include	"object.h"
#include	"payload.h"


namespace	mBrane{
	namespace	sdk{

		inline	_Object::_Object():refCount(0){
		}

		inline	_Object::~_Object(){
		}

		inline	void	_Object::incRef(){

			refCount++;
		}

		inline	void	_Object::decRef(){

			if(--refCount==0)
				delete	this;
		}

		////////////////////////////////////////////////////////////////////////////////////

		inline	__P::__P():object(NULL){
		}

		inline	__P::~__P(){
		}

		inline	__P	&__P::operator	=(_Object	*o){

			object=o;
			return	*this;
		}

		inline	__P	&__P::operator	=(_LP	&p){

			object=p.object;
			return	*this;
		}

		inline	__P	&__P::operator	=(_PP	&p){

			object=p.object;
			return	*this;
		}

		////////////////////////////////////////////////////////////////////////////////////

		inline	_LP::_LP():__P(){
		}

		inline	_LP::~_LP(){
		}

		inline	_LP::operator	_Object	*(){

			return	object;
		}

		inline	_LP	&_LP::operator	=(_Object	*o){

			__P::operator	=(o);
			return	*this;
		}

		inline	_LP	&_LP::operator	=(_LP	&p){

			__P::operator	=(p);
			return	*this;
		}

		inline	_LP	&_LP::operator	=(_PP	&p){

			__P::operator	=(p);
			return	*this;
		}
	}
}