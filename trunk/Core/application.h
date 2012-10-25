//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
//_/_/
//_/_/ < REPLACE THIS LINE WITH THE NAME OF THE SOFTWARE >
//_/_/
//_/_/ Eric Nivel
//_/_/ Center for Analysis and Design of Intelligent Agents
//_/_/   Reykjavik University, Menntavegur 1, 101 Reykjavik, Iceland
//_/_/   http://cadia.ru.is
//_/_/ Copyright?2012
//_/_/
//_/_/ This software was developed by the above copyright holder as part of 
//_/_/ the HUMANOBS EU research project, in collaboration with the 
//_/_/ following parties:
//_/_/ 
//_/_/ Autonomous Systems Laboratory
//_/_/   Technical University of Madrid, Spain
//_/_/   http://www.aslab.org/
//_/_/
//_/_/ Communicative Machines
//_/_/   Edinburgh, United Kingdom
//_/_/   http://www.cmlabs.com/
//_/_/
//_/_/ Istituto Dalle Molle di Studi sull'Intelligenza Artificiale
//_/_/   University of Lugano and SUPSI, Switzerland
//_/_/   http://www.idsia.ch/
//_/_/
//_/_/ Institute of Cognitive Sciences and Technologies
//_/_/   Consiglio Nazionale delle Ricerche, Italy
//_/_/   http://www.istc.cnr.it/
//_/_/
//_/_/ Dipartimento di Ingegneria Informatica
//_/_/   University of Palermo, Italy
//_/_/   http://roboticslab.dinfo.unipa.it/index.php/Main/HomePage
//_/_/
//_/_/
//_/_/ --- HUMANOBS Open-Source BSD License, with CADIA Clause v 1.0 ---
//_/_/
//_/_/ Redistribution and use in source and binary forms, with or without 
//_/_/ modification, is permitted provided that the following conditions 
//_/_/ are met:
//_/_/
//_/_/ - Redistributions of source code must retain the above copyright 
//_/_/ and collaboration notice, this list of conditions and the 
//_/_/ following disclaimer.
//_/_/
//_/_/ - Redistributions in binary form must reproduce the above copyright 
//_/_/ notice, this list of conditions and the following
//_/_/ disclaimer in the documentation and/or other materials provided 
//_/_/ with the distribution.
//_/_/
//_/_/ - Neither the name of its copyright holders nor the names of its 
//_/_/ contributors may be used to endorse or promote products 
//_/_/ derived from this software without specific prior written permission.
//_/_/
//_/_/ - CADIA Clause: The license granted in and to the software under this 
//_/_/ agreement is a limited-use license. The software may not be used in 
//_/_/ furtherance of: 
//_/_/ (i) intentionally causing bodily injury or severe emotional distress 
//_/_/ to any person; 
//_/_/ (ii) invading the personal privacy or violating the human rights of 
//_/_/ any person; or 
//_/_/ (iii) committing or preparing for any act of war.
//_/_/
//_/_/ THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
//_/_/ "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
//_/_/ LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR 
//_/_/ A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
//_/_/ OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
//_/_/ SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
//_/_/ LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, 
//_/_/ DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY 
//_/_/ THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
//_/_/ (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
//_/_/ OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//_/_/
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/  

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
