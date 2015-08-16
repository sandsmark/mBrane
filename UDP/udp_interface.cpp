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


#include "udp_interface.h"
#include "udp_channel.h"

#if defined (WINDOWS)
#include <iphlpapi.h>
#endif

#if defined (LINUX)
#include <sys/ioctl.h>
#include <net/if.h>
#endif

#include <cstring>

uint32_t UDPInterface::Intialized = 0;

bool UDPInterface::Init()
{
    if (Intialized) {
        Intialized++;
        return true;
    }

#if defined (WINDOWS)
    WSADATA wsaData;
    int32_t r;
    r = WSAStartup(MAKEWORD(2, 2), &wsaData);

    if (r) {
        std::cout << "> Error: WSAStartup failed: " << r << std::endl;
        return false;
    }

#endif
    Intialized++;
    return true;
}

void UDPInterface::Shutdown()
{
    if (!--Intialized) {
#if defined (WINDOWS)
        WSACleanup();
#endif
    }
}

UDPInterface *UDPInterface::New(XMLNode &n)
{
    if (!Init()) {
        return NULL;
    }

    UDPInterface *i = new UDPInterface();

    if (!i->load(n)) {
        delete i;
        Shutdown();
        return NULL;
    }

    return i;
}

UDPInterface::UDPInterface(): NetworkInterface(UDP)
{
}

UDPInterface::~UDPInterface()
{
    Shutdown();
}

