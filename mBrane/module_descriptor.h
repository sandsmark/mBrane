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

#ifndef mBrane_module_descriptor_h
#define mBrane_module_descriptor_h

#include "../Core/list.h"
#include "../Core/module.h"
#include "xml_parser.h"

#include "projection.h"


using namespace mBrane::sdk;
using namespace mBrane::sdk::module;

namespace mBrane
{

class NodeEntry;
class ModuleEntry:
    public Object<Memory, _Object, ModuleEntry>
{
public:
    NodeEntry *node;
    ModuleDescriptor *descriptor;
    ModuleEntry(NodeEntry *n, ModuleDescriptor *m);
    ~ModuleEntry();
};

class NodeEntry
{
private:
    std::mutex mutex; // guards against Messaging::SendMessages.
    uint32_t activationCount; // TODO: std::atomic
public:
    static Array<Array<NodeEntry, 32>, 128> Main[2]; // 0: Data and Control: message class -> nodes -> modules, 1: Streams: stream id -> nodes -> modules
    NodeEntry();
    ~NodeEntry();
    void incActivation()
    {
        mutex.lock();
        activationCount++;
        mutex.unlock();
    }
    void decActivation()
    {
        mutex.lock();
        activationCount--;
        mutex.unlock();
    }
    void getActivation(uint32_t &a)
    {
        mutex.lock();
        a = activationCount;
        mutex.unlock();
    }
    List<P<ModuleEntry>, 1024> modules;
};

template<> class Projection<ModuleDescriptor>:
    public _Projection<ModuleDescriptor, Projection<ModuleDescriptor>>
{
public:
    Array<List<P<ModuleEntry>, 1024>::Iterator, 128> subscriptions[2]; // 0: indexed by message class ID (MCID), 1: indexed by stream ID (SID)
    uint16_t  subscriptionCount[2]; // idem
    Projection(ModuleDescriptor *projected, Space *space);
    ~Projection();
    void activate();
    void deactivate();
    void setActivationLevel(float a);
    void updateActivationCount(float t);
    void addSubscription(uint8_t payloadType, uint16_t ID, List<P<ModuleEntry>, 1024>::Iterator i);
    void removeSubscription(uint8_t payloadType, uint16_t ID);
};

// Module proxy.
class ModuleDescriptor:
    public Projectable<ModuleDescriptor>
{
private:
    class _Subscription
    {
    public:
        uint16_t MCID;
        uint16_t SID;
    };
    class _Projection
    {
    public:
        uint16_t  spaceID;
        float activationLevel;
        Array<_Subscription, 8> subscriptions;
    };
    Array<_Projection, 32> initialProjections;
    const char *name;
public:
    uint16_t CID;
    static Array<Array<P<ModuleDescriptor>, 128>, 32> Config; // indexed by module descriptor class ID | ID; temporary: used at config time when node IDs are not known; tranfered in Node::start in Main.
    static Array<Array<Array<P<ModuleDescriptor>, 128>, 32>, 8> Main; // indexed by host ID | module descriptor class ID | ID.
    static ModuleDescriptor *New(XMLNode &n);
    static void Init(uint8_t hostID); // resolves host name into ID, copies Config in Main, apply initial projections.
    static uint16_t  GetID(uint8_t hostID, uint16_t CID); // returns the first available slot in Main[hostID][CID].
    static const char *GetName(uint16_t cid, uint16_t id); // returns the name of CID.
    Host::host_name hostName; // resolved in hostID at Node::run() time
    P<_Module> module; // NULL if remote
    //_Module *module;
    ModuleDescriptor(const char *hostName, _Module *m, uint16_t CID, const char *name); // invoked at Node::loadApplication() time.
    ModuleDescriptor(uint8_t hostID, uint16_t CID, uint16_t ID); // invoked dynamically.
    ~ModuleDescriptor();
    void applyInitialProjections(uint8_t hostID);
    const char *getName();
    void _activate();
    void _deactivate();
    void addSubscription_message(uint8_t hostID, uint16_t spaceID, uint16_t MCID);
    void addSubscription_stream(uint8_t hostID, uint16_t spaceID, uint16_t SID);
    void removeSubscription_message(uint8_t hostID, uint16_t spaceID, uint16_t MCID);
    void removeSubscription_stream(uint8_t hostID, uint16_t spaceID, uint16_t SID);
    void removeSubscriptions_message(uint8_t hostID, uint16_t spaceID);
    void removeSubscriptions_stream(uint8_t hostID, uint16_t spaceID);
};
}


#endif
