//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
//_/_/
//_/_/ HUMANOBS - mBrane
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
		void	start(uint8	assignedNID,NetworkID	*networkID,bool	isTimeReference);
		void	notifyNodeJoined(uint8	NID,NetworkID	*networkID);
		void	notifyNodeLeft(uint8	NID);
		//	MESSAGING
		void	startReceivingThreads(uint8	NID);
		//	SHARED MEMORY
		//	Array<Array<uint8>,65535>	sharedMemorySegments;	//	FUTURE DEVELOPMENT
		//	NODE
	//	Array<Host::host_name,32>	nodeNames;
	//	Array<int32,32>	nodeStatus;
		Node(uint8	traceLevels);
		bool	loadApplication(const	char	*fileName);
	public:
	//	uint8	nodeCount;
	//	uint8	nodeJoined;
	//	uint8	getNID(const	char	*name);
	//	bool allNodesJoined();
	//	bool allNodesReady();
		const	char	*name();
		//	main() NODE API
		static	Node	*New(const	char	*configFileName,SharedLibrary	&userLibrary,uint8	traceLevels);
		static	Node	*NewXML(const	char	*configXML,SharedLibrary	&userLibrary,uint8	traceLevels);
		~Node();
		void	run();
		void	ready();
		//void	systemReady();
		void	shutdown();
		//	MODULE NODE API
		void	send(const	_Module	*sender,_Payload	*message,module::Node::Network	network=module::Node::PRIMARY);
		void	send(const	_Module	*sender,_Payload	*message, uint8	nodeID,Network	network=module::Node::PRIMARY);
		void	send(const	_Module	*sender,_Payload	*message, Array<uint8, 128>	*nodeIDs,Network	network);
		uint64	time()	const;
		void	newSpace(const	_Module	*sender,Network	network=PRIMARY);
		void	newModule(const	_Module	*sender,uint16	CID,Network	network=PRIMARY,const	char	*hostName="local");
		void	deleteSpace(uint16	ID,Network	network=PRIMARY);
		void	deleteModule(uint16	CID,uint16	ID,Network	network=PRIMARY);
		void	activateModule(const	_Module	*sender,uint16	module_cid,uint16	module_id,uint16	space_id,float32	activationLevel,Network	network=PRIMARY);
		void	activateSpace(const	_Module	*sender,uint16	space_id,uint16	target_sid,float32	activationLevel,Network	network=PRIMARY);
		void	setSpaceThreshold(const	_Module	*sender,uint16	space_id,float32	threshold,Network	network=PRIMARY);
		void	subscribeMessage(const	_Module	*sender,uint16	module_cid,uint16	module_id,uint16	space_id,uint16	message_cid,Network	network=PRIMARY);
		void	unsubscribeMessage(const	_Module	*sender,uint16	module_cid,uint16	module_id,uint16	space_id,uint16	message_cid,Network	network=PRIMARY);
		void	subscribeStream(const	_Module	*sender,uint16	module_cid,uint16	module_id,uint16	space_id,uint16	stream_id,Network	network=PRIMARY);
		void	unsubscribeStream(const	_Module	*sender,uint16	module_cid,uint16	module_id,uint16	space_id,uint16	stream_id,Network	network=PRIMARY);
		const	char	*getSpaceName(uint16	hostID,uint16	ID);
		const	char	*getModuleName(uint16	CID);
		void		markUnused(_Payload	*p);
		void		addConstantObject(_Payload	*c,const	std::string	&name);
		_Payload	*getConstantObject(uint32	OID);
		_Payload	*getConstantObject(const	std::string	&name);	//	usage: call once and retain a pointer to the result.
		void		addLookup(uint8	sourceNID,uint32	OID);
		bool		hasLookup(uint8	destinationNID,uint32	OID);
		void		addSharedObject(_Payload	*o);
		_Payload	*getSharedObject(uint32	OID);
		void		consolidate(_Payload	*p);
		//	DAEMON NODE API
		void			dump(const	char	*fileName);
		void			load(const	char	*fileName);
		void			migrate(uint16	CID,uint16	ID,uint8	NID);
		//Array<uint8,65535>	&sharedMemorySegment(uint8	segment);	//	FUTURE DEVELOPMENT
		_Module			*getModule(uint8	hostID,uint16	CID,uint16	ID);
	};
}


#endif
