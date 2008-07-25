// object.cpp
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

#include	<memory>
#include	"object.h"
#include	"payload.h"


namespace	mBrane{
	namespace	sdk{

		inline	_Object::_Object():refCount(0){
		}

		inline	_Object::~_Object(){
		}

		void	_Object::incRef(){

			refCount++;
		}

		inline	void	_Object::decRef(){

			if(--refCount==0)
				delete	this;
		}

		////////////////////////////////////////////////////////////////////////////////////

		inline	__P::__P():object(NULL){
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