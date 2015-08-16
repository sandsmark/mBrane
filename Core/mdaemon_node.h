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

#ifndef mBrane_sdk_mdaemon_node_h
#define mBrane_sdk_mdaemon_node_h

#include "module_node.h"
#include <CoreLibrary/xml_parser.h>
#include "dynamic_class_loader.h"

#include <thread>

#pragma warning( disable : 4251 )

namespace mBrane
{
class RecvThread;
class PushThread;
class XThread;
class Executing;
namespace sdk
{
namespace mdaemon
{

class Daemon;
// Node API, as seen from the daemons
class mBrane_dll Node:
    public module::Node
{
    friend class RecvThread;
    friend class PushThread;
    friend class XThread;
    friend class Executing;
protected:
    bool volatile _shutdown;
    StaticArray<DynamicClassLoader<Daemon> *> daemonLoaders;
    StaticArray<Daemon *> daemons;
    StaticArray<std::thread> daemonThreads;
    Node(uint8_t ID = NoID);
    bool loadConfig(XMLNode &n);
    void start();
    virtual void shutdown();
    ~Node();
public:
    bool isRunning();
    virtual void dump(const char *fileName) = 0; // dumps the current system state; module dump fileNames: module_class_ID.bin: ex: CR1_123.bin
    virtual void load(const char *fileName) = 0; // initializes itself from a previously saved system state
    virtual void migrate(uint16_t CID, uint16_t ID, uint8_t NID) = 0; // identifies the module and the target node
    //virtual Array<uint8_t,65535> &sharedMemorySegment(uint8_t segment)=0; // FUTURE DEVELOPMENT: pinned down RAM for RDMA, 16KB-1
    virtual module::_Module *getModule(uint8_t hostID, uint16_t CID, uint16_t ID) = 0; // so that the daemons can write module internals and use modules as interfaces to the pub-sub network
    // TODO: define API as pure virtual functions
    // -> node map (an array of mBrane::Networking::NetworkID)
    // -> profiling data
    // - msg throughput
    // - routing latencies (send_ts/recv_ts, node_send_ts/node_recv_ts)
    // - runtime per module, and its ratio wrt total node runtime, routing time consumption/module runtime
    // - logical topology, i.e. affinities between modules; traffic between nodes in light of traffic between modules
};

class mBrane_dll Daemon
{
protected:
    Node *const node;
    Daemon(Node *node);
public:
    typedef Daemon *(*Load)(XMLNode &, Node *); // function exported by the shared library
    static void Run(Daemon *daemon); // args=this daemon
    virtual ~Daemon();
    virtual void init() = 0; // called once, before looping
    virtual uint32_t run() = 0; // called in a loop: while(!node->_shutdown); returns error code (or 0 if none)
    virtual void shutdown() = 0; // called when run returns an error, and when the node shutsdown
};
}
}
}


#endif
