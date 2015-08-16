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

#include "network_interface.h"
#include "message.h"
#include "module_node.h"

#include <iostream>


using namespace mBrane::sdk::module;

namespace mBrane
{
namespace sdk
{

NetworkInterface::NetworkInterface(Protocol _protocol): _protocol(_protocol)
{
}

NetworkInterface::~NetworkInterface()
{
}

NetworkInterface::Protocol NetworkInterface::protocol() const
{
    return _protocol;
}

////////////////////////////////////////////////////////////////////////////////////////////////

CommChannel::CommChannel()
{
    sendBufferPos = 0;
    sendBufferLen = 4096;
    sendBuffer = (uint8_t *) malloc(sendBufferLen);
}

CommChannel::~CommChannel()
{
    free(sendBuffer);
    sendBuffer = NULL;
    sendBufferPos = 0;
    sendBufferLen = 0;
}

inline int16_t CommChannel::_send(__Payload *c, uint8_t destinationNID)
{
    // uint64_t t1 = Time::Get();
    ClassRegister *CR = ClassRegister::Get(c->cid());
    int16_t r;
    uint32_t size = (uint32_t)c->size();
    //std::cout<<"Info: Sending payload type '"<<CR->class_name<<"' ["<<c->cid()<<"] size '"<<size<<"'..."<<std::endl;
    commSendCS.enter();

    if (r = bufferedSend((uint8_t *)&size, sizeof(uint32_t))) { // send the total size first (includes the size of the non transmitted data): will be used to alloc on the recv side
        commSendCS.leave();
        return r;
    }

    if (destinationNID != 0xFF && (c->isConstant() || c->isShared())) {
        if (((_Payload *)c)->getOID() == 0x00FFFFFF) { // object is shared and has never been sent and is not in the cache yet.
            Node::Get()->addSharedObject((_Payload *)c);
        }

        if (r = bufferedSend(((uint8_t *)c) + CR->offset(), sizeof(uint64_t))) { // send the metadata.
            commSendCS.leave();
            return r;
        }

        if (c->isConstant()) {
            if (r = bufferedSend(NULL, 0, true)) {
                commSendCS.leave();
                return r;
            }

            commSendCS.leave();
            return 0;
        }

        if (!Node::Get()->hasLookup(destinationNID, ((_Payload *)c)->getOID())) { // the destination node does not have the object already.
            if (r = bufferedSend(((uint8_t *)c) + CR->offset(), size - CR->offset() - sizeof(uint64_t))) { // send the rest of the object.
                commSendCS.leave();
                return r;
            }

            Node::Get()->addLookup(destinationNID, ((_Payload *)c)->getOID()); // we now know that the receiver has the object.
            // the receiver also knows now that we have it.
        }
    } else if (r = bufferedSend(((uint8_t *)c) + CR->offset(), size - CR->offset())) { // send in full.
        commSendCS.leave();
        return r;
    }

    uint8_t ptrCount = (uint8_t)c->ptrCount();
    __Payload *p;

    for (uint8_t i = 0; i < ptrCount; i++) {
        p = c->getPtr(i);

        if (!p) {
            continue;
        }

        if (r = _send(p, destinationNID)) {
            commSendCS.leave();
            return r;
        }
    }

    // printf("CommChannel Send time:    %u\n", (uint32_t) (Time::Get() - t1));
    if (r = bufferedSend(NULL, 0, true)) {
        commSendCS.leave();
        return r;
    }

    commSendCS.leave();
    return 0;
}

int16_t CommChannel::bufferedSend(uint8_t *b, size_t s, bool sendNow)
{
    if (b && s) {
        while (sendBufferLen - sendBufferPos < s) {
            sendBufferLen += (uint32_t)(s < 4096 ? 4096 : s);
            sendBuffer = (uint8_t *)realloc(sendBuffer, sendBufferLen);
        }

        memcpy(sendBuffer + sendBufferPos, b, s);
        sendBufferPos += (uint32_t)s;
    }

    if (!sendNow) {
        return 0;
    }

    // uint64_t t = Time::Get();
    int16_t res = send(sendBuffer, sendBufferPos);
    // uint64_t d = Time::Get() - t;
    // if (d > 2000) {
    // printf("********* Network send %u bytes took %u us**********\n\n",
    // sendBufferPos, (uint32_t)d);
    // }
    sendBufferPos = 0;
    return res;
}

inline int16_t CommChannel::_recv(__Payload **c, uint8_t sourceNID)
{
    uint64_t metaData;
    int16_t r;
    uint32_t size;
    commRecvCS.enter();

    if (r = recv((uint8_t *)&size, sizeof(uint32_t))) { // receive the total size (includes the size of the non transmitted data)
        commRecvCS.leave();
        return r;
    }

    //Error::PrintBinary((char*)&size, sizeof(uint32_t), true, "Received Size");
    if (r = recv((uint8_t *)&metaData, sizeof(uint64_t), true)) { // receive __Payload::_metaData
        commRecvCS.leave();
        return r;
    }

    //Error::PrintBinary((char*)&metaData,sizeof(uint64_t), true, "Received metaData");
    // allocate and initialize the payload (default ctor is called)
    ClassRegister *CR = ClassRegister::Get((uint16_t)(metaData >> 16));

    if (CR == NULL) {
        commRecvCS.leave();
        return -1;
    }

    //printf("Received Class: '%s' [%u] size '%u'...\n", CR->class_name, (uint16_t)(metaData >> 16), size);

    if (sourceNID != 0xFF) {
        uint32_t OID = metaData >> 32;

        if (OID & 0x80000000) { // constant object.
            *c = Node::Get()->getConstantObject(OID);
            commRecvCS.leave();
            return 0;
        }

        if (OID != 0x00FFFFFF) { // shared object.
            if (Node::Get()->hasLookup(sourceNID, OID)) { // object is already there and the sender knows: we know that because we have sent it to the sender previously.
                // no need to recv anything.
                *c = Node::Get()->getSharedObject(OID);
                Node::Get()->consolidate((_Payload *)*c); // handles the case where c has been doomed after being sent by the source node: ressuscitate it if no advertisement has been made yet.
                commRecvCS.leave();
                return 0;
            }

            Node::Get()->addLookup(sourceNID, OID); // the sender obviously has the object.
            // the sender now knows we have it.
        }
    }

    *c = (__Payload *)(*CR->allocator())(size); // calls UserDefinedClass::New(size)

    if (r = recv(((uint8_t *)*c) + CR->offset(), size - CR->offset())) { // metadata only peeked: read from the offset, and not from offset+sizeof(_metaData)
        commRecvCS.leave();
        return r;
    }

    if (sourceNID != 0xFF) {
        uint32_t OID = (metaData >> 32) & 0x00FFFFFF;

        if (OID != 0x00FFFFFF) { // shared object..
            _Payload *s = Node::Get()->getSharedObject(OID);

            if (s) { // object was already there but the sender didn't know: discard what has been received and use the known object instead.
                delete c;
                *c = s;
                commRecvCS.leave();
                return 0;
            }

            Node::Get()->addSharedObject((_Payload *)*c);
        }
    }

    uint8_t ptrCount = (uint8_t)(*c)->ptrCount();
    __Payload *p;

    for (uint8_t i = 0; i < ptrCount; i++) {
        if (r = _recv(&p, sourceNID)) {
            delete *c;
            commRecvCS.leave();
            return r;
        }

        (*c)->setPtr(i, p);
    }

    (*c)->init();
    commRecvCS.leave();
    return 0;
}

int16_t CommChannel::send(_Payload *p, uint8_t destinationNID)
{
    // p->node_send_ts()=Time::Get();
    return _send(p, destinationNID);
}

int16_t CommChannel::recv(_Payload **p, uint8_t sourceNID)
{
    int16_t r;

    if (r = _recv((__Payload **)p, sourceNID)) {
        return r;
    }

    // (*p)->node_recv_ts()=Time::Get();
    return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////

ConnectedCommChannel::ConnectedCommChannel(): CommChannel()
{
}

ConnectedCommChannel::~ConnectedCommChannel()
{
}

////////////////////////////////////////////////////////////////////////////////////////////////

BroadcastCommChannel::BroadcastCommChannel(): CommChannel()
{
}

BroadcastCommChannel::~BroadcastCommChannel()
{
}
}
}
