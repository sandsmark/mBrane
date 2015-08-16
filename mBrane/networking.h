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

#ifndef mBrane_networking_h
#define mBrane_networking_h

#include "../Core/network_interface.h"
#include "network_id.h"

#include "pipe.h"
#include "../Core/list.h"
#include "../Core/control_messages.h"
#include "messaging.h"
#include <thread>

using namespace mBrane::sdk;
using namespace mBrane::sdk::mdaemon;

namespace mBrane
{

#define CONTROL_PRIMARY_INITIALISED 0x0001
#define DATA_PRIMARY_INITIALISED 0x0002
#define STREAM_PRIMARY_INITIALISED 0x0004
#define CONTROL_SECONDARY_INITIALISED 0x0008
#define DATA_SECONDARY_INITIALISED 0x0010
#define STREAM_SECONDARY_INITIALISED 0x0020
#define CONTROL_PRIMARY_CONNECTED 0x0040
#define DATA_PRIMARY_CONNECTED 0x0080
#define STREAM_PRIMARY_CONNECTED 0x0100
#define CONTROL_SECONDARY_CONNECTED 0x0200
#define DATA_SECONDARY_CONNECTED 0x0400
#define STREAM_SECONDARY_CONNECTED 0x0800

class NodeCon;
struct ReceiveThreadInfo {
    NodeCon *con;
    CommChannel *channel;
};

class Networking;
class NodeCon
{
public:
    NodeCon(Networking *node);
    virtual ~NodeCon();

    bool setSourceNID(uint8_t sourceNID);
    bool setName(const char *name);

    bool isInUse();
    uint32_t getConnectionStatus();
    bool isConnected(module::Node::Network network = module::Node::EITHER);
    bool disconnect();

    bool startNetworkChannel(CommChannel *c, uint8_t type, bool isCopy = false);
    CommChannel *getNetworkChannel(uint8_t type);

    Networking *node;
    NetworkID *networkID;
    uint8_t  sourceNID;
    char *name;
    bool joined;
    bool ready;

    Array<CommChannel *, 6> commChannels;
    Array<std::thread, 6> commThreads;
    std::thread pushThread;

    Pipe11<P<_Payload>, MESSAGE_INPUT_BLOCK_SIZE> buffer; // incoming messages from remote nodes
    static void ReceiveMessages(ReceiveThreadInfo *info);
    static void PushJobs(NodeCon *_this);
};

class Messaging;
// Handles network initialization and connection.
// Handles two isolated networks: primary (ex: core computation) and secondary (ex: I/O, signal processing)
// Network IDs carry the primary, secondary or both identifications
// When receiving a bcast id bearing two, connect to the primary only
// When sending to a node, use the primary only if two are available
// Receiving is agnostic
//
// Reference nodes must be on the primary network
//
// Node boot sequence:
//
// 1 boot one single node with a timeout (if it times out, it's the ref node)
// 2 when ready (callback), boot all the other nodes
//
// Algorithm for node connection:
//
// bcast its net ID on discovery channel
// accept connections:
// if timedout this is ref node, scan IDs on discovery channel
// else
// the ref node sends (on data channel if control channel is bcast, on control channel otherwise): its own net ID, an assigned NID and the net map (i.e. the list of ready nodes net ID)
// connect to each node in the list excepted the sender
// if(ref node) send time sync periodically on control channel
// start messages sending and receiving threads
//
// When at least one connection to a remote node dies, the node in question is considred dead and the other connections to it are terminated
// if the ref node dies, the node with the lowest NID is the new ref node
class mbrane_dll Networking:
    public mdaemon::Node, public Messaging
{
    friend class Messaging;
    friend class RecvThread;
// friend class Messaging;
    friend class NodeCon;
protected:
    Host::host_name hostName;
    uint8_t  hostNameSize;

    typedef void (*BootCallback)();
    SharedLibrary *callbackLibrary;
    BootCallback bootCallback;

    DynamicClassLoader<NetworkInterface> *networkInterfaceLoaders[7];
    NetworkInterface *networkInterfaces[7];

    Network network;
    std::mutex acceptConnectionMutex;

    bool startInterfaces();
    void stopInterfaces();

    int32_t bcastTimeout; // in ms

    uint8_t connectedNodeCount;

    NetworkID *networkID;

    //class DataCommChannel{
    //public:
    // DataCommChannel();
    // ~DataCommChannel();
    // typedef struct{
    // CommChannel *data;
    // CommChannel *stream;
    // }CommChannels;
    // CommChannels channels[2]; // 1 for each network
    // NetworkID *networkID;
    //};
    CommChannel *discoveryChannel; // bcast
    CommChannel *broadcastChannel[2]; // bcast
// Array<CommChannel *,32> controlChannels[2]; // for each network: 1 (bcast capable) or many (connected)
// Array<DataCommChannel *,32,ArrayManaged> dataChannels;
    std::mutex channelsMutex; // protects controlChannels and dataChannels
    UNORDERED_MAP<uint8_t, NodeCon *> nodes;

    bool isTimeReference;
    uint8_t referenceNID;
    void setNewReference();

    virtual void startReceivingThreads(uint8_t NID) = 0;
    virtual void notifyNodeJoined(uint8_t NID, NetworkID *networkID) = 0;
    virtual void notifyNodeLeft(uint8_t NID) = 0;
    virtual void shutdown();

    Array<std::thread, 32> commThreads;

    bool checkSyncProbe(uint8_t syncNodeID);
    void systemReady();

    uint8_t nodeCount;
    bool addNodeName(const char *name, bool myself = false);
    uint8_t getNodeID(const char *name);
    bool allNodesJoined();
    bool allNodesReady();

    static void ScanIDs(Networking *node);
    typedef struct {
        Networking *node;
        int32_t  timeout;
        Network network;
        _Payload::Category category;
    } AcceptConnectionArgs;
    static void AcceptConnections(AcceptConnectionArgs *acargs);
    static void Sync(Networking *node);
    int64_t timeDrift; // in ms
    int64_t syncPeriod; // in ms

    uint16_t sendID(CommChannel *c, NetworkID *networkID);
    uint16_t recvID(CommChannel *c, NetworkID *&networkID, bool expectToken = true);
    uint16_t sendMap(CommChannel *c);
    uint16_t recvMap(CommChannel *c, NetworkID *fromNetworkID);
    uint16_t connect(NetworkID *networkID);
    uint16_t connect(Network network, NetworkID *networkID);
    void _broadcastControlMessage(_Payload *p, Network network);
    void broadcastControlMessage(_Payload *p, Network network);
    void _sendControlMessage(_Payload *p, uint8_t destinationNID, Network network);
    void sendControlMessage(_Payload *p, uint8_t destinationNID, Network network);
    void sendData(uint8_t NID, _Payload *p, Network network);
    void sendStreamData(uint8_t NID, _Payload *p, Network network);
    void processError(uint8_t NID); // upon send/recv error. Disconnect the node on both networks
    uint8_t addNodeEntry();

    bool init();
    virtual void start(uint8_t assignedNID, NetworkID *networkNID, bool isTimeReference);
    bool startSync();

    Networking();
    ~Networking();
    bool loadInterface(XMLNode &interfaces, XMLNode &config, const char *name, InterfaceType type);
    bool loadConfig(XMLNode &n);
};
}


#endif
