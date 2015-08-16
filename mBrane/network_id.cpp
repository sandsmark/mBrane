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

#include	"network_id.h"

#include	<cstring>

namespace	mBrane
{

uint16_t	NetworkID::DiscoveryIDSize = 0;

uint16_t	NetworkID::Size = 0;

uint16_t	NetworkID::CtrlIDSize[2] = {	0,	0	};

uint16_t	NetworkID::DataIDSize[2] = {	0,	0	};

uint16_t	NetworkID::StreamIDSize[2] = {	0,	0	};

NetworkID::NetworkID(): data(NULL)
{
}

NetworkID::NetworkID(uint8_t	NID, Node::Network	description, uint8_t	nameSize, char	*name)
{
    Size = DiscoveryIDSize;
    headerSize = 3 + nameSize;

    switch (description) {
    case	Node::PRIMARY:
        Size += CtrlIDSize[Node::PRIMARY] + DataIDSize[Node::PRIMARY] + StreamIDSize[Node::PRIMARY];
        break;

    case	Node::SECONDARY:
        Size += CtrlIDSize[Node::SECONDARY] + DataIDSize[Node::SECONDARY] + StreamIDSize[Node::SECONDARY];
        break;

    case	Node::BOTH:
        Size += CtrlIDSize[Node::PRIMARY] + DataIDSize[Node::PRIMARY] + StreamIDSize[Node::PRIMARY] + CtrlIDSize[Node::SECONDARY] + DataIDSize[Node::SECONDARY] + StreamIDSize[Node::SECONDARY];
        break;
    }

    data = new	uint8_t[headerSize + Size];
    data[0] = NID;
    data[1] = description;
    data[2] = nameSize;
    memcpy(data + 3, name, nameSize);
}

NetworkID::~NetworkID()
{
    if (data) {
        delete	data;
    }
}

uint8_t	NetworkID::NID()	const
{
    return	data[0];
}

uint8_t	NetworkID::setNID(uint8_t newNID)
{
    data[0] = newNID;
    return 0;
}

Node::Network	NetworkID::network()	const
{
    return	(Node::Network)data[1];
}

char	*NetworkID::name()	const
{
    return	(char *)(data + 3);
}

uint8_t	*NetworkID::at(InterfaceType	t)	const
{
    switch (t) {
    case	DISCOVERY:
        return	data + headerSize;

    case	CONTROL_PRIMARY:
        switch (data[1]) {
        case	Node::BOTH:
        case	Node::PRIMARY:
            return	data + headerSize + DiscoveryIDSize;

        case	Node::SECONDARY:
            return	NULL;
        }

    case	DATA_PRIMARY:
        switch (data[1]) {
        case	Node::BOTH:
        case	Node::PRIMARY:
            return	data + headerSize + DiscoveryIDSize + CtrlIDSize[Node::PRIMARY];

        case	Node::SECONDARY:
            return	NULL;
        }

    case	STREAM_PRIMARY:
        switch (data[1]) {
        case	Node::BOTH:
        case	Node::PRIMARY:
            return	data + headerSize + DiscoveryIDSize + CtrlIDSize[Node::PRIMARY] + DataIDSize[Node::PRIMARY];

        case	Node::SECONDARY:
            return	NULL;
        }

    case	CONTROL_SECONDARY:
        switch (data[1]) {
        case	Node::PRIMARY:
            return	NULL;

        case	Node::SECONDARY:
            return	data + headerSize + DiscoveryIDSize;

        case	Node::BOTH:
            return	data + headerSize + DiscoveryIDSize + CtrlIDSize[Node::PRIMARY] + DataIDSize[Node::PRIMARY] + StreamIDSize[Node::PRIMARY];
        }

    case	DATA_SECONDARY:
        switch (data[1]) {
        case	Node::PRIMARY:
            return	NULL;

        case	Node::SECONDARY:
            return	data + headerSize + DiscoveryIDSize + CtrlIDSize[Node::SECONDARY];

        case	Node::BOTH:
            return	data + headerSize + DiscoveryIDSize + CtrlIDSize[Node::PRIMARY] + DataIDSize[Node::PRIMARY] + StreamIDSize[Node::PRIMARY] + CtrlIDSize[Node::SECONDARY];
        }

    case	STREAM_SECONDARY:
        switch (data[1]) {
        case	Node::PRIMARY:
            return	NULL;

        case	Node::SECONDARY:
            return	data + headerSize + DiscoveryIDSize + CtrlIDSize[Node::SECONDARY] + DataIDSize[Node::SECONDARY];

        case	Node::BOTH:
            return	data + headerSize + DiscoveryIDSize + CtrlIDSize[Node::PRIMARY] + DataIDSize[Node::PRIMARY] + StreamIDSize[Node::PRIMARY] + CtrlIDSize[Node::SECONDARY] + DataIDSize[Node::SECONDARY];
        }
    }

    return	NULL;
}
}
