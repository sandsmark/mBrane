//	application.h
//
//	Author: Eric Nivel
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

#ifndef	_application_h_
#define	_application_h_

#define	MBRANE_MESSAGE_CLASSES	"mBrane_message_classes.h"

#define	MBRANE_MESSAGE_CLASS(C)	static	const	uint16	C##_class=__COUNTER__;
#define	APPLICATION_CLASS(C)	MBRANE_MESSAGE_CLASS(C)
#include	MBRANE_MESSAGE_CLASSES
#include	APPLICATION_CLASSES

//	for use in switches (instead of user_class::CID())
#define	CLASS_ID(C)	C##_class

template<class	U>	class	Crank:	//	TODO:	in notify and preview switches: insert control message class processing
public	crank::_Crank{
protected:
	static	const	uint16	_CID;
	Crank(uint16	ID):crank::_Crank(ID){}
public:
	static	crank::_Crank	*New(uint16	ID){	return	new	U(ID);	}
	virtual	~Crank(){}
	const	uint16	cid(){	return	_CID;	}
	void	notify(_Payload	*p){
		switch(p->cid()){
		#define	MBRANE_MESSAGE_CLASS(C)	case	CLASS_ID(C):	((U	*)this)->process((C	*)p);	return;
		#define	APPLICATION_CLASS(C)	MBRANE_MESSAGE_CLASS(C)
		#include	MBRANE_MESSAGE_CLASSES
		#include	APPLICATION_CLASSES
		default:	return;
		}
	}
	bool	preview(_Payload	*p){
		switch(p->cid()){
		#define	MBRANE_MESSAGE_CLASS(C)	case	CLASS_ID(C):	return	((U	*)this)->preview((C	*)p);
		#define	APPLICATION_CLASS(C)	MBRANE_MESSAGE_CLASS(C)
		#include	MBRANE_MESSAGE_CLASSES
		#include	APPLICATION_CLASSES
		default:	return	false;
		}
	}
};
template<class	U>	uint16	Crank<U>::_CID=CrankRegister::Load(New);


#endif