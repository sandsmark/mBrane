//	module_descriptor.h
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

#ifndef	mBrane_module_descriptor_h
#define	mBrane_module_descriptor_h

#include	"..\Core\list.h"
#include	"..\Core\module.h"

#include	"projection.h"


using	namespace	mBrane::sdk;
using	namespace	mBrane::sdk::module;

namespace	mBrane{

	class	NodeEntry;
	class	ModuleEntry:
	public	Object<Memory,_Object,ModuleEntry>{
	public:
		NodeEntry			*node;
		ModuleDescriptor	*module;
		ModuleEntry(NodeEntry	*n,ModuleDescriptor	*m);
		~ModuleEntry();
	};

	class	NodeEntry{
	public:
		static	Array<Array<NodeEntry> >	Main[2];	//	0: Data and Control: message class -> nodes -> modules, 2: Streams: stream id -> nodes -> modules
		static	CriticalSection				CS[2];
		uint32					activationCount;
		List<P<ModuleEntry> >	modules;
	};

	template<>	class	Projection<ModuleDescriptor>:
	public	Object<Memory,_Object,Projection<ModuleDescriptor> >{
	private:
		ModuleDescriptor	*projected;
		Space				*space;
		float32				activationLevel;
		void				propagateActivation();
		void				propagateDeactivation();
	public:
		Projection(ModuleDescriptor	*projected,Space	*space);
		~Projection();
		void	setActivationLevel(float32	a);
		void	updateActivationCount(float32	t);
		Array<typename	List<P<ModuleEntry> >::Iterator>	subscriptions[2];	//	0: indexed by message class ID (MCID), 1: indexed by stream ID (SID)
	};

	class	ModuleDescriptor:
	public	Projectable<ModuleDescriptor>{
	public:
		static	Array<Array<P<ModuleDescriptor> > >	Main;	//	indexed by module descriptor class ID | ID
		uint16	hostID;	//	node
		P<_Module>	module;	//	NULL if remote
		ModuleDescriptor(uint16	hostID,_Module	*m=NULL);
		~ModuleDescriptor();
		void	addSubscription_message(uint16	spaceID,uint16	MCID);
		void	addSubscription_stream(uint16	spaceID,uint16	SID);
		void	removeSubscription_message(uint16	spaceID,uint16	MCID);
		void	removeSubscription_stream(uint16	spaceID,uint16	SID);
	};
}


#endif