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

#include "utils.h"
#include <thread>
#include <chrono>

#if defined WINDOWS
#include <intrin.h>
#pragma intrinsic (_InterlockedDecrement)
#pragma intrinsic (_InterlockedIncrement)
#pragma intrinsic (_InterlockedExchange)
#pragma intrinsic (_InterlockedExchange64)
#pragma intrinsic (_InterlockedCompareExchange)
#pragma intrinsic (_InterlockedCompareExchange64)
#elif defined LINUX
#endif

#include <algorithm>
#include <cctype>
#include <ctime>


#define R250_IA (sizeof(uint32_t)*103)
#define R250_IB (sizeof(uint32_t)*R250_LEN-R250_IA)
#define R521_IA (sizeof(uint32_t)*168)
#define R521_IB (sizeof(uint32_t)*R521_LEN-R521_IA)

namespace core
{

#if defined LINUX
bool CalcTimeout(struct timespec &timeout, uint32_t ms)
{
    struct timeval now;

    if (gettimeofday(&now, NULL) != 0) {
        return false;
    }

    timeout.tv_sec = now.tv_sec + ms / 1000;
    long us = now.tv_usec + ms % 1000;

    if (us >= 1000000) {
        timeout.tv_sec++;
        us -= 1000000;
    }

    timeout.tv_nsec = us * 1000; // usec -> nsec
    return true;
}

uint64_t GetTime()
{
    struct timeval tv;

    if (gettimeofday(&tv, NULL)) {
        return 0;
    }

    return (tv.tv_usec + tv.tv_sec * 1000000LL);
}
#endif

void Error::PrintBinary(void *p, uint32_t size, bool asInt, const char *title)
{
    if (title != NULL) {
        printf("--- %s %u ---\n", title, size);
    }

    unsigned char c;

    for (uint32_t n = 0; n < size; n++) {
        c = *(((unsigned char *)p) + n);

        if (asInt) {
            printf("[%u] ", (unsigned int)c);
        } else {
            printf("[%c] ", c);
        }

        if ((n > 0) && ((n + 1) % 10 == 0)) {
            printf("\n");
        }
    }

    printf("\n");
}

SharedLibrary *SharedLibrary::New(const char *fileName)
{
    SharedLibrary *sl = new SharedLibrary();

    if (sl->load(fileName)) {
        return sl;
    } else {
        delete sl;
        return NULL;
    }
}

SharedLibrary::SharedLibrary(): library(NULL)
{
}

SharedLibrary::~SharedLibrary()
{
#if defined WINDOWS

    if (library) {
        FreeLibrary(library);
    }

#elif defined LINUX

    if (library) {
        dlclose(library);
    }

#endif
}

SharedLibrary *SharedLibrary::load(const char *fileName)
{
#if defined WINDOWS
    library = LoadLibrary(TEXT(fileName));

    if (!library) {
        DWORD error = GetLastError();
        std::cerr << "> Error: unable to load shared library " << fileName << " :" << error << std::endl;
        return NULL;
    }

#elif defined LINUX
    /*
     * libraries on Linux are called 'lib<name>.so'
     * if the passed in fileName does not have those
     * components add them in.
     */
    char *libraryName = (char *)calloc(1, strlen(fileName) + 6 + 1);

    if (strstr(fileName, "lib") == NULL) {
        strcat(libraryName, "lib");
    }

    strcat(libraryName, fileName);

    if (strstr(fileName + (strlen(fileName) - 3), ".so") == NULL) {
        strcat(libraryName, ".so");
    }

    library = dlopen(libraryName, RTLD_NOW | RTLD_GLOBAL);

    if (!library) {
        std::cout << "> Error: unable to load shared library " << fileName << " :" << dlerror() << std::endl;
        free(libraryName);
        return NULL;
    }

    free(libraryName);
#endif
    return this;
}

////////////////////////////////////////////////////////////////////////////////////////////////

std::string Time::ToString_seconds(uint64_t t)
{
    uint64_t us = t % 1000;
    uint64_t ms = t / 1000;
    uint64_t s = ms / 1000;
    ms = ms % 1000;
    std::string _s = String::Uint2String(s);
    _s += "s:";
    _s += String::Uint2String(ms);
    _s += "ms:";
    _s += String::Uint2String(us);
    _s += "us";
    return _s;
}

std::string Time::ToString_year(uint64_t t)
{
    uint64_t us = t % 1000;
    uint64_t ms = t / 1000;
    uint64_t s = ms / 1000;
    ms = ms % 1000;
    time_t _gmt_time;
    time(&_gmt_time);
    struct tm   *_t = gmtime(&_gmt_time);
    std::string _s = asctime(_t); // _s is: Www Mmm dd hh:mm:ss yyyy but we want: Www Mmm dd yyyy hh:mm:ss:msmsms:ususus
    std::string year = _s.substr(_s.length() - 5, 4);
    _s.erase(_s.length() - 6, 5);
    std::string hh_mm_ss = _s.substr(_s.length() - 9, 8);
    hh_mm_ss += ":";
    hh_mm_ss += String::Uint2String(ms);
    hh_mm_ss += ":";
    hh_mm_ss += String::Uint2String(us);
    _s.erase(_s.length() - 9, 9);
    _s += year;
    _s += " ";
    _s += hh_mm_ss;
    _s += " GMT";
    return _s;
}

////////////////////////////////////////////////////////////////////////////////////////////////

uint8_t Host::Name(char *name)
{
#if defined WINDOWS
    DWORD s = 255;
    GetComputerName(name, &s);
    return (uint8_t)s;
#elif defined LINUX
    struct utsname utsname;
    uname(&utsname);
    strcpy(name, utsname.nodename);
    return strlen(name);
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////

#if defined WINDOWS
const uint32_t Semaphore::Infinite = INFINITE;
#elif defined LINUX
/*
 * Normally this should be SEM_VALUE_MAX but apparently the <semaphore.h> header
 * does not define it. The documents I have read indicate that on Linux it is
 * always equal to INT_MAX - so use that instead.
 */
const uint32_t Semaphore::Infinite = INT_MAX;
#endif

Semaphore::Semaphore(uint32_t initialCount, uint32_t maxCount)
{
#if defined WINDOWS
    s = CreateSemaphore(NULL, initialCount, maxCount, NULL);
#elif defined LINUX
    sem_init(&s, 0, initialCount);
#endif
}

Semaphore::~Semaphore()
{
#if defined WINDOWS
    CloseHandle(s);
#elif defined LINUX
    sem_destroy(&s);
#endif
}

bool Semaphore::acquire(uint32_t timeout)
{
#if defined WINDOWS
    uint32_t r = WaitForSingleObject(s, timeout);
    return r == WAIT_TIMEOUT;
#elif defined LINUX
    struct timespec t;
    int r;
    CalcTimeout(t, timeout);
    r = sem_timedwait(&s, &t);
    return r == 0;
#endif
}

void Semaphore::release(uint32_t count)
{
#if defined WINDOWS
    ReleaseSemaphore(s, count, NULL);
#elif defined LINUX

    for (uint32_t c = 0; c < count; c++) {
        sem_post(&s);
    }

#endif
}

void Semaphore::reset()
{
#if defined WINDOWS
    bool r;

    do {
        r = acquire(0);
    } while (!r);

#elif defined LINUX
    bool r;

    do {
        r = acquire(0);
    } while (!r);

#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////

CriticalSection::CriticalSection()
{
#if defined WINDOWS
    InitializeCriticalSection(&cs);
#elif defined LINUX
    pthread_mutex_init(&cs, NULL);
#endif
}

CriticalSection::~CriticalSection()
{
#if defined WINDOWS
    DeleteCriticalSection(&cs);
#elif defined LINUX
    pthread_mutex_destroy(&cs);
#endif
}

void CriticalSection::enter()
{
#if defined WINDOWS
    EnterCriticalSection(&cs);
#elif defined LINUX
    pthread_mutex_lock(&cs);
#endif
}

void CriticalSection::leave()
{
#if defined WINDOWS
    LeaveCriticalSection(&cs);
#elif defined LINUX
    pthread_mutex_unlock(&cs);
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////

void SignalHandler::Add(sighandler_t h)
{
#if defined WINDOWS

    if (SetConsoleCtrlHandler(h, true) == 0) {
        int e = GetLastError();
        std::cerr << "Error: " << e << " failed to add signal handler" << std::endl;
        return;
    }

#elif defined LINUX
    signal(SIGABRT, SIG_IGN);
    signal(SIGPIPE, SIG_IGN);
    signal(SIGBUS, SIG_IGN);
//   signal(SIGHUP, h);
    signal(SIGTERM, h);
    signal(SIGINT, h);
    signal(SIGABRT, h);
//   signal(SIGFPE, h);
//   signal(SIGILL, h);
//   signal(SIGSEGV, h);
#endif
}

void SignalHandler::Remove(signal_handler h)
{
#if defined WINDOWS
    SetConsoleCtrlHandler(h, false);
#elif defined LINUX
    signal(SIGABRT, SIG_IGN);
    signal(SIGPIPE, SIG_IGN);
    signal(SIGBUS, SIG_IGN);
//   signal(SIGHUP, SIG_DFL);
    signal(SIGTERM, SIG_DFL);
    signal(SIGINT, SIG_DFL);
    signal(SIGABRT, SIG_DFL);
//   signal(SIGFPE, SIG_DFL);
//   signal(SIGILL, SIG_DFL);
//   signal(SIGSEGV, SIG_DFL);
#endif
}


////////////////////////////////////////////////////////////////////////////////////////////////

uint8_t BSR(uint32_t data)
{
#if defined WINDOWS
#if defined ARCH_32
    DWORD index;
    _BitScanReverse(&index, data);
    return (uint8_t)index;
#elif defined ARCH_64
    uint64_t index;
    _BitScanReverse64(&index, data);
    return (uint8_t)index;
#endif
#elif defined LINUX
#if defined ARCH_32
    return (uint8_t)(31 - __builtin_clz((uint32_t)data));
#elif defined ARCH_64
    return (uint8_t)(63 - __builtin_clzll((uint64_t)data));
#endif
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////

FastSemaphore::FastSemaphore(uint32_t initialCount, uint32_t maxCount): Semaphore(initialCount > 0 ? 1 : 0, 1), count(initialCount), maxCount(maxCount)
{
}

FastSemaphore::~FastSemaphore()
{
}

void FastSemaphore::acquire()
{
    int32_t c;

    while ((c = --count) >= maxCount); // release calls can bring count over maxCount: acquire has to exhaust these extras

    if (c < 0) {
        Semaphore::acquire();
    }
}

void FastSemaphore::release()
{
    int32_t c = ++count;

    if (c <= 0) {
        Semaphore::release();
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////
/*
 FastMutex::FastMutex(uint32_t initialCount):Semaphore(initialCount,1),count(initialCount){
 }

 FastMutex::~FastMutex(){
 }

 void FastMutex::acquire(){

 int32_t former=Atomic::Swap32(&count,0);
 if(former==0)
 Semaphore::acquire();
 }

 void FastMutex::release(){

 int32_t former=Atomic::Swap32(&count,1);
 if(former==0)
 Semaphore::release();
 }
 */
////////////////////////////////////////////////////////////////////////////////////////////////

bool Error::PrintLastOSErrorMessage(const char *title)
{
    int32_t err = Error::GetLastOSErrorNumber();
    char buf[1024];

    if (!Error::GetOSErrorMessage(buf, 1024, err)) {
        printf("%s: [%d] (could not get error message)\n", title, err);
    } else {
        printf("%s: [%d] %s\n", title, err, buf);
    }

    return true;
}

int32_t Error::GetLastOSErrorNumber()
{
#ifdef WINDOWS
    int32_t err = WSAGetLastError();
    WSASetLastError(0);
    return err;
#else
    return (int32_t) errno;
#endif
}

bool Error::GetOSErrorMessage(char *buffer, uint32_t buflen, int32_t err)
{
    if (buffer == NULL) {
        return false;
    }

    if (buflen < 512) {
        strcpy(buffer, "String buffer not large enough");
        return false;
    }

    if (err < 0) {
        err = Error::GetLastOSErrorNumber();
    }

#ifdef WINDOWS

    if (err == WSANOTINITIALISED) {
        strcpy(buffer, "Cannot initialize WinSock!");
    } else if (err == WSAENETDOWN) {
        strcpy(buffer, "The network subsystem or the associated service provider has failed");
    } else if (err == WSAEAFNOSUPPORT) {
        strcpy(buffer, "The specified address family is not supported");
    } else if (err == WSAEINPROGRESS) {
        strcpy(buffer, "A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function");
    } else if (err == WSAEMFILE) {
        strcpy(buffer, "No more socket descriptors are available");
    } else if (err == WSAENOBUFS) {
        strcpy(buffer, "No buffer space is available. The socket cannot be created");
    } else if (err == WSAEPROTONOSUPPORT) {
        strcpy(buffer, "The specified protocol is not supported");
    } else if (err == WSAEPROTOTYPE) {
        strcpy(buffer, "The specified protocol is the wrong type for this socket");
    } else if (err == WSAESOCKTNOSUPPORT) {
        strcpy(buffer, "The specified socket type is not supported in this address family");
    } else if (err == WSAEADDRINUSE) {
        strcpy(buffer, "The socket's local address is already in use and the socket was not marked to allow address reuse with SO_REUSEADDR. This error usually occurs during execution of the bind function, but could be delayed until this function if the bind was to a partially wildcard address (involving ADDR_ANY) and if a specific address needs to be committed at the time of this function");
    } else if (err == WSAEINVAL) {
        strcpy(buffer, "The socket has not been bound with bind");
    } else if (err == WSAEISCONN) {
        strcpy(buffer, "The socket is already connected");
    } else if (err == WSAENOTSOCK) {
        strcpy(buffer, "The descriptor is not a socket");
    } else if (err == WSAEOPNOTSUPP) {
        strcpy(buffer, "The referenced socket is not of a type that supports the listen operation");
    } else if (err == WSAEADDRNOTAVAIL) {
        strcpy(buffer, "The specified address is not a valid address for this machine");
    } else if (err == WSAEFAULT) {
        strcpy(buffer, "The name or namelen parameter is not a valid part of the user address space, the namelen parameter is too small, the name parameter contains an incorrect address format for the associated address family, or the first two bytes of the memory block specified by name does not match the address family associated with the socket descriptor s");
    } else if (err == WSAEMFILE) {
        strcpy(buffer, "The queue is nonempty upon entry to accept and there are no descriptors available");
    } else if (err == SOCKETWOULDBLOCK) {
        strcpy(buffer, "The socket is marked as nonblocking and no connections are present to be accepted");
    } else if (err == WSAETIMEDOUT) {
        strcpy(buffer, "Attempt to connect timed out without establishing a connection");
    } else if (err == WSAENETUNREACH) {
        strcpy(buffer, "The network cannot be reached from this host at this time");
    } else if (err == WSAEISCONN) {
        strcpy(buffer, "The socket is already connected (connection-oriented sockets only)");
    } else if (err == WSAECONNREFUSED) {
        strcpy(buffer, "The attempt to connect was forcefully rejected");
    } else if (err == WSAEAFNOSUPPORT) {
        strcpy(buffer, "Addresses in the specified family cannot be used with this socket");
    } else if (err == WSAEADDRNOTAVAIL) {
        strcpy(buffer, "The remote address is not a valid address (such as ADDR_ANY)");
    } else if (err == WSAEALREADY) {
        strcpy(buffer, "A nonblocking connect call is in progress on the specified socket");
    } else if (err == WSAECONNRESET) {
        strcpy(buffer, "Connection was reset");
    } else if (err == WSAECONNABORTED) {
        strcpy(buffer, "Software caused connection abort");
    } else {
        strcpy(buffer, "Socket error with no description");
    }

#else
    strcpy(buffer, strerror(err));
#endif
    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////

bool WaitForSocketReadability(socket s, int32_t timeout)
{
    int maxfd = 0;
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 0;
    fd_set rdds;
    // create a list of sockets to check for activity
    FD_ZERO(&rdds);
    // specify mySocket
    FD_SET(s, &rdds);
#ifdef WINDOWS
#else
    maxfd = s + 1;
#endif

    if (timeout > 0) {
        ldiv_t d = ldiv(timeout * 1000, 1000000);
        tv.tv_sec = d.quot;
        tv.tv_usec = d.rem;
    }

    // Check for readability
    int ret = select(maxfd, &rdds, NULL, NULL, &tv);
    return (ret > 0);
}

bool WaitForSocketWriteability(socket s, int32_t timeout)
{
    int maxfd = 0;
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 0;
    fd_set wds;
    // create a list of sockets to check for activity
    FD_ZERO(&wds);
    // specify mySocket
    FD_SET(s, &wds);
#ifdef WINDOWS
#else
    maxfd = s + 1;
#endif

    if (timeout > 0) {
        ldiv_t d = ldiv(timeout * 1000, 1000000);
        tv.tv_sec = d.quot;
        tv.tv_usec = d.rem;
    }

    // Check for readability
    return (select(maxfd, NULL, &wds, NULL, &tv) > 0);
}

////////////////////////////////////////////////////////////////////////////////////////////////

int32_t String::StartsWith(const std::string &s, const std::string &str)
{
    std::string::size_type pos = s.find_first_of(str);

    if (pos == 0) {
        return 0;
    } else {
        return -1;
    }
}

int32_t String::EndsWith(const std::string &s, const std::string &str)
{
    std::string::size_type pos = s.find_last_of(str);

    if (pos == s.size() - str.size()) {
        return pos;
    } else {
        return -1;
    }
}

void String::MakeUpper(std::string &str)
{
    std::transform(str.begin(), str.end(), str.begin(), toupper);
}

void String::MakeLower(std::string &str)
{
    std::transform(str.begin(), str.end(), str.begin(), tolower);
}

void String::Trim(std::string &str, const char *chars2remove)
{
    TrimLeft(str, chars2remove);
    TrimRight(str, chars2remove);
}

void String::TrimLeft(std::string &str, const char *chars2remove)
{
    if (!str.empty()) {
        std::string::size_type pos = str.find_first_not_of(chars2remove);

        if (pos != std::string::npos) {
            str.erase(0, pos);
        } else {
            str.erase(str.begin() , str.end());    // make empty
        }
    }
}

void String::TrimRight(std::string &str, const char *chars2remove)
{
    if (!str.empty()) {
        std::string::size_type pos = str.find_last_not_of(chars2remove);

        if (pos != std::string::npos) {
            str.erase(pos + 1);
        } else {
            str.erase(str.begin() , str.end());    // make empty
        }
    }
}


void String::ReplaceLeading(std::string &str, const char *chars2replace, char c)
{
    if (!str.empty()) {
        std::string::size_type pos = str.find_first_not_of(chars2replace);

        if (pos != std::string::npos) {
            str.replace(0, pos, pos, c);
        } else {
            int n = str.size();
            str.replace(str.begin(), str.end() - 1, n - 1, c);
        }
    }
}

std::string String::Int2String(int64_t i)
{
    char buffer[1024];
    sprintf(buffer, "%d", i);
    return std::string(buffer);
}

std::string String::Uint2String(uint64_t i)
{
    char buffer[1024];
    sprintf(buffer, "%u", i);
    return std::string(buffer);
}

////////////////////////////////////////////////////////////////////////////////////////////////

int32_t Random::r250_index;
int32_t Random::r521_index;
uint32_t Random::r250_buffer[R250_LEN];
uint32_t Random::r521_buffer[R521_LEN];

void Random::Init()
{
    int32_t i = R521_LEN;
    uint32_t mask1 = 1;
    uint32_t mask2 = 0xFFFFFFFF;

    while (i-- > R250_LEN) {
        r521_buffer[i] = rand();
    }

    while (i-- > 31) {
        r250_buffer[i] = rand();
        r521_buffer[i] = rand();
    }

    // Establish linear independence of the bit columns
    // by setting the diagonal bits and clearing all bits above
    while (i-- > 0) {
        r250_buffer[i] = (rand() | mask1) & mask2;
        r521_buffer[i] = (rand() | mask1) & mask2;
        mask2 ^= mask1;
        mask1 >>= 1;
    }

    r250_buffer[0] = mask1;
    r521_buffer[0] = mask2;
    r250_index = 0;
    r521_index = 0;
}

float Random::operator()(uint32_t range)
{
    /*
    I prescale the indices by sizeof(unsigned long) to eliminate
    four shlwi instructions in the compiled code.  This minor optimization
    increased perf by about 12%.

    I also carefully arrange index increments and comparisons to minimize
    instructions.  gcc 3.3 seems a bit weak on instruction reordering. The
    j1/j2 branches are mispredicted, but nevertheless these optimizations
    increased perf by another 10%.
    */
    int32_t i1 = r250_index;
    int32_t i2 = r521_index;
    uint8_t *b1 = (uint8_t *)r250_buffer;
    uint8_t *b2 = (uint8_t *)r521_buffer;
    uint32_t *tmp1, *tmp2;
    uint32_t r, s;
    int32_t j1, j2;
    j1 = i1 - R250_IB;

    if (j1 < 0) {
        j1 = i1 + R250_IA;
    }

    j2 = i2 - R521_IB;

    if (j2 < 0) {
        j2 = i2 + R521_IA;
    }

    tmp1 = (uint32_t *)(b1 + i1);
    r = (*(uint32_t *)(b1 + j1)) ^ (*tmp1);
    *tmp1 = r;
    tmp2 = (uint32_t *)(b2 + i2);
    s = (*(uint32_t *)(b2 + j2)) ^ (*tmp2);
    *tmp2 = s;
    i1 = (i1 != sizeof(uint32_t) * (R250_LEN - 1)) ? (i1 + sizeof(uint32_t)) : 0;
    r250_index = i1;
    i2 = (i2 != sizeof(uint32_t) * (R521_LEN - 1)) ? (i2 + sizeof(uint32_t)) : 0;
    r521_index = i2;
    float _r = r ^ s;
    //return range*(_r/((float)ULONG_MAX));
    return _r;
}
}
