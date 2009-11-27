//	module_descriptor.h
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

#ifndef	mBrane_module_descriptor_h
#define	mBrane_module_descriptor_h

#include	"../Core/list.h"
#include	"../Core/module.h"
#include	"../Core/xml_parser.h"

#include	"projection.h"


using	namespace	mBrane::sdk;
using	namespace	mBrane::sdk::module;

namespace	mBrane{

	class	NodeEntry;
	class	ModuleEntry:
	public	Object<Memory,_Object,ModuleEntry>{
	public:
		NodeEntry			*node;
		ModuleDescriptor	*descriptor;
		ModuleEntry(NodeEntry	*n,ModuleDescriptor	*m);
		~ModuleEntry();
	};

	class	NodeEntry:
	public	CriticalSection{	//	guards against Messaging::SendMessages.
	private:
		uint32	activationCount;
	public:
		static	Array<Array<NodeEntry,32>,128>	Main[2];	//	0: Data and Control: message class -> nodes -> modules, 1: Streams: stream id -> nodes -> modules
		NodeEntry();
		~NodeEntry();
		void	incActivation(){	enter();activationCount++;leave();	}
		void	decActivation(){	enter();activationCount--;leave();	}
		void	getActivation(uint32	&a){	enter();a=activationCount;leave();	}
		List<P<ModuleEntry>,1024>	modules;
	};

	template<>	class	Projection<ModuleDescriptor>:
	public	_Projection<ModuleDescriptor,Projection<ModuleDescriptor> >{
	public:
		Array<List<P<ModuleEntry>,1024>::Iterator,128>	subscriptions[2];		//	0: indexed by message class ID (MCID), 1: indexed by stream ID (SID)
		uint16														subscriptionCount[2];	//	idem
		Projection(ModuleDescriptor	*projected,Space				*space);
		~Projection();
		void	activate();
		void	deactivate();
		void	setActivationLevel(float32	a);
		void	updateActivationCount(float32	t);
		void	addSubscription(uint8	payloadType,uint16	ID,List<P<ModuleEntry>,1024>::Iterator	i);
		void	removeSubscription(uint8	payloadType,uint16	ID);
	};

	//	Module proxy.
	class	ModuleDescriptor:
	public	Projectable<ModuleDescriptor>{
	private:
		class	_Subscription{
		public:
			uint16	MCID;
			uint16	SID;
		};
		class	_Projection{
		public:
			uint16					spaceID;
			float32					activationLevel;
			Array<_Subscription,8>	subscriptions;
		};
		Array<_Projection,32>	initialProjections;
		const	char	*name;
	public:
		uint16	CID;
		static	Array<Array<P<ModuleDescriptor>,128>,32>			Config;	//	indexed by module descriptor class ID | ID; temporary: used at config time when node IDs are not known; tranfered in Node::start in Main.
		static	Array<Array<Array<P<ModuleDescriptor>,128>,32>,8>	Main;	//	indexed by host ID | module descriptor class ID | ID.
		static	ModuleDescriptor									*New(XMLNode	&n);
		static	void												Init(uint16	hostID);	//	resolves host name into ID, copies Config in Main, apply initial projections.
		static	uint16												GetID(uint16	hostID,uint16	CID);	//	returns the first available slot in Main[hostID][CID].
		Host::host_name	hostName;	//	resolved in hostID at Node::run() time
		P<_Module>	module;	//	NULL if remote
		//_Module	*module;
		ModuleDescriptor(const	char	*hostName,_Module	*m,uint16	CID,const	char	*name);	//	invoked at Node::loadApplication() time.
		ModuleDescriptor(uint16	hostID,uint16	CID,uint16	ID);									//	invoked dynamically.
		~ModuleDescriptor();
		void	applyInitialProjections(uint16	hostID);
		const	char	*getName();
		void	_activate();
		void	_deactivate();
		void	addSubscription_message(uint16	hostID,uint16	spaceID,uint16	MCID);
		void	addSubscription_stream(uint16	hostID,uint16	spaceID,uint16	SID);
		void	removeSubscription_message(uint16	hostID,uint16	spaceID,uint16	MCID);
		void	removeSubscription_stream(uint16	hostID,uint16	spaceID,uint16	SID);
		void	removeSubscriptions_message(uint16	hostID,uint16	spaceID);
		void	removeSubscriptions_stream(uint16	hostID,uint16	spaceID);
	};
}


#endif