bool UDPInterface::load(XMLNode &n)
{
    const char *_nic = n.getAttribute("nic");

    if (!_nic) {
        std::cout << "> Error: NodeConfiguration::Network::" << n.getName() << "::nic is missing" << std::endl;
        return false;
    }

    unsigned char *socketAddr;
    char ipAddressString[16];
    bool found = false;
    bool gotIPAddress = false;
#if defined (WINDOWS)
    char adaptorString[128];
    PIP_ADAPTER_ADDRESSES pAddresses = NULL;
    ULONG iterations = 0;
    DWORD dwRetVal = 0;
    ULONG outBufLen = 15000;
    unsigned int i = 0;
    // Set the flags to pass to GetAdaptersAddresses
    ULONG flags = GAA_FLAG_INCLUDE_PREFIX;
    // default to unspecified address family (both)
    ULONG family = AF_UNSPEC;
    LPVOID lpMsgBuf = NULL;
    PIP_ADAPTER_ADDRESSES pCurrAddresses = NULL;
    PIP_ADAPTER_UNICAST_ADDRESS pUnicast = NULL;
    PIP_ADAPTER_ANYCAST_ADDRESS pAnycast = NULL;
    PIP_ADAPTER_MULTICAST_ADDRESS pMulticast = NULL;
    IP_ADAPTER_DNS_SERVER_ADDRESS *pDnServer = NULL;
    IP_ADAPTER_PREFIX *pPrefix = NULL;

    do {
        pAddresses = (IP_ADAPTER_ADDRESSES *) malloc(outBufLen);

        if (pAddresses == NULL) {
            std::cout << "> Error: could not get adapter info" << std::endl;
            return false;
        }

        dwRetVal = GetAdaptersAddresses(family, flags, NULL, pAddresses, &outBufLen);

        if (dwRetVal == ERROR_BUFFER_OVERFLOW) {
            free(pAddresses);
            pAddresses = NULL;
        } else {
            break;
        }

        iterations++;
    } while ((dwRetVal == ERROR_BUFFER_OVERFLOW) && (iterations < 3));

    if (dwRetVal == NO_ERROR) {
        // If successful, output some information from the data we received
        pCurrAddresses = pAddresses;

        while (pCurrAddresses) {
            gotIPAddress = false;

            if ((pCurrAddresses->OperStatus == IfOperStatusUp) && (pCurrAddresses->IfIndex != 0)) {
                // First check the IP address
                pUnicast = pCurrAddresses->FirstUnicastAddress;

                if (pUnicast != NULL) {
                    for (i = 0; pUnicast != NULL; i++) {
                        socketAddr = ((unsigned char *)((SOCKADDR *)(pUnicast->Address.lpSockaddr))->sa_data) + 2;

                        if ((socketAddr[0] != 0) && (socketAddr[0] != 127)) {
                            sprintf(ipAddressString, "%u.%u.%u.%u", socketAddr[0], socketAddr[1], socketAddr[2], socketAddr[3]);

                            if (strstr(ipAddressString, _nic) != NULL) {
                                found = true;
                            }

                            // For now only look at the first non-0 and non-127 address on each adaptor
                            gotIPAddress = true;
                            break;
                        }

                        pUnicast = pUnicast->Next;
                    }
                }

                // Then check the adaptor description field
                if ((!found) && gotIPAddress) {
                    if (WideCharToMultiByte(CP_ACP, 0, pCurrAddresses->Description, -1, adaptorString, 128, NULL, NULL) != 0) {
                        if (strstr(adaptorString, _nic) != NULL) {
                            found = true;
                        }
                    }
                }

                if (found) {
                    address.s_addr = inet_addr(ipAddressString);
                    break;
                }

                //printf("\tAdapter name: %s\n", pCurrAddresses->AdapterName);
                //printf("\tDescription: %wS\n", pCurrAddresses->Description);
                //printf("\tFriendly name: %wS\n", pCurrAddresses->FriendlyName);
                //printf("\n");
            }

            pCurrAddresses = pCurrAddresses->Next;
        }
    } else {
        if (pAddresses) {
            free(pAddresses);
        }

        return false;
    }

    if (pAddresses) {
        free(pAddresses);
    }

#endif
#if defined (LINUX)
    int sock = 0;
    struct ifreq ifreq;
    struct sockaddr_in *saptr = NULL;
    struct if_nameindex *iflist = NULL, *listsave = NULL;

    //need a socket for ioctl()
    if ((sock = ::socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        return false;
    }

    //returns pointer to dynamically allocated list of structs
    iflist = listsave = if_nameindex();

    if (iflist == NULL) {
        close(sock);
        return false;
    }

    //walk thru the array returned and query for each
    //interface's address
    for (iflist; iflist->if_index != 0; iflist++) {
        gotIPAddress = false;
        //copy in the interface name to look up address of
        strncpy(ifreq.ifr_name, iflist->if_name, IF_NAMESIZE);

        //get the address for this interface
        if (ioctl(sock, SIOCGIFADDR, &ifreq) != 0) {
            // ignore;
            continue;
        }

        saptr = (struct sockaddr_in *)&ifreq.ifr_addr;
        // First check the IP Address
        socketAddr = (unsigned char *) & (saptr->sin_addr.s_addr);

        if ((socketAddr[0] != 0) && (socketAddr[0] != 127)) {
            sprintf(ipAddressString, "%u.%u.%u.%u", socketAddr[0], socketAddr[1], socketAddr[2], socketAddr[3]);

            if (strstr(ipAddressString, _nic) != NULL) {
                found = true;
            }

            // For now only look at the first non-0 and non-127 address on each adaptor
            gotIPAddress = true;
        }

        // Then check the adaptor name
        if ((!found) && gotIPAddress) {
            if (strstr(ifreq.ifr_name, _nic) != NULL) {
                found = true;
            }
        }

        if (found) {
            memcpy(&address, &saptr->sin_addr, sizeof(struct in_addr));
            break;
        }
    }

    //free the dynamic memory kernel allocated for us
    if_freenameindex(listsave);
    close(sock);
#endif

    if (!found) {
        std::cout << "> Error: NodeConfiguration::Network::" << n.getName() << "::nic " << _nic << " does not exist" << std::endl;
        return false;
    }

    const char *_port = n.getAttribute("port");

    if (!_port) {
        std::cout << "> Error: NodeConfiguration::Network::" << n.getName() << "::port is missing" << std::endl;
        return false;
    }

    port = atoi(_port);
    return true;
}

bool UDPInterface::operator ==(NetworkInterface &i)
{
    if (i.protocol() != protocol()) {
        return false;
    }

    return strcmp(inet_ntoa(address), inet_ntoa(((UDPInterface *)&i)->address)) == 0 && port == ((UDPInterface *)&i)->port;
}

bool UDPInterface::operator !=(NetworkInterface &i)
{
    return !operator ==(i);
}

bool UDPInterface::canBroadcast()
{
    return true;
}

uint16_t UDPInterface::start()
{
    return 0;
}

uint16_t UDPInterface::stop()
{
    Shutdown();
    return 0;
}

uint16_t UDPInterface::getIDSize()
{
    return sizeof(struct in_addr) + sizeof(uint32_t);
}

void UDPInterface::fillID(uint8_t *ID)  // address|port
{
    memcpy(ID, &address, sizeof(struct in_addr));
    memcpy(ID + sizeof(struct in_addr), &port, sizeof(uint32_t));
}

uint16_t UDPInterface::newChannel(uint8_t *ID, CommChannel **channel)
{
    core::socket s;

    if ((s =::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == SOCKET_ERROR) {
        closesocket(s);
        Shutdown();
        return 1;
    }

    //bool on=true;
    int one = 1;
    setsockopt(s, SOL_SOCKET, SO_BROADCAST, (char *)&one, sizeof(one));
    /* we also need to bind the listener to the right port */
    struct sockaddr_in if_addr;
    if_addr.sin_family = AF_INET;
    if_addr.sin_addr.s_addr = INADDR_ANY; // Since this is a server, any address will do
    if_addr.sin_port = htons((unsigned short)port);

    if (bind(s, (struct sockaddr *)&if_addr, sizeof(struct sockaddr_in)) < 0) {
        std::cout << "> Error: unable to bind to port " << port << std::endl;
        return 1;
    }

    std::cout << "> Info: UDP bound to port " << port << std::endl;
    *channel = new UDPChannel(s, port);
    return 0;
}

uint16_t UDPInterface::acceptConnection(ConnectedCommChannel **channel, int32_t timeout, bool &timedout)
{
    return 1;
}
