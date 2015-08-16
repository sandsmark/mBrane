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

#ifndef mBrane_sdk_message_h
#define mBrane_sdk_message_h

#include "payload.h"
#include "memory.h"
#include "module_node.h"


// Root classes for defining applicative message classes
namespace mBrane
{
namespace sdk
{

class CommChannel;

namespace payloads
{

template<class U> class ControlMessage:
    public Payload<U, Memory>
{
protected:
    ControlMessage();
public:
    virtual ~ControlMessage();
};

////////////////////////////////////////////////////////////////////////////////////////////////

class mBrane_dll _StreamData
{
protected:
    uint16_t _sid; // stream identifer
    _StreamData(uint16_t sid);
public:
    virtual ~_StreamData();
    uint16_t &sid();
};

////////////////////////////////////////////////////////////////////////////////////////////////

template<class U, class M> class StreamData:
    public Payload<U, M>,
    public _StreamData
{
protected:
    StreamData(uint16_t sid = 0);
public:
    virtual ~StreamData();
    _StreamData *as_StreamData();
};

////////////////////////////////////////////////////////////////////////////////////////////////

class mBrane_dll _Message
{
protected:
    uint16_t _senderModuleCID;
    uint16_t _senderModuleID;
    uint16_t _senderNodeID;
    _Message();
public:
    virtual ~_Message();
    uint16_t &senderModule_cid();
    uint16_t &senderModule_id();
    uint16_t &senderNodeID();
};

////////////////////////////////////////////////////////////////////////////////////////////////

template<class U, class M> class Message:
    public Payload<U, M>,
    public _Message
{
protected:
    Message();
public:
    virtual ~Message();
    _Message *as_Message();
};

////////////////////////////////////////////////////////////////////////////////////////////////

// Shared objects are cached. The first 32 bits of their metadata is NID(7 bits)|ID(24 bits). The most significant bit of the OID is 0 (1 for constants).
// Shared objects are supposed not to change once created.
// Constant objects are duplicated on each node.
template<class U, class M> class SharedObject:
    public Message<U, M>
{
protected:
    void decRef(); // notifies the node when the ref count drops to 1: doomed for deletion.
    SharedObject();
public:
    virtual ~SharedObject();
    bool isShared();
    bool isConstant();
};

////////////////////////////////////////////////////////////////////////////////////////////////

// CStorage stands for contiguous storage.
// S. superclass, T: type of the data to be stored contiguously.
// Typical use: data compacted dynamically, i.e. whose size is not an integral constant (e.g. that could not parameterize a template), e.g. archives, compressed images, etc.
// Usage sample:
// template<class U> class CoreData:public Message<U,Memory>{...};
// class ACStorage:public CStorage<CoreData<ACStorage,int32_t> >{...};
// ACStorage *acs=new(32) ACStorage(); // acs contains an array of 32 int32.
// Do not declare any data in subclasses of CStorage: such subclasses shall only contain logic, e.g. functions to exploit CoreData (if any) and the array of Ts.
template<class S, typename T> class CStorage:
    public S
{
protected:
    uint32_t _size; // of the whole instance (not normalized)
    uint32_t _capacity; // max number of elements in the array
    T *_data; // points to ((T *)(((uint8_t *)this)+offsetof(CStorage<S,T>,_data)+sizeof(T *)));
    CStorage();
public:
    static void *New(uint32_t size); // total size of the instance; called upon sending
    void *operator new(size_t s); // unused; need for compilation of ___Payload<U,P,M>::New
    void *operator new(size_t s, uint32_t capacity); // overrides Object<U,M>::new
    void operator delete(void *o); // overrides Object<U,M>::delete
    virtual ~CStorage();
    size_t size() const;
    uint32_t getCapacity() const;
    T &data(uint32_t i);
    T &data(uint32_t i) const;
    T *data();
};
}
}
}


#include "message.tpl.cpp"


#endif
