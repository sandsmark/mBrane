//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
//_/_/
//_/_/ HUMANOBS - mBrane - UDP
//_/_/
//_/_/ Eric Nivel
//_/_/ Center for Analysis and Design of Intelligent Agents
//_/_/   Reykjavik University, Menntavegur 1, 101 Reykjavik, Iceland
//_/_/   http://cadia.ru.is
//_/_/ Copyright(c)2012
//_/_/
//_/_/ This software was developed by the above copyright holder as part of
//_/_/ the HUMANOBS EU research project, in collaboration with the
//_/_/ following parties:
//_/_/
//_/_/ Autonomous Systems Laboratory
//_/_/   Technical University of Madrid, Spain
//_/_/   http://www.aslab.org/
//_/_/
//_/_/ Communicative Machines
//_/_/   Edinburgh, United Kingdom
//_/_/   http://www.cmlabs.com/
//_/_/
//_/_/ Istituto Dalle Molle di Studi sull'Intelligenza Artificiale
//_/_/   University of Lugano and SUPSI, Switzerland
//_/_/   http://www.idsia.ch/
//_/_/
//_/_/ Institute of Cognitive Sciences and Technologies
//_/_/   Consiglio Nazionale delle Ricerche, Italy
//_/_/   http://www.istc.cnr.it/
//_/_/
//_/_/ Dipartimento di Ingegneria Informatica
//_/_/   University of Palermo, Italy
//_/_/   http://roboticslab.dinfo.unipa.it/index.php/Main/HomePage
//_/_/
//_/_/
//_/_/ --- HUMANOBS Open-Source BSD License, with CADIA Clause v 1.0 ---
//_/_/
//_/_/ Redistribution and use in source and binary forms, with or without
//_/_/ modification, is permitted provided that the following conditions
//_/_/ are met:
//_/_/
//_/_/ - Redistributions of source code must retain the above copyright
//_/_/ and collaboration notice, this list of conditions and the
//_/_/ following disclaimer.
//_/_/
//_/_/ - Redistributions in binary form must reproduce the above copyright
//_/_/ notice, this list of conditions and the following
//_/_/ disclaimer in the documentation and/or other materials provided
//_/_/ with the distribution.
//_/_/
//_/_/ - Neither the name of its copyright holders nor the names of its
//_/_/ contributors may be used to endorse or promote products
//_/_/ derived from this software without specific prior written permission.
//_/_/
//_/_/ - CADIA Clause: The license granted in and to the software under this
//_/_/ agreement is a limited-use license. The software may not be used in
//_/_/ furtherance of:
//_/_/ (i) intentionally causing bodily injury or severe emotional distress
//_/_/ to any person;
//_/_/ (ii) invading the personal privacy or violating the human rights of
//_/_/ any person; or
//_/_/ (iii) committing or preparing for any act of war.
//_/_/
//_/_/ THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
//_/_/ "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
//_/_/ LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
//_/_/ A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
//_/_/ OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
//_/_/ SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
//_/_/ LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
//_/_/ DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
//_/_/ THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//_/_/ (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
//_/_/ OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//_/_/
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/


#include "udp_channel.h"


using namespace mBrane;
using namespace mBrane::sdk;

UDPChannel::UDPChannel(core::socket s, uint32_t port): BroadcastCommChannel(), s(s)
{
    bcast_address.sin_family = AF_INET;
    bcast_address.sin_port = htons((unsigned short)port);
    bcast_address.sin_addr.s_addr = INADDR_BROADCAST;
    bufferLen = 0;
    buffer = NULL;
    bufferContentLen = 0;
    bufferContentPos = 0;
    initialiseBuffer(4096);
}

UDPChannel::~UDPChannel()
{
    std::lock_guard<std::mutex> guard(m_mutex);

    disconnect();

    if (buffer != NULL) {
        delete(buffer);
    }

    bufferLen = 0;
    bufferContentLen = 0;
    bufferContentPos = 0;
    buffer = NULL;
}

bool UDPChannel::initialiseBuffer(uint32_t len)
{
    if (len < 128) {
        return false;
    }

    std::lock_guard<std::mutex> guard(m_mutex);

    if (buffer != NULL) {
        delete(buffer);
    }

    bufferLen = len;
    buffer = new char[len];
    bufferContentLen = 0;
    bufferContentPos = 0;

    return true;
}

int16_t UDPChannel::send(uint8_t *b, size_t s)
{
    if (::sendto(this->s, (char *)b, (int)s, 0, (SOCKADDR *)&bcast_address, sizeof(sockaddr_in)) == SOCKET_ERROR) {
        return 1;
    }

    return 0;
}

int16_t UDPChannel::recv(uint8_t *b, size_t s, bool peek)
{
    m_mutex.lock();

    if (s > bufferLen) {
        m_mutex.unlock();
        initialiseBuffer(s * 2);
        m_mutex.lock();
    }

    // Do we have enough data in the buffer already
    if (bufferContentLen - bufferContentPos < s) {
        // if not, discard what we have in the buffer now
        bufferContentLen = bufferContentPos = 0;
        // and read from the socket
        int count = ::recvfrom(this->s, buffer, bufferLen, 0, NULL, 0);

        if (count == SOCKET_ERROR) {
            // Error::PrintLastOSErrorMessage("Error: UDPChannel::recv");
            m_mutex.unlock();
            return 1;
        }

        bufferContentLen = count;
        // std::cout<<"Info: Not enough data in buffer, received "<<count<<" bytes from socket..."<<std::endl;
    }

    // Do we now have enough data in the buffer
    if (bufferContentLen - bufferContentPos < s) {
        // if not, we give up
        m_mutex.unlock();
        // std::cout<<"UDP Error: Not enough data in buffer, have "<<bufferContentLen-bufferContentPos<<" bytes, need "<<s<<" bytes..."<<std::endl;
        return 1;
    }

    // if yes, great
    memcpy(b, buffer + bufferContentPos, s);

    if (!peek) {
        bufferContentPos += s;
    }

    m_mutex.unlock();
// std::cout<<"Info: Read "<<s<<" bytes from buffer, "<<bufferContentLen-bufferContentPos<<" bytes left..."<<std::endl;
    return 0;
}

bool UDPChannel::isConnected()
{
    return true;
}

bool UDPChannel::disconnect()
{
    if (s != INVALID_SOCKET) {
        shutdown(s, SD_BOTH);
        closesocket(s);
    }

    s = INVALID_SOCKET;
    return true;
}
