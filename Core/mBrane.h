// mBrane.h
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

#ifndef	mBrane_h
#define	mBrane_h

#include	"memory.h"
#include	"crank.h"
#include	"control_messages.h"


#define	CLASS(C)	\
			case	__COUNTER__:	\
				if(preview)	\
					return	((U	*)this)->preview((C	*)p);	\
				else{	\
					((U	*)this)->process((C	*)p);	\
					return	true;	\
				}

#define	USER_CLASSES_BEGIN	\
template<class	U>	class	Crank:	\
public	mBrane::sdk::_Crank{	\
protected:	\
	static	mBrane::uint16	_CID;	\
	Crank(uint16	_ID):_Crank(_ID){}	\
public:	\
	static	U	*New(uint16	_ID){	return	new	U(_ID);	}	\
	virtual	~Crank(){}	\
	uint16	cid()	const{	return	_CID;	}	\
	bool	notify(_Payload	*p,bool	preview=false){	\
		switch(p->cid()){
			//	TODO:	follow by control message class processing as for user defined message classes: CLASS(control_message_class)

#define	USER_CLASSES_END	\
			default:	return	true;	\
		}	\
	}	\
};	\
template<class	U>	mBrane::uint16	Crank<U>::_CID=CrankRegister::Load(New);


#endif