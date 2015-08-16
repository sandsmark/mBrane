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

namespace mBrane
{
namespace sdk
{
namespace payloads
{

template<class U> ControlMessage<U>::ControlMessage()/*:Payload<U,Memory>()*/
{
    this->_metaData = ControlMessage<U>::_MetaData | ControlMessage<U>::CONTROL;
}

template<class U> ControlMessage<U>::~ControlMessage()
{
}

////////////////////////////////////////////////////////////////////////////////////////////////

template<class U, class M> inline StreamData<U, M>::StreamData(uint16_t sid): Payload<U, M>(), _StreamData(sid)
{
    this->_metaData = StreamData<U, M>::_MetaData | StreamData<U, M>::STREAM;
}

template<class U, class M> inline StreamData<U, M>::~StreamData()
{
}

template<class U, class M> _StreamData *StreamData<U, M>::as_StreamData()
{
    return (_StreamData *)(((uint8_t *)this) + sizeof(Payload<U, M>));
}

////////////////////////////////////////////////////////////////////////////////////////////////

template<class U, class M> Message<U, M>::Message(): Payload<U, M>(), _Message()
{
    this->_metaData = Message<U, M>::_MetaData | Message<U, M>::DATA;
}

template<class U, class M> Message<U, M>::~Message()
{
}

template<class U, class M> _Message *Message<U, M>::as_Message()
{
    return (_Message *)(((uint8_t *)this) + sizeof(Payload<U, M>));
}

////////////////////////////////////////////////////////////////////////////////////////////////

template<class U, class M> SharedObject<U, M>::SharedObject(): Message<U, M>()
{
    module::Node::Get()->addSharedObject(this);
}

template<class U, class M> SharedObject<U, M>::~SharedObject()
{
}

template<class U, class M> void SharedObject<U, M>::decRef()
{
    if (_Payload::getOID() == 0x00FFFFFF || isConstant()) { // object has not been sent yet: it has not been smart pointed by the cache: treat as a normal object.
        _Object::decRef();
    } else {
        int32_t ref_count = Atomic::Decrement32(&this->refCount);

        switch (ref_count) {
        case 1:
            module::Node::Get()->markUnused(this);
            break;

        case 0:
            delete this;
            break;
        }
    }
}

template<class U, class M> bool SharedObject<U, M>::isShared()
{
    return true;
}

template<class U, class M> bool SharedObject<U, M>::isConstant()
{
    return this->getNID() == 0x80;
}

////////////////////////////////////////////////////////////////////////////////////////////////

template<class S, typename T> void *CStorage<S, T>::New(uint32_t size)
{
    uint32_t capacity = size - sizeof(CStorage<S, T>); // capacity is now the size of the array
    capacity = capacity / sizeof(T); // now capacity equals the number of elements in the array
    return new(capacity) CStorage<S, T>();
}

template<class S, typename T> void *CStorage<S, T>::operator new(size_t s)
{
    return NULL;
}

template<class S, typename T> void *CStorage<S, T>::operator new(size_t s, uint32_t capacity)
{
    uint32_t normalizedSize;
    uint32_t size = sizeof(CStorage<S, T>) + capacity * sizeof(T);
    CStorage<S, T> *o = (CStorage<S, T> *)S::Alloc(size, normalizedSize);
    o->_size = size;
    o->_capacity = capacity;
    o->_metaData = S::_MetaData;
    return o;
}

template<class S, typename T> void CStorage<S, T>::operator delete(void *o)
{
    S::Dealloc(((CStorage<S, T> *)o)->_size, o);
}

template<class S, typename T> CStorage<S, T>::CStorage(): S()
{
    _data = (T *)(((uint8_t *)this) + sizeof(S) + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(T *));
}

template<class S, typename T> CStorage<S, T>::~CStorage()
{
}

template<class S, typename T> size_t CStorage<S, T>::size() const
{
    return _size;
}

template<class S, typename T> uint32_t CStorage<S, T>::getCapacity() const
{
    return _capacity;
}

template<class S, typename T> T &CStorage<S, T>::data(uint32_t i) const
{
    return _data[i];
}

template<class S, typename T> T &CStorage<S, T>::data(uint32_t i)
{
    return _data[i];
}

template<class S, typename T> T *CStorage<S, T>::data()
{
    return _data;
}
}
}
}
