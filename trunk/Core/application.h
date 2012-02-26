//	application.h
//
//	Author: Eric Nivel
//
//	BSD license:
//	Copyright (c) 2010, Eric Nivel
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
#include	"module_node.h"
#include	"module.h"


#define	MBRANE_MESSAGE_CLASSES	"mBrane_message_classes.h"

//	C##_CID needed for use in switches (instead of the non constant expression user_class::CID())
//	the initialization of C##_metaData triggers the intialization of  C::_MetaData
#define	MBRANE_MESSAGE_CLASS(C)		static	const	core::uint16	C##_CID=(core::uint16)__COUNTER__;static	const	uint64	C##_metaData=ClassRegister::Load<C>(C##_CID);
#define	MBRANE_STREAM_DATA_CLASS(C)	static	const	core::uint16	C##_CID=(core::uint16)__COUNTER__;static	const	uint64	C##_metaData=ClassRegister::Load<C>(C##_CID);
#include	APPLICATION_CLASSES

#define	CLASS_ID(C)	C##_CID


template<class	U>	class	Module:
public	mBrane::sdk::Object<Memory,module::_Module,U>{
protected:
	static	const	core::uint16	_CID;
	Module(bool	canMigrate=true):Object<Memory,module::_Module,U>(){
		this->_canMigrate=canMigrate;
	}
public:
	static	const	core::uint16	CID(){	return	_CID;	}
	static	module::_Module	*New(){	return	new	U();	}
	virtual	~Module(){}
	void	_start(){
		this->_ready=true;
		((U	*)this)->start();
		module::Node::Get()->trace(module::Node::EXECUTION)<<"> Info: Module "<<this->_cid<<"|"<<this->_id<<" started"<<std::endl;
	}
	void	_stop(){
		((U	*)this)->stop();
		this->_ready=false;
		module::Node::Get()->trace(module::Node::EXECUTION)<<"> Info: Module "<<this->_cid<<"|"<<this->_id<<" stopped"<<std::endl;
	}
	void	notify(_Payload	*p){
		switch(p->cid()){
		#undef MBRANE_MESSAGE_CLASS
		#undef MBRANE_STREAM_DATA_CLASS
		#define	MBRANE_MESSAGE_CLASS(C)	case	CLASS_ID(C):	((U	*)this)->react((C	*)p);	return;
		#define	MBRANE_STREAM_DATA_CLASS(C)
		#include "classes.h"
		default:	return;
		}
	}
	void	notify(core::uint16	sid,_Payload	*p){
		switch(p->cid()){
		#undef MBRANE_MESSAGE_CLASS
		#undef MBRANE_STREAM_DATA_CLASS
		#define	MBRANE_MESSAGE_CLASS(C)
		#define	MBRANE_STREAM_DATA_CLASS(C)	case	CLASS_ID(C):	((U	*)this)->react(sid,(C	*)p);	return;
		#include "classes.h"
		default:	return;
		}
	}
	module::_Module::Decision	dispatch(_Payload	*p){
		switch(p->cid()){
		#undef MBRANE_MESSAGE_CLASS
		#undef MBRANE_STREAM_DATA_CLASS
		#define	MBRANE_MESSAGE_CLASS(C)		case	CLASS_ID(C):	return	((U	*)this)->decide((C	*)p);
		#define	MBRANE_STREAM_DATA_CLASS(C)	case	CLASS_ID(C):	return	((U	*)this)->decide((C	*)p);
		#include "classes.h"
		default:	return module::_Module::DISCARD;
		}
	}
};
template<class	U>	const	core::uint16	Module<U>::_CID=ModuleRegister::Load(New,U::ClassName);

//	to use in user module cpp files; forces the intitialization of Module<U>::_CID
#define	LOAD_MODULE(C)	static	core::uint16	cid_##C=C::CID();

//	force _MetaData initilization in user mBrane_dll.
#undef MBRANE_MESSAGE_CLASS
#define	MBRANE_MESSAGE_CLASS(C)		static	const	uint64	C##_force_init=C::_MetaData;
#include	APPLICATION_CLASSES

//	for retrieving CIDs from names (in specs)
#undef MBRANE_MESSAGE_CLASS
#undef MBRANE_STREAM_DATA_CLASS
#define	MBRANE_MESSAGE_CLASS(C)		static	const	core::uint16	C##_name=ClassRegister::Load(#C);
#define	MBRANE_STREAM_DATA_CLASS(C)	static	const	core::uint16	C##_name=ClassRegister::Load(#C);
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
	~C(){}	\
	Decision	decide(KillModule	*p){	\
		return	WAIT;	\
	}	\
	void	react(KillModule	*p){	\
		delete	this;	\
	}

#define	MODULE_CLASS_END(C)	\
};	\
const	char	*C::ClassName=#C;	\
extern	"C"{	\
	mBrane::sdk::module::_Module *	cdecl	New##C(){	return	new	C();	}	\
	const	core::uint16	C##_CID(){	return	C::CID();	}	\
}


#endif