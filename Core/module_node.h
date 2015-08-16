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

#ifndef	mBrane_sdk_module_node_h
#define	mBrane_sdk_module_node_h

#include	"payload.h"
#include	<iostream>

namespace	mBrane
{
namespace	sdk
{

namespace	module
{

class	_Module;

//	Node API, as seen from the module
class	mBrane_dll	Node
{
private:
    static	Node	*Singleton;
protected:
    static	std::ostream	*Streams[3];
    uint8_t	_ID;	//	max: 0x7F
    Node(uint8_t	ID);
    ~Node();
public:
    static	const	uint8_t	NoID = 0xFF;
    typedef	enum {
        PRIMARY = 0,
        SECONDARY = 1,
        BOTH = 2,
        LOCAL = 3,
        EITHER = 4
    } Network;
    typedef	enum {
        EXECUTION = 0,
        NETWORK = 1,
        APPLICATION = 2
    } TraceLevel;
    static	Node	*Get();
    static	std::ostream	&trace(TraceLevel	l);
    uint8_t	id()	const;
    virtual	const	char	*name() = 0;
    virtual	void	send(const	_Module	*sender, _Payload	*p, Network	network = PRIMARY) = 0;
    virtual	void	send(const	_Module	*sender, _Payload	*message, uint8_t	nodeID, Network	network = PRIMARY) = 0;
    virtual	uint64_t	time()	const = 0;	//	in us since 01/01/70
    virtual	void	newSpace(const	_Module	*sender, Network	network = PRIMARY) = 0;	//	names are meaningless for dynamic instances
    virtual	void	newModule(const	_Module	*sender, uint16_t	CID, Network	network = PRIMARY, const	char	*hostName = "local") = 0;
    virtual	void	deleteSpace(uint16_t	ID, Network	network = PRIMARY) = 0;
    virtual	void	deleteModule(uint16_t	CID, uint16_t	ID, Network	network = PRIMARY) = 0;
    virtual	void	activateModule(const	_Module	*sender, uint16_t	module_cid, uint16_t	module_id, uint16_t	space_id, float	activationLevel, Network	network = PRIMARY) = 0;
    virtual	void	activateSpace(const	_Module	*sender, uint16_t	space_id, uint16_t	target_sid, float	activationLevel, Network	network = PRIMARY) = 0;
    virtual	void	setSpaceThreshold(const	_Module	*sender, uint16_t	space_id, float	threshold, Network	network = PRIMARY) = 0;
    virtual	void	subscribeMessage(const	_Module	*sender, uint16_t	module_cid, uint16_t	module_id, uint16_t	space_id, uint16_t	message_cid, Network	network = PRIMARY) = 0;
    virtual	void	unsubscribeMessage(const	_Module	*sender, uint16_t	module_cid, uint16_t	module_id, uint16_t	space_id, uint16_t	message_cid, Network	network = PRIMARY) = 0;
    virtual	void	subscribeStream(const	_Module	*sender, uint16_t	module_cid, uint16_t	module_id, uint16_t	space_id, uint16_t	stream_id, Network	network = PRIMARY) = 0;
    virtual	void	unsubscribeStream(const	_Module	*sender, uint16_t	module_cid, uint16_t	module_id, uint16_t	space_id, uint16_t	stream_id, Network	network = PRIMARY) = 0;
    virtual	const	char	*getSpaceName(uint16_t	hostID, uint16_t	ID) = 0;
    virtual	const	char	*getModuleName(uint16_t	CID) = 0;
    virtual uint8_t	getNodeID(const char *name) = 0;
    //	Interface for caching.
    virtual	void		markUnused(_Payload	*p) = 0;
    virtual	void		addConstantObject(_Payload	*c, const	std::string	&name) = 0;
    virtual	_Payload	*getConstantObject(uint32_t	OID) = 0;
    virtual	_Payload	*getConstantObject(const	std::string	&name) = 0;
    virtual	void		addLookup(uint8_t	sourceNID, uint32_t	OID) = 0;
    virtual	bool		hasLookup(uint8_t	destinationNID, uint32_t	OID) = 0;
    virtual	void		addSharedObject(_Payload	*p) = 0;
    virtual	_Payload	*getSharedObject(uint32_t	OID) = 0;
    virtual	void		consolidate(_Payload	*p) = 0;
};
}
}
}


#endif
