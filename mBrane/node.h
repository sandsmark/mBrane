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
#include	"publishing_subscribing.h"
#include	"executing.h"


using	namespace	mBrane::sdk;
using	namespace	mBrane::sdk::crank;
using	namespace	mBrane::sdk::daemon;

namespace	mBrane{

	class	Node:
	public	Networking,
	public	Messaging,
	public	PublishingSubscribing,
	public	Executing{
	private:
		//	CONFIG
		const	char	*application_configuration_file;
		SharedLibrary	*userLibrary;
		Node	*loadConfig(const	char	*configFileName);

		//	NETWORKING
		typedef	struct{
			Node					*n;
			CommChannel				*c;
			uint16					e;
			NetworkInterfaceType	t;
		}ReceiveThreadArgs;
		static	uint32	thread_function_call	ReceiveMessages(void	*args);
		static	uint32	thread_function_call	SendMessages(void	*args);
		static	uint32	thread_function_call	NotifyMessages(void	*args);
		void	start(uint16	assignedNID,NetworkID	*networkID,bool	isTimeReference);
		void	startReceivingThreads(uint16	NID);
		void	notifyNodeJoined(uint16	NID,NetworkID	*networkID);
		void	notifyNodeLeft(uint16	NID);

		//	NODE
		uint16	nodeCount;
		Array<const	char	*>	nodeNames;
		Node();
	public:
		static	Node	*New(const	char	*configFileName);
		~Node();
		void	run();
		void	shutdown();
		void	dump(const	char	*fileName);	//	dumps the current system state; crank dump fileNames: crank_class_ID.bin: ex: CR1_123.bin
		void	load(const	char	*fileName);	//	initializes itself from a previously saved system state
		Node	*loadApplication(const	char	*fileName=NULL);	//	return NULL if unsuccessful; fileName overrides the fileName found in the node config file
		void	unloadApplication();
		int64	time()	const;	//	in ms since midnight 01/01/70
		void	send(const	_Crank	*sender,_Payload	*message);
		_Crank	*buildCrank(uint16	CID);
		void	start(_Crank	*c);
		void	stop(_Crank	*c);
		void	migrate(_Crank	*c,uint16	NID);
	};
}


#endif
