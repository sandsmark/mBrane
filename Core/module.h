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

#ifndef mBrane_sdk_module_h
#define mBrane_sdk_module_h

#include "../CoreLibrary/utils.h"
#include "message.h"
#include <thread>

using namespace mBrane::sdk::payloads;

namespace mBrane
{
class XThread;
class ModuleDescriptor;
class Node;
namespace sdk
{
namespace module
{

// Root class for all modules.
// The actual base class for user-defined modules is defined in application.h and respectively, in library.h for module library vendors
// Migration sequence: migrateOut->dump->payload->send-/ /-receive->load->migrateIn; modules can then launch their own internal threads if any
class mBrane_dll _Module:
    public _Object
{
    friend class mBrane::Node;
    friend class mBrane::XThread;
    friend class mBrane::ModuleDescriptor;
private:
    XThread *processor;
    FastSemaphore *sync;
    ModuleDescriptor *descriptor;
protected:
    uint16_t _cid;
    uint16_t _id;
    bool _canMigrate;
    uint8_t _priority;
    bool _ready; // set to false after stop and migrateOut, set to true after start and migrateIn
    _Module();
    void sleep(int64_t d);
    void wait(std::thread *threads[], uint32_t threadCount);
    void wait(std::thread *_thread);
public:
    typedef enum {
        DISCARD = 0,
        WAIT = 1,
        PREEMPT = 2
    } Decision;
    virtual ~_Module();
    virtual void loadParameters(const std::vector<int32_t> &numbers, const std::vector<std::string> &strings) {} // parameters is deallocated after the call; called only upon local construction - in other ints, will not be called upon remote construction (using the CreateModule message)
    uint8_t &priority();
    bool canMigrate(); // on another node; dynamic
    bool isReady(); // if not, messages will be lost
    virtual uint32_t dumpSize(); // dynamic
    virtual _Payload *dump(); // dumps the current state; can be called anytime
    virtual void load(_Payload *chunk); // initializes itself from a previously saved state
    virtual void _start() = 0; // called when the module is loaded in a thread for the first time, i.e. at node starting time
    virtual void _stop() = 0; // called just before the module is unloaded from the thread for the last time, i.e. at node shutdown time
    virtual void migrateOut(); // called when the module is unloaded from its current thread for migration
    virtual void migrateIn(); // called when the module is loaded in a new thread after having migrated
    virtual void notify(_Payload *p) = 0; // called when the module receives a message
    virtual void notify(uint16_t sid, _Payload *p) = 0; // called when the module receives data from a stream
    virtual Decision dispatch(_Payload *p) = 0; // called when the module code is already processed by an XThread and a new message comes in
}; //class object
} //namespace module
} //namespace sdk
} //namespace mbrane


#endif
