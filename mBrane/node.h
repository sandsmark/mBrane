//	node.h
//
//	Author: Eric Nivel
//
//	BSD license:
//	Copyright (c) 2008, Eric Nivel, Thor List
//	All rights reserved.
//	Redistribution and use in source and binary forms, with or without
//	modification, are permitted provided that the following conditions are met:
//
//   - Redistributions of source code must retain the above copyright
//     notice, this list of conditions and the following disclaimer.
//   - Redistributions in binary form must reproduce the above copyright
//     notice, this list of conditions and the following disclaimer in the
//     documentation and/or other materials provided with the distribution.
//   - Neither the name of Eric Nivel, Thor List nor the
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

#include	"..\Core\message.h"
#include	"..\Core\list.h"

#include	"networking.h"
#include	"messaging.h"
#include	"unordered_messaging_engine.h"
#include	"ordered_messaging_engine.h"
#include	"executing.h"


using	namespace	mBrane::sdk;
using	namespace	mBrane::sdk::module;
using	namespace	mBrane::sdk::daemon;

#if	defined	ORDERED_MESSAGING_ENGINE
	#define	MESSAGING_CLASS	Messaging<OrderedMessagingEngine>
#elif	defined	UNORDERED_MESSAGING_ENGINE
	#define	MESSAGING_CLASS	Messaging<UnorderedMessagingEngine>
#endif

namespace	mBrane{

	class	Node:
	public	Networking,
	public	MESSAGING_CLASS,
	public	Executing{
	template<class	Engine>	friend	class	Messaging;
	private:
		//	CONFIG
		SharedLibrary	*userLibrary;
		Node	*loadConfig(const	char	*configFileName);
		//	NETWORKING
		void	start(uint16	assignedNID,NetworkID	*networkID,bool	isTimeReference);
		void	notifyNodeJoined(uint16	NID,NetworkID	*networkID);
		void	notifyNodeLeft(uint16	NID);
		//	MESSAGING
		void	startReceivingThreads(uint16	NID);
		//	SHARED MEMORY
		Array<Array<uint8> >	sharedMemorySegments;
		//	NODE
		uint16	nodeCount;
		Array<const	char	*>	nodeNames;
		uint16	getNIDFromName(const	char	*name);
		Node();
		bool	loadApplication(const	char	*fileName);
		void	unloadApplication();
	public:
		//	main() NODE API
		static	Node	*New(const	char	*configFileName);
		~Node();
		void	run();
		void	shutdown();
		//	MODULE NODE API
		void	send(const	_Module	*sender,_Payload	*message,module::Node::Network	network=module::Node::PRIMARY);
		int64	time()	const;
		void	newSpace(const	_Module	*sender,Network	network=PRIMARY);
		void	newModule(const	_Module	*sender,uint16	CID,Network	network=PRIMARY,const	char	*hostName=NULL);
		void	deleteSpace(uint16	ID,Network	network=PRIMARY);
		void	deleteModule(uint16	CID,uint16	ID,Network	network=PRIMARY);
		const	char	*getSpaceName(uint16	ID);
		const	char	*getModuleName(uint16	ID);
		//	DAEMON NODE API
		void			dump(const	char	*fileName);
		void			load(const	char	*fileName);
		void			migrate(uint16	CID,uint16	ID,uint16	NID);
		Array<uint8>	&sharedMemorySegment(uint8	segment);
		_Module			*getModule(uint16	CID,uint16	ID);
	};
}


#endif
