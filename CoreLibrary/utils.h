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

#ifndef core_utils_h
#define core_utils_h
#define LINUX
#include "types.h"
#include <stdio.h>
#include <semaphore.h>
#include <iostream>
#include <string>
#include <functional>
#if defined WINDOWS
#include <sys/timeb.h>
#include <time.h>
#elif defined LINUX
#include <dlfcn.h>
#include <errno.h>
#include <sys/utsname.h>
#include <sys/time.h>
#include <cstring>
#include <cstdlib>
#include <pthread.h>
#include <signal.h>
#include <sys/time.h>
#include <unistd.h>
#include <time.h>
// #undef HANDLE
// #define HANDLE pthread_cond_t*
#endif

#include <chrono>
#include <atomic>
#include <mutex>
#include <condition_variable>

#ifdef WINDOWS
#define SOCKETWOULDBLOCK WSAEWOULDBLOCK
#else
#define SOCKETWOULDBLOCK EWOULDBLOCK
#endif

#ifndef SD_BOTH
#define SD_BOTH 2
#endif

#define R250_LEN 250
#define R521_LEN 521

// Wrapping of OS-dependent functions
namespace core
{

bool core_dll WaitForSocketReadability(socket s, int32_t timeout);
bool core_dll WaitForSocketWriteability(socket s, int32_t timeout);

class core_dll Error
{
public:
    static int32_t GetLastOSErrorNumber();
    static bool GetOSErrorMessage(char *buffer, uint32_t buflen, int32_t err = -1);
    static bool PrintLastOSErrorMessage(const char *title);
    static void PrintBinary(void *p, uint32_t size, bool asInt, const char *title = NULL);
};

class core_dll SharedLibrary
{
private:
    shared_object library;
public:
    static SharedLibrary *New(const char *fileName);
    SharedLibrary();
    ~SharedLibrary();
    SharedLibrary *load(const char *fileName);
    template<typename T> T getFunction(const char *functionName);
};

class core_dll Time  // TODO: make sure time stamps are consistent when computed by different cores
{
public:
    /// in us since 01/01/1970
    static uint64_t Get() {
        using namespace std::chrono;
        return duration_cast<microseconds>(high_resolution_clock::now().time_since_epoch()).count();
    }
};

class core_dll Host
{
public:
    typedef char host_name[255];
    static uint8_t Name(char *name); // name size=255; return the actual size
};

class core_dll Semaphore
{
private:
    int count;
    std::mutex mutex;
    std::condition_variable_any condition;

public:
    Semaphore(uint32_t initialCount = 0) : count(initialCount) {}
    void acquire(){
        std::lock_guard<std::mutex> guard(mutex);

        while (count == 0) {
            condition.wait(mutex);
        }
        count--;
    }

    void release(){
        std::lock_guard<std::mutex> guard(mutex);
        count++;
        condition.notify_one();
    }

    void reset() {
        std::lock_guard<std::mutex> guard(mutex);
        if (count > 0) {
            count = 0;
        }
    }
};

class core_dll CriticalSection
{
private:
    pthread_mutex_t cs;
public:
    CriticalSection();
    ~CriticalSection();
    void enter();
    void leave();
};

class core_dll SignalHandler
{
public:
    static void Add(sighandler_t h);
    static void Remove(sighandler_t h);
};

uint8_t core_dll BSR(uint32_t data); // BitScanReverse

}

#include "utils.tpl.cpp"


#endif
