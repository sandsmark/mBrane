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

		_Object::_Object():refCount(0){
		}

		_Object::~_Object(){
		}

		void	_Object::incRef(){

			refCount++;
		}

		void	_Object::decRef(){

			if(--refCount==0)
				delete	this;
		}

		////////////////////////////////////////////////////////////////////////////////////

		__P::__P():object(NULL){
		}

		__P::~__P(){
		}

		__P	&__P::operator	=(_Object	*o){

			object=o;
			return	*this;
		}

		__P	&__P::operator	=(_LP	&p){

			object=p.object;
			return	*this;
		}

		__P	&__P::operator	=(_PP	&p){

			object=p.object;
			return	*this;
		}

		////////////////////////////////////////////////////////////////////////////////////

		_LP::_LP():__P(){
		}

		_LP::~_LP(){
		}

		_LP::operator	_Object	*(){

			return	object;
		}

		_LP	&_LP::operator	=(_Object	*o){

			__P::operator	=(o);
			return	*this;
		}

		_LP	&_LP::operator	=(_LP	&p){

			__P::operator	=(p);
			return	*this;
		}

		_LP	&_LP::operator	=(_PP	&p){

			__P::operator	=(p);
			return	*this;
		}
	}
}