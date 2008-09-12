//	application.h
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

#ifndef	_application_h_
#define	_application_h_

#include	"module_register.h"
#include	"class_register.h"


#define	MBRANE_MESSAGE_CLASSES	"mBrane_message_classes.h"

#define	MBRANE_MESSAGE_CLASS(C)		static	const	uint16	C##_class=__COUNTER__;
#define	MBRANE_STREAM_DATA_CLASS(C)	static	const	uint16	C##_class=__COUNTER__;
#ifndef	LIBRARY_CLASSES
	#include	MBRANE_MESSAGE_CLASSES
#endif
#include	APPLICATION_CLASSES

//	for use in switches (instead of the non constant expression user_class::CID())
#define	CLASS_ID(C)	C##_class

template<class	U>	class	Module:
public	Object<Memory,module::_Module,Module<U> >{
protected:
	static	const	uint16	_CID;
	Module(bool	canBeSwapped=true,bool	canMigrate=true):Object<Memory,module::_Module,Module<U> >(){
		_canBeSwapped=canBeSwapped;
		_canMigrate=_canMigrate;
	}
public:
	static	const	uint16	CID(){	return	_CID;	}
	static	module::_Module	*New(){	return	new	U();	}
	virtual	~Module(){}
	void	notify(_Payload	*p){
		switch(p->cid()){
		#define	MBRANE_MESSAGE_CLASS(C)	case	CLASS_ID(C):	((U	*)this)->react((C	*)p);	return;
		#define	MBRANE_STREAM_DATA_CLASS(C)
		#if defined	LIBRARY_CLASSES
			#include	LIBRARY_CLASSES	
		#else
			#include	MBRANE_MESSAGE_CLASSES
		#endif
		#include	APPLICATION_CLASSES
		default:	return;
		}
	}
	void	notify(uint16	sid,_Payload	*p){
		switch(p->cid()){
		#define	MBRANE_MESSAGE_CLASS(C)
		#define	MBRANE_STREAM_DATA_CLASS(C)	case	CLASS_ID(C):	((U	*)this)->react(sid,(C	*)p);	return;
		#if defined	LIBRARY_CLASSES
			#include	LIBRARY_CLASSES	
		#else
			#include	MBRANE_MESSAGE_CLASSES
		#endif
		#include	APPLICATION_CLASSES
		default:	return;
		}
	}
	module::_Module::Decision	dispatch(_Payload	*p){
		switch(p->cid()){
		#define	MBRANE_MESSAGE_CLASS(C)		case	CLASS_ID(C):	return	((U	*)this)->decide((C	*)p);
		#define	MBRANE_STREAM_DATA_CLASS(C)	case	CLASS_ID(C):	return	((U	*)this)->decide((C	*)p);
		#if defined	LIBRARY_CLASSES
			#include	LIBRARY_CLASSES	
		#else
			#include	MBRANE_MESSAGE_CLASSES
		#endif
		#include	APPLICATION_CLASSES
		default:	return	DISCARD;
		}
	}
};
template<class	U>	const	uint16	Module<U>::_CID=ModuleRegister::Load(New,U::ClassName);


//	for retrieving CIDs from names (in specs)
#define	MBRANE_MESSAGE_CLASS(C)		static	const	uint16	C##_name=ClassRegister::Load(#C);
#define	MBRANE_STREAM_DATA_CLASS(C)	static	const	uint16	C##_name=ClassRegister::Load(#C);
#ifndef	LIBRARY_CLASSES
	#include	MBRANE_MESSAGE_CLASSES
#endif
#include	APPLICATION_CLASSES


#define	MODULE_CLASS_BEGIN(C,S)	\
class	C:	\
public S{	\
public:	\
	static	const	char	*ClassName;	\
	C():S(){}	\
	~C(){}

#define	MODULE_CLASS_END(C)	\
};	\
const	char	*C::ClassName=#C;	\
extern	"C"{	\
	mBrane::sdk::module::_Module *	__cdecl	New##C(){	return	new	C();	}	\
	const	mBrane::uint16	C##_CID(){	return	C::CID();	}	\
}


#endif