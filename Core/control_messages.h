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

#ifndef	mBrane_sdk_control_messages_h
#define	mBrane_sdk_control_messages_h

#include	"message.h"


namespace	mBrane
{
namespace	sdk
{
namespace	payloads
{

class	mBrane_dll	SystemReady:	//	sent when all the nodes specified in the node cfg file are up and running
    public	ControlMessage<SystemReady>
{
};

class	mBrane_dll	SyncProbe:
    public	ControlMessage<SyncProbe>
{
public:
    uint8_t	node_id;
};

class	mBrane_dll	SyncEcho:
    public	ControlMessage<SyncEcho>
{
public:
    int64_t	t0;	//	from probe node_send_ts
    int64_t	t1;	//	from probe node_recv_ts
    // t2 is recorded in echo node_send_ts
    // t3 is recorded in echo node_recv_ts
};

class	mBrane_dll	NodeJoined:
    public	ControlMessage<NodeJoined>
{
public:
    uint8_t	node_id;
};

class	mBrane_dll	NodeLeft:
    public	ControlMessage<NodeLeft>
{
public:
    uint8_t	node_id;
};

class	mBrane_dll	SetThreshold:
    public	ControlMessage<SetThreshold>
{
public:
    uint8_t	host_id;
    uint16_t	space_id;
    float	threshold;
};

class	mBrane_dll	ActivateModule:
    public	ControlMessage<ActivateModule>
{
public:
    uint8_t	host_id;
    uint16_t	module_cid;
    uint16_t	module_id;
    uint16_t	space_id;
    float	activationLevel;
};

class	mBrane_dll	ActivateSpace:
    public	ControlMessage<ActivateSpace>
{
public:
    uint8_t	host_id;
    uint16_t	space_id;	//	the space on which target space is projected onto
    uint16_t	target_sid;	//	the space to set the activation for
    float	activationLevel;
};

template<class	U>	class	Subscribe:
    public	ControlMessage<U>
{
public:
    uint8_t	host_id;
    uint16_t	module_cid;
    uint16_t	module_id;
    uint16_t	space_id;
};

class	mBrane_dll	SubscribeMessage:
    public	Subscribe<SubscribeMessage>
{
public:
    uint16_t	message_cid;
};

class	mBrane_dll	UnsubscribeMessage:
    public	Subscribe<UnsubscribeMessage>
{
public:
    uint16_t	message_cid;
};

class	mBrane_dll	SubscribeStream:
    public	Subscribe<SubscribeStream>
{
public:
    uint16_t	stream_id;
};

class	mBrane_dll	UnsubscribeStream:
    public	Subscribe<UnsubscribeStream>
{
public:
    uint16_t	stream_id;
};

class	mBrane_dll	CreateModule:
    public	ControlMessage<CreateModule>
{
public:
    uint16_t	sender_cid;
    uint16_t	sender_id;
    uint8_t	host_id;
    uint16_t	module_cid;
};

class	mBrane_dll	DeleteModule:
    public	ControlMessage<DeleteModule>
{
public:
    uint8_t	host_id;
    uint16_t	module_cid;
    uint16_t	module_id;
};

class	mBrane_dll	CreateSpace:
    public	ControlMessage<CreateSpace>
{
public:
    uint16_t	sender_cid;
    uint16_t	sender_id;
    uint8_t	host_id;
};

class	mBrane_dll	DeleteSpace:
    public	ControlMessage<DeleteSpace>
{
public:
    uint8_t	host_id;
    uint16_t	space_id;
};

class	mBrane_dll	KillModule:	//	internal message issued upon reception of DeleteModule
    public	ControlMessage<KillModule>
{
};

template<class	U>	class	_DeleteSharedObjects:
    public	ControlMessage<U>
{
public:
    uint8_t	node_id;
};

class	mBrane_dll	DeleteSharedObjects:	//	issued by the garbage collector. Triggers ack from remote nodes.
    public	CStorage<_DeleteSharedObjects<DeleteSharedObjects>, uint32_t>
{
};

class	mBrane_dll	AckDeleteSharedObjects:
    public	ControlMessage<AckDeleteSharedObjects>
{
public:
    uint8_t	node_id;
};

#define	MBRANE_MESSAGE_CLASS(C)	static	const	uint16_t	C##_CID=(uint16_t)__COUNTER__;
//#define	MBRANE_MESSAGE_CLASS(C)	static	const	uint16_t	C##_CID=ClassRegister::Load<C>(0);
#include	"mBrane_message_classes.h"

void	mBrane_dll	LoadControlMessageMetaData();	//	overwrites C::_MetaData with actual values (i.e. C_CID dependent) - where C is a Core-defined control message;
//	Core, mBrane and the user lib have different copies of C::_MetaData (initialized to 0x00FFFFFFFFFFFFF) at Core loading time;
//	called by mBrane in main();
//	C::_MetaData is not used by the user lib (no instances of mBrane control messages are created: the user lib only reacts to their occurrences).
}
}
}


#endif
