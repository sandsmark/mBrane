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

#ifndef	mBrane_network_id_h
#define	mBrane_network_id_h

#include	"../Core/module_node.h"


using	namespace	mBrane::sdk::module;

namespace	mBrane
{

#define MBRANETOKEN 0xABCD

typedef	enum {
    CONTROL_PRIMARY = 0,
    DATA_PRIMARY = 1,
    STREAM_PRIMARY = 2,
    CONTROL_SECONDARY = 3,
    DATA_SECONDARY = 4,
    STREAM_SECONDARY = 5,
    DISCOVERY = 6
} InterfaceType;

//	Identifier of a node connection.
//	The total size depends on the network: headerSize+Size[PRIMARY]+Size[SECONDARY] or headerSize+Size[PRIMARY] or headerSize+Size[SECONDARY]
class	NetworkID
{
public:
    static	uint16_t DiscoveryIDSize;
    static	uint16_t Size;
    static	uint16_t CtrlIDSize[2];	//	1 for each network
    static	uint16_t DataIDSize[2];
    static	uint16_t StreamIDSize[2];
    uint8_t headerSize;	//	sizeof(NID)+sizeof(name size)+name size+sizeof(network)
    uint8_t *data;		//	[NID(8)|network(8)|name size(8)|name(name size*8)|discovery ID|control ID|data ID|stream ID|(control ID|data ID|stream ID) optional]
    NetworkID();
    NetworkID(uint8_t NID, mBrane::sdk::module::Node::Network	description, uint8_t nameSize, char	*name);
    ~NetworkID();
    uint8_t 							NID()	const;
    uint8_t 							setNID(uint8_t newNID);
    mBrane::sdk::module::Node::Network	network()	const;
    char								*name()	const;
    uint8_t *at(InterfaceType	t)		const;
};
}


#endif
