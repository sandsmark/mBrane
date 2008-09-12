//	module_descriptor.cpp
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

#include	"..\Core\module_register.h"
#include	"..\Core\class_register.h"

#include	"module_descriptor.h"
#include	"node.h"

#include	<iostream>


#define	DC	0	//	Data and Control
#define	ST	1	//	Streams

#define	INITIAL_LIST_SIZE	32

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

	NodeEntry::NodeEntry(){

		modules.alloc(INITIAL_LIST_SIZE);
	}

	NodeEntry::~NodeEntry(){
	}

	////////////////////////////////////////////////////////////////////////////////////////////////

	Array<Array<P<ModuleDescriptor> > >	ModuleDescriptor::Main;

	ModuleDescriptor	*ModuleDescriptor::New(XMLNode	&n){

		const	char	*_class=n.getAttribute("class");
		if(!_class){

			std::cout<<"Error: Module::class is missing\n";
			return	NULL;
		}

		uint16	CID=ModuleRegister::GetCID(_class);
		if(CID==ClassRegister::NoClass){

			std::cout<<"Error: class: "<<_class<<" does not exists\n";
			return	NULL;
		}

		const	char	*name=n.getAttribute("name");

		const	char	*_host=n.getAttribute("host");
		if(!_host){

			std::cout<<"Error: Module::host is missing\n";
			return	NULL;
		}

		uint16	hostID=((mBrane::Node	*)Node::Get())->getNID(_host);
		_Module	*_m=NULL;
		if(strcmp(_host,"local")==0)
			_m=ModuleRegister::Get(CID)->buildModule();

		ModuleDescriptor	*m=new	ModuleDescriptor(hostID,_m,CID,name);

		uint16	projectionCount=n.nChildNode("Projection");
		if(!projectionCount)
			m->setActivationLevel(0,1);
		else	for(uint16	i=0;i<projectionCount;i++){

			XMLNode	projection=n.getChildNode("Projection",i);
			const	char	*spaceName=projection.getAttribute("space");	//	to be projected on
			if(!spaceName){

				std::cout<<"Error: Module: "<<name<<" ::Projection::name is Missing\n";
				goto	error;
			}
			const	char	*_activationLevel=projection.getAttribute("activation_level");
			if(!_activationLevel){

				std::cout<<"Error: Module: "<<name<<" ::Projection::activation_level is Missing\n";
				goto	error;
			}
			Space	*_s=Space::Get(spaceName);
			if(!_s){

				std::cout<<"Error: Space "<<spaceName<<" does not exist\n";
				goto	error;
			}
			m->setActivationLevel(_s->ID,atoi(_activationLevel));
			uint16	subscriptionCount=projection.nChildNode("Subscription");
			for(uint16	i=0;i<subscriptionCount;i++){

				XMLNode	subscription=n.getChildNode("Subscription",i);
				const	char	*_messageClass=subscription.getAttribute("message_class");
				const	char	*_stream=subscription.getAttribute("stream");
				if(!_messageClass	&&	!_stream){

					std::cout<<"Error: Module::"<<name<<"Projection::"<<spaceName<<"Subscription: neither message_class nor stream are specified\n";
					goto	error;
				}
				if(_messageClass){

					uint16	MCID=ClassRegister::GetCID(_messageClass);
					if(MCID==ClassRegister::NoClass){

						std::cout<<"Error: Module::"<<name<<"Projection::"<<spaceName<<"Subscription::message_class: "<<_messageClass<<" does not exist\n";
						goto	error;
					}
					m->addSubscription_message(_s->ID,MCID);
				}
				if(_stream)
					m->addSubscription_stream(_s->ID,atoi(_stream));
			}
		}

		return	m;
error:	delete	m;
		return	NULL;
	}

	ModuleDescriptor::ModuleDescriptor(uint16	hostID,_Module	*m,uint16	CID,const	char	*name):Projectable<ModuleDescriptor>(ModuleDescriptor::Main[CID].count()),module(m),hostID(hostID),CID(CID){

		if(m){

			module->descriptor=this;
			module->_cid=CID;
			module->_id=ID;
			module->start();
		}

		if(name){

			this->name=new	char[strlen(name)];
			memcpy((void	*)this->name,name,strlen(name));
		}
	}

	const	char	*ModuleDescriptor::getName(){

		return	name;
	}

	ModuleDescriptor::~ModuleDescriptor(){

		if(ModuleDescriptor::Main[CID][ID]==NULL)
			return;
		if(module!=NULL)
			module->stop();
		ModuleDescriptor::Main[CID][ID]=NULL;
		delete[]	name;
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