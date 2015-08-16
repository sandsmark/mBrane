/*
* HUMANOBS - mBrane
*
* Eric Nivel
* Center for Analysis and Design of Intelligent Agents
*   Reykjavik University, Menntavegur 1, 101 Reykjavik, Iceland
*   http://cadia.ru.is
* Copyright(c)2012
*
* This software was developed by the above copyright holder as part of
* the HUMANOBS EU research project, in collaboration with the
* following parties:
*
* Autonomous Systems Laboratory
*   Technical University of Madrid, Spain
*   http://www.aslab.org/
*
* Communicative Machines
*   Edinburgh, United Kingdom
*   http://www.cmlabs.com/
*
* Istituto Dalle Molle di Studi sull'Intelligenza Artificiale
*   University of Lugano and SUPSI, Switzerland
*   http://www.idsia.ch/
*
* Institute of Cognitive Sciences and Technologies
*   Consiglio Nazionale delle Ricerche, Italy
*   http://www.istc.cnr.it/
*
* Dipartimento di Ingegneria Informatica
*   University of Palermo, Italy
*   http://roboticslab.dinfo.unipa.it/index.php/Main/HomePage
*
*
* --- HUMANOBS Open-Source BSD License, with CADIA Clause v 1.0 ---
*
* Redistribution and use in source and binary forms, with or without
* modification, is permitted provided that the following conditions
* are met:
*
* - Redistributions of source code must retain the above copyright
* and collaboration notice, this list of conditions and the
* following disclaimer.
*
* - Redistributions in binary form must reproduce the above copyright
* notice, this list of conditions and the following
* disclaimer in the documentation and/or other materials provided
* with the distribution.
*
* - Neither the name of its copyright holders nor the names of its
* contributors may be used to endorse or promote products
* derived from this software without specific prior written permission.
*
* - CADIA Clause: The license granted in and to the software under this
* agreement is a limited-use license. The software may not be used in
* furtherance of:
* (i) intentionally causing bodily injury or severe emotional distress
* to any person;
* (ii) invading the personal privacy or violating the human rights of
* any person; or
* (iii) committing or preparing for any act of war.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
* "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
* LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
* A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
* OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
* SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
* LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
* DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
* THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
* OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef mBrane_sdk_payload_h
#define mBrane_sdk_payload_h

#include	"object.h"
#include "config.h"


namespace	mBrane
{
namespace	sdk
{

namespace	payloads
{
class	_Message;
class	_StreamData;
}

//	Base interface for payloads
class	mBrane_dll	__Payload:
    public	_Object
{
protected:
    __Payload();
public:
    virtual	~__Payload();
    virtual	uint16_t		cid()	const = 0;
    virtual	void		init();								//	called upon reception
    virtual	size_t		size()	const = 0;					//	returns the size of the whole instance; called upon sending
    virtual	uint16_t		ptrCount()	const;					//	number of pointers to payloads
    virtual	__Payload	*getPtr(uint16_t	i)	const;			//	iterates the pointers to payloads
    virtual	void		setPtr(uint16_t	i, __Payload	*p);	//	iterates the pointers to payloads

    virtual	bool	isShared()		const;	//	called upon reception and transmission.
    virtual	bool	isConstant()	const;	//	called upon reception and transmission.
};

//	Convenience for writing getPtr and setPtr
#define	PTR(Class,Member)	(__Payload	*)(((uint8_t	*)this)+offsetof(Class,Member));

namespace payloads
{
class	_StreamData;
class	_Message;
}

class	mBrane_dll	_Payload:
    public	__Payload
{
public:
    typedef	enum {
        CONTROL = 0,
        DATA = 1,
        STREAM = 2
    } Category;
private:
    static	uint32_t	LastConstantOID;
    static	uint32_t	LastSharedOID;
protected:
    uint64_t	_node_recv_ts;	//	not transmitted
    uint64_t	_recv_ts;		//	not transmitted
    uint64_t	_metaData;		//	[oid(32)|cid(16)|reserved(14)|category(2)]; oid==0x00FFFFFF means non shared object, or shared object not sent yet.
    uint64_t	_node_send_ts;
    uint64_t	_send_ts;
    _Payload();
public:
    virtual	~_Payload();
    Category	category()	const;
    uint64_t		&node_send_ts();	//	send timestamp: time of emission from a node
    uint64_t		&node_recv_ts();	//	recv timestamp: time of reception by a node
    uint64_t		&send_ts();			//	send timestamp: time of emission from a module (< than node_send_ts)
    uint64_t		&recv_ts();			//	recv timestamp: time of reception by a module (> than node_recv_ts)
    //	down_casting; return NULL by default
    virtual	payloads::_Message		*as_Message();
    virtual	payloads::_StreamData	*as_StreamData();
    //	caching.
    void	setOID(uint8_t	NID);	//	for shared objects.
    void	setOID();				//	for constant objects.
    uint32_t	getOID()	const;		//	full OID: 32 bits.
    uint32_t	getID()		const;		//	object ID: 24 bits.
    uint8_t	getNID()	const;		//	0x80 for constant objects.
};

class	mBrane_dll	_RPayload:
    public	__Payload
{
protected:
    uint64_t	_metaData;	//	[reserved(32)|cid(16)|reserved(16)]
    _RPayload();
public:
    virtual	~_RPayload();
};

//	Base class for all payloads.
template<class	P, class	U, class	M>	class	___Payload:	//	P: payload class, U: final class, M: memory
    public	Object<M, P, U>
{
protected:
    //	convenience for accessing the Memory from subclasses unaware of M
    static	void	*Alloc(uint32_t	requested_size, uint32_t	&normalized_size);
    static	void	Dealloc(uint32_t	requested_size, void	*o);
    ___Payload();
public:
    static	uint64_t		_MetaData;
    static	void		*New(uint32_t	size);	//	to initialize the _vftable on recv(); size used for non-standard cases (like Storage<T>), i.e. when the actual size is not sizeof the class
    static	uint16_t		CID();
    static	size_t		Offset();	//	to metadata from this
    void	*operator	new(size_t	s);
    void	operator	delete(void	*o);
    virtual	~___Payload();
    uint16_t				cid()	const;
    virtual	size_t		size()	const;	//	default; returns sizeof(U)
};

template<class	U, class	M>	class	Payload:
    public	___Payload<_Payload, U, M>
{
protected:
    Payload();
public:
    virtual	~Payload();
};

//	Template variant of the well-known DP. Adapts C to Payload<U,M>.
//	Usage:	class	Some3rdPartyClass{ ... };
//			class Some3rdPartyClassAdapted:public PayloadAdapter<Some3rdPartyClass,Some3rdPartyClassAdapted>{ ... };
template<class	C, class	U, class	M>	class	PayloadAdapter:
    public	C,
    public	Payload<U, M>
{
protected:
    PayloadAdapter();
public:
    virtual	~PayloadAdapter();
};

//	Standard raw payload (no transmission information) to embed in any payload.
template<class	U, class	M>	class	RPayload:
    public	___Payload<_RPayload, U, M>
{
protected:
    RPayload();
public:
    virtual	~RPayload();
};

//	Template variant of the well-known DP. Adapts C to RPayload<U,M>.
template<class	C, class	U, class	M>	class	RPayloadAdapter:
    public	C,
    public	RPayload<U, M>
{
protected:
    RPayloadAdapter();
public:
    virtual	~RPayloadAdapter();
};
}
}


#include	"payload.tpl.cpp"


#endif
