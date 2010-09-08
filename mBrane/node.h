//	node.h
//
//	Author: Eric Nivel
//
//	BSD license:
//	Copyright (c) 2008, Eric Nivel
//	All rights reserved.
//	Redistribution and use in source and binary forms, with or without
//	modification, are permitted provided that the following conditions are met:
//
//   - Redistributions of source code must retain the above copyright
//     notice, this list of conditions and the following disclaimer.
//   - Redistributions in binary form must reproduce the above copyright
//     notice, this list of conditions and the following disclaimer in the
//     documentation and/or other materials provided with the distribution.
//   - Neither the name of Eric Nivel nor the
//     names of their contributors may be used to endorse or promote products
//     derived from this software without specific prior written permission.
//
//	THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND ANY
//	EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
//	WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
//	DISCLAIMED. IN NO EVENT SHALL THE REGENTS AND CONTRIBUTORS BE LIABLE FOR ANY
//	DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
//	(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
//	LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
//	ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//	(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
//	SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#ifndef	mBrane_node_h
#define	mBrane_node_h

#include	"../Core/payload.h"
#include	"../Core/list.h"

#include	"networking.h"
#include	"messaging.h"
#include	"executing.h"


using	namespace	mBrane::sdk;
using	namespace	mBrane::sdk::module;
using	namespace	mBrane::sdk::mdaemon;


namespace	mBrane{

	class	Node:
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
		Node	*loadConfig(const	char	*configFileName);
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
