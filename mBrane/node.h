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

#ifndef	mBrane_node_h
#define	mBrane_node_h

#include	"mBrane.h"
#include	"../Core/payload.h"
#include	"../Core/list.h"

#include	"networking.h"
#include	"messaging.h"
#include	"executing.h"


using	namespace	mBrane::sdk;
using	namespace	mBrane::sdk::module;
using	namespace	mBrane::sdk::mdaemon;


namespace	mBrane{

	class	mbrane_dll	Node:
	public	Networking,
	public	Executing{
	private:
		class	NoStream:
		public	std::ostream{
		public:
			NoStream():std::ostream(NULL){}
			template<typename	T>	NoStream&	operator	<<(T	&t){
				return	*this;
			}
		};
		//	CONFIG
		SharedLibrary	*userLibrary;
		Node	*loadConfigXML(const	char	*configXML);
		Node	*loadConfigFile(const	char	*configFileName);
		//	NETWORKING
		void	start(uint8_t assignedNID,NetworkID	*networkID,bool	isTimeReference);
		void	notifyNodeJoined(uint8_t NID,NetworkID	*networkID);
		void	notifyNodeLeft(uint8_t NID);
		//	MESSAGING
		void	startReceivingThreads(uint8_t NID);
		//	SHARED MEMORY
		//	Array<Array<uint8_t>,65535>	sharedMemorySegments;	//	FUTURE DEVELOPMENT
		//	NODE
	//	Array<Host::host_name,32>	nodeNames;
	//	Array<int32_t,32>	nodeStatus;
		Node(uint8_t traceLevels);
		bool	loadApplication(const	char	*fileName);
	public:
	//	uint8_t nodeCount;
	//	uint8_t nodeJoined;
	//	uint8_t getNID(const	char	*name);
	//	bool allNodesJoined();
	//	bool allNodesReady();
		const	char	*name();
		//	main() NODE API
		static	Node	*New(const	char	*configFileName,SharedLibrary	&userLibrary,uint8_t traceLevels);
		static	Node	*NewXML(const	char	*configXML,SharedLibrary	&userLibrary,uint8_t traceLevels);
		~Node();
		void	run();
		void	ready();
		//void	systemReady();
		void	shutdown();
		//	MODULE NODE API
		void	send(const	_Module	*sender,_Payload	*message,module::Node::Network	network=module::Node::PRIMARY);
		void	send(const	_Module	*sender,_Payload	*message, uint8_t nodeID,Network	network=module::Node::PRIMARY);
        void	send(const	_Module	*sender,_Payload	*message, Array<uint8_t, 128>	*nodeIDs,Network	network);
		uint64_t time()	const;
		void	newSpace(const	_Module	*sender,Network	network=PRIMARY);
		void	newModule(const	_Module	*sender,uint16_t CID,Network	network=PRIMARY,const	char	*hostName="local");
		void	deleteSpace(uint16_t ID,Network	network=PRIMARY);
		void	deleteModule(uint16_t CID,uint16_t ID,Network	network=PRIMARY);
		void	activateModule(const	_Module	*sender,uint16_t module_cid,uint16_t module_id,uint16_t space_id,float	activationLevel,Network	network=PRIMARY);
		void	activateSpace(const	_Module	*sender,uint16_t space_id,uint16_t target_sid,float	activationLevel,Network	network=PRIMARY);
		void	setSpaceThreshold(const	_Module	*sender,uint16_t space_id,float	threshold,Network	network=PRIMARY);
		void	subscribeMessage(const	_Module	*sender,uint16_t module_cid,uint16_t module_id,uint16_t space_id,uint16_t message_cid,Network	network=PRIMARY);
		void	unsubscribeMessage(const	_Module	*sender,uint16_t module_cid,uint16_t module_id,uint16_t space_id,uint16_t message_cid,Network	network=PRIMARY);
		void	subscribeStream(const	_Module	*sender,uint16_t module_cid,uint16_t module_id,uint16_t space_id,uint16_t stream_id,Network	network=PRIMARY);
		void	unsubscribeStream(const	_Module	*sender,uint16_t module_cid,uint16_t module_id,uint16_t space_id,uint16_t stream_id,Network	network=PRIMARY);
		const	char	*getSpaceName(uint16_t hostID,uint16_t ID);
		const	char	*getModuleName(uint16_t CID);
		void		markUnused(_Payload	*p);
		void		addConstantObject(_Payload	*c,const	std::string	&name);
		_Payload	*getConstantObject(uint32_t OID);
		_Payload	*getConstantObject(const	std::string	&name);	//	usage: call once and retain a pointer to the result.
		void		addLookup(uint8_t sourceNID,uint32_t OID);
		bool		hasLookup(uint8_t destinationNID,uint32_t OID);
		void		addSharedObject(_Payload	*o);
		_Payload	*getSharedObject(uint32_t OID);
		void		consolidate(_Payload	*p);
		//	DAEMON NODE API
		void			dump(const	char	*fileName);
		void			load(const	char	*fileName);
		void			migrate(uint16_t CID,uint16_t ID,uint8_t NID);
		//Array<uint8_t,65535>	&sharedMemorySegment(uint8_t segment);	//	FUTURE DEVELOPMENT
		_Module			*getModule(uint8_t hostID,uint16_t CID,uint16_t ID);
	};
}


#endif
