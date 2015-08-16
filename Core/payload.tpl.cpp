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

#include "class_register.h"


namespace mBrane
{
namespace sdk
{

template<class P, class U, class M> uint64_t ___Payload<P, U, M>::_MetaData = ClassRegister::Load<U>();

template<class P, class U, class M> inline void *___Payload<P, U, M>::New(uint32_t size)
{
    return new U();
}

template<class P, class U, class M> inline uint16_t ___Payload<P, U, M>::CID()
{
    return (uint16_t)(_MetaData >> 16);
}

template<class P, class U, class M> void *___Payload<P, U, M>::Alloc(uint32_t requested_size, uint32_t &normalized_size)
{
    return M::GetDynamic(requested_size)->alloc(normalized_size);
}

template<class P, class U, class M> void ___Payload<P, U, M>::Dealloc(uint32_t requested_size, void *o)
{
    M::GetDynamic(requested_size)->dealloc(o);
}

template<class P, class U, class M> inline void *___Payload<P, U, M>::operator new(size_t s)
{
    U *p = (U *)Object<M, P, U>::operator new(s);
    p->_metaData = _MetaData;
    return p;
}

template<class P, class U, class M> inline void ___Payload<P, U, M>::operator delete(void *o)
{
    Object<M, P, U>::operator delete(o);
}

template<class P, class U, class M> inline size_t ___Payload<P, U, M>::Offset()
{
    return offsetof(U, _metaData);
}

template<class P, class U, class M> inline ___Payload<P, U, M>::___Payload(): Object<M, P, U>()
{
    this->_metaData = ___Payload<P, U, M>::_MetaData;
}

template<class P, class U, class M> inline ___Payload<P, U, M>::~___Payload()
{
}

template<class P, class U, class M> uint16_t ___Payload<P, U, M>::cid() const
{
    return (uint16_t)(this->_metaData >> 16);
}

template<class P, class U, class M> size_t ___Payload<P, U, M>::size() const
{
    return sizeof(U);
}

////////////////////////////////////////////////////////////////////////////////////

template<class U, class M> inline Payload<U, M>::Payload(): ___Payload<_Payload, U, M>()
{
}

template<class U, class M> inline Payload<U, M>::~Payload()
{
}

////////////////////////////////////////////////////////////////////////////////////

template<class U, class M> inline RPayload<U, M>::RPayload(): ___Payload<_RPayload, U, M>()
{
}

template<class U, class M> inline RPayload<U, M>::~RPayload()
{
}

////////////////////////////////////////////////////////////////////////////////////

template<class C, class U, class M> inline PayloadAdapter<C, U, M>::PayloadAdapter(): Payload<U, M>(), C()
{
}

template<class C, class U, class M> inline PayloadAdapter<C, U, M>::~PayloadAdapter()
{
}

////////////////////////////////////////////////////////////////////////////////////

template<class C, class U, class M> inline RPayloadAdapter<C, U, M>::RPayloadAdapter(): RPayload<U, M>(), C()
{
}

template<class C, class U, class M> inline RPayloadAdapter<C, U, M>::~RPayloadAdapter()
{
}
}
}
