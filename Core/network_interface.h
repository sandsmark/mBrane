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

#ifndef mBrane_sdk_network_interface_h
#define mBrane_sdk_network_interface_h

#include "utils.h"
#include "xml_parser.h"
#include "mdaemon_node.h"


namespace mBrane
{
namespace sdk
{

class CommChannel;
class ConnectedCommChannel;
class mBrane_dll NetworkInterface
{
public:
    typedef enum {
        UDP = 0,
        TCP = 1,
        RM = 2,
        IB = 3 // MPI2
    } Protocol;
protected:
    Protocol _protocol;
    NetworkInterface(Protocol _protocol);
public:
    typedef NetworkInterface *(*Load)(XMLNode &, mdaemon::Node *n); // function exported by the shared library
    virtual ~NetworkInterface();
    Protocol protocol() const;
    virtual bool operator ==(NetworkInterface &i) = 0;
    virtual bool operator !=(NetworkInterface &i) = 0;
    virtual bool canBroadcast() = 0; // as opposed to connected mode
    virtual uint16_t start() = 0; // initialize the network interface; returns 0 if successful
    virtual uint16_t stop() = 0; // the network interface; returns 0 if successful
    virtual uint16_t getIDSize() = 0; // node net ID to be broadcast
    virtual void fillID(uint8_t *ID) = 0; // with relevant parameters (different from Node::_ID; ex: IP addr and port)
    virtual uint16_t newChannel(uint8_t *ID, CommChannel **channel) = 0; // create a new channel (bcast ID=local node net ID, or connected ID=remote node net ID); returns 0 if successful
    virtual uint16_t acceptConnection(ConnectedCommChannel **channel, int32_t timeout, bool &timedout) = 0; // listen to connect attempts and creates a new channel accordingly; returns 0 if successful
};

class _Payload;
class mBrane_dll CommChannel
{
protected:
    CommChannel(); // initialization to be performed in subclasses' constructors
    int16_t _send(__Payload *c, uint8_t destinationNID);
    int16_t _recv(__Payload **c, uint8_t sourceNID);
    std::mutex commSendMutex;
    std::mutex commRecvMutex;
    uint8_t *sendBuffer;
    uint32_t sendBufferLen;
    uint32_t sendBufferPos;
public:
    virtual ~CommChannel(); // shutdown to be performed in subclasses' destructors
    virtual int16_t send(uint8_t *b, size_t s) = 0; // return 0 if successfull, error code (>0) otherwise
    virtual int16_t recv(uint8_t *b, size_t s, bool peek = false) = 0;
    virtual bool isConnected() = 0;
    virtual bool disconnect() = 0;
    int16_t bufferedSend(uint8_t *b, size_t s, bool sendNow = false);
    int16_t send(_Payload *p, uint8_t destinationNID); // return 0 if successfull, error code (>0) otherwise. destinationNID used only for connected comm channels.
    int16_t recv(_Payload **p, uint8_t sourceNID);
};

class mBrane_dll ConnectedCommChannel:
    public CommChannel
{
protected:
    ConnectedCommChannel();
public:
    virtual ~ConnectedCommChannel();
};

class mBrane_dll BroadcastCommChannel:
    public CommChannel
{
protected:
    BroadcastCommChannel();
public:
    virtual ~BroadcastCommChannel();
};
}
}


#endif
