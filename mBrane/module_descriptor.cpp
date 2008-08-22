//	module_descriptor.cpp
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

#include	"module_descriptor.h"


#define	DC	0	//	Data and Control
#define	ST	1	//	Streams

namespace	mBrane{

	ModuleEntry::ModuleEntry(NodeEntry	*n,ModuleDescriptor	*m):Object<Memory,_Object,ModuleEntry>(),node(n),module(m){
	}

	ModuleEntry::~ModuleEntry(){

		if(module->activationCount)
			node->activationCount--;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////

	Array<Array<NodeEntry> >	NodeEntry::Main[2];

	CriticalSection	NodeEntry::CS[2];

	////////////////////////////////////////////////////////////////////////////////////////////////

	Array<Array<P<ModuleDescriptor> > >	ModuleDescriptor::Main;

	ModuleDescriptor::ModuleDescriptor(uint16	hostID,_Module	*m,uint16	CID,uint16	ID):Projectable<ModuleDescriptor>(),module(m),hostID(hostID){

		if(m){

			module->descriptor=this;
			module->_cid=CID;
			module->_id=ID;
			module->start();
		}
	}

	ModuleDescriptor::~ModuleDescriptor(){

		if(module!=NULL)
			module->stop();
	}

	void	ModuleDescriptor::addSubscription_message(uint16	spaceID,uint16	MCID){

		if(!projections[spaceID])
			project(spaceID);
		P<ModuleEntry>	p=new	ModuleEntry(NodeEntry::Main[DC][MCID].get(hostID),this);
		((P<Projection<ModuleDescriptor> >)projections[spaceID])->subscriptions[DC][MCID]=NodeEntry::Main[DC][MCID][hostID].modules.addElementTail(p);
	}

	void	ModuleDescriptor::addSubscription_stream(uint16	spaceID,uint16	SID){

		if(!projections[spaceID])
			project(spaceID);
		P<ModuleEntry>	p=new	ModuleEntry(NodeEntry::Main[ST][SID].get(hostID),this);
		((P<Projection<ModuleDescriptor> >)projections[spaceID])->subscriptions[ST][SID]=NodeEntry::Main[ST][SID][hostID].modules.addElementTail(p);
	}

	void	ModuleDescriptor::removeSubscription_message(uint16	spaceID,uint16	MCID){

		((P<ModuleEntry>)((P<Projection<ModuleDescriptor> >)projections[spaceID])->subscriptions[DC][MCID])=NULL;
		((P<Projection<ModuleDescriptor> >)projections[spaceID])->subscriptions[DC][MCID].remove();
	}

	void	ModuleDescriptor::removeSubscription_stream(uint16	spaceID,uint16	SID){

		((P<ModuleEntry>)((P<Projection<ModuleDescriptor> >)projections[spaceID])->subscriptions[ST][SID])=NULL;
		((P<Projection<ModuleDescriptor> >)projections[spaceID])->subscriptions[ST][SID].remove();
	}

	void	ModuleDescriptor::removeSubscriptions_message(uint16	spaceID){

		for(uint16	i=0;i<NodeEntry::Main[DC].count();i++)
			removeSubscription_message(spaceID,i);
	}

	void	ModuleDescriptor::removeSubscriptions_stream(uint16	spaceID){

		for(uint16	i=0;i<NodeEntry::Main[ST].count();i++)
			removeSubscription_stream(spaceID,i);
	}

	void	ModuleDescriptor::activate(){
	}

	void	ModuleDescriptor::deactivate(){
	}

	////////////////////////////////////////////////////////////////////////////////////////////////

	Projection<ModuleDescriptor>::Projection(ModuleDescriptor	*projected,Space	*space):_Projection<ModuleDescriptor,Projection<ModuleDescriptor> >(projected,space){
	}

	Projection<ModuleDescriptor>::~Projection(){
	}
	
	void	Projection<ModuleDescriptor>::activate(){

		for(uint32	i=0;i<subscriptions[DC].count();i++)
			((P<ModuleEntry>)subscriptions[DC][i])->node->activationCount++;
		for(uint32	i=0;i<subscriptions[ST].count();i++)
			((P<ModuleEntry>)subscriptions[ST][i])->node->activationCount++;
	}

	void	Projection<ModuleDescriptor>::deactivate(){

		for(uint32	i=0;i<subscriptions[DC].count();i++)
			((P<ModuleEntry>)subscriptions[DC][i])->node->activationCount--;
		for(uint32	i=0;i<subscriptions[ST].count();i++)
			((P<ModuleEntry>)subscriptions[ST][i])->node->activationCount--;
	}
}