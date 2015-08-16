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

#include	"module_register.h"
#include	"class_register.h"

#include	"module_descriptor.h"
#include	"projection.tpl.cpp"
#include	"node.h"

#include	<iostream>
#include	<memory>


#define	DC	0	//	Data and Control
#define	ST	1	//	Streams

namespace	mBrane{

	ModuleEntry::ModuleEntry(NodeEntry	*n,ModuleDescriptor	*m):Object<Memory,_Object,ModuleEntry>(),node(n),descriptor(m){
	}

	ModuleEntry::~ModuleEntry(){

		if(descriptor->activationCount)
			node->decActivation();
	}

	////////////////////////////////////////////////////////////////////////////////////////////////

	Array<Array<NodeEntry,32>,128>	NodeEntry::Main[2];

	NodeEntry::NodeEntry(){
	}

	NodeEntry::~NodeEntry(){
	}

	////////////////////////////////////////////////////////////////////////////////////////////////

	Array<Array<P<ModuleDescriptor>,128>,32>			ModuleDescriptor::Config;

	Array<Array<Array<P<ModuleDescriptor>,128>,32>,8>	ModuleDescriptor::Main;

	ModuleDescriptor	*ModuleDescriptor::New(XMLNode	&n){

		const	char	*_class=n.getAttribute("class");
		if(!_class){

			std::cout<<"> Error: Module::class is missing"<<std::endl;
			return	NULL;
		}

		uint16_t	CID=ModuleRegister::GetCID(_class);
		if(CID==ClassRegister::NoClass){

			std::cout<<"> Error: class: "<<_class<<" does not exist"<<std::endl;
			return	NULL;
		}

		const	char	*name=n.getAttribute("name");

		const	char	*_host=n.getAttribute("host");
		if(!_host){

			std::cout<<"> Error: Module::host is missing"<<std::endl;
			return	NULL;
		}

		_Module	*_m=NULL;
		if(	stricmp(_host,Node::Get()->name())==0	||
			stricmp(_host,"local")==0){

			_m=ModuleRegister::Get(CID)->buildModule();

			XMLNode	parameters=n.getChildNode("Parameters");
			if(!!parameters){

				uint32_t						_parameterCount=parameters.nChildNode();
				std::vector<int32_t>			numerical_args;
				std::vector<std::string>	string_args;
				for(uint32_t	i=0;i<_parameterCount;++i){

					XMLNode	p=parameters.getChildNode("Parameter",i);
					const	char	*_type=p.getAttribute("type");
					const	char	*_value=p.getAttribute("value");
					if(strcmp(_type,"float")==0){

						float	value=(float)atof(_value);
						numerical_args.push_back(*reinterpret_cast<int32_t	*>(&value));
					}else	if(strcmp(_type,"int32")==0)
						numerical_args.push_back(atoi(_value));
					else	if(strcmp(_type,"string")==0)
						string_args.push_back(std::string(_value));
					else{

						std::cout<<"> Error: module "<<name<<": unrecognized parameter type"<<std::endl;
						return	NULL;
					}
				}
				_m->loadParameters(numerical_args,string_args);
			}
		}

		uint16_t	ID=(uint16_t)ModuleDescriptor::Config[CID].count();
		ModuleDescriptor	*m=new	ModuleDescriptor(_host,_m,CID,name);
		ModuleDescriptor::Config[CID][ID]=m;

		uint16_t	projectionCount=n.nChildNode("Projection");
		if(!projectionCount){	//	when no projection is defined, the module is projected on root at the highest activation level.

			m->initialProjections[0].spaceID=0;
			m->initialProjections[0].activationLevel=1.0;
		}else{
			
			for(uint16_t	i=0;i<projectionCount;i++){

				XMLNode	projection=n.getChildNode("Projection",i);
				const	char	*spaceName=projection.getAttribute("space");	//	to be projected on.
				if(!spaceName){

					std::cout<<"> Error: Module: "<<name<<" ::Projection::name is missing"<<std::endl;
					goto	error;
				}
				const	char	*_activationLevel=projection.getAttribute("activation_level");
				if(!_activationLevel){

					std::cout<<"> Error: Module: "<<name<<" ::Projection::activation_level is missing"<<std::endl;
					goto	error;
				}
				Space	*_s=Space::Get(spaceName);
				if(!_s){

					std::cout<<"> Error: Space "<<spaceName<<" does not exist"<<std::endl;
					goto	error;
				}
				m->initialProjections[i].spaceID=_s->ID;
				m->initialProjections[i].activationLevel=(float)atof(_activationLevel);
				uint16_t	subscriptionCount=projection.nChildNode("Subscription");
				for(uint16_t	j=0;j<subscriptionCount;j++){

					XMLNode	subscription=projection.getChildNode("Subscription",j);
					const	char	*_messageClass=subscription.getAttribute("message_class");
					const	char	*_stream=subscription.getAttribute("stream");
					if(!_messageClass	&&	!_stream){

						std::cout<<"> Error: Module::"<<name<<" Projection::"<<spaceName<<" Subscription: neither message_class nor stream are specified"<<std::endl;
						goto	error;
					}
					if(_messageClass){

						uint16_t	MCID=ClassRegister::GetCID(_messageClass);
						if(MCID==ClassRegister::NoClass){

							std::cout<<"> Error: Module::"<<name<<" Projection::"<<spaceName<<" Subscription::message_class: "<<_messageClass<<" does not exist"<<std::endl;
							goto	error;
						}
						m->initialProjections[i].subscriptions[j].MCID=MCID;
						m->initialProjections[i].subscriptions[j].SID=0xFFFF;
					}
					if(_stream){

						m->initialProjections[i].subscriptions[j].MCID=ClassRegister::NoClass;
						m->initialProjections[i].subscriptions[j].SID=atoi(_stream);
					}
				}
			}
		}

		return	m;
error:	ModuleDescriptor::Config[CID][m->ID]=NULL;
		return	NULL;
	}

	void	ModuleDescriptor::Init(uint8_t	hostID){

		ModuleDescriptor	*md;
		for(uint32_t	i=0;i<ModuleDescriptor::Config.count();i++)	//	resolve host names into NID
			for(uint32_t	j=0;j<ModuleDescriptor::Config[i].count();j++){
				
				Node::Get()->trace(Node::EXECUTION)<<"> Info: Module init '"<<ModuleDescriptor::Config[i][j]->getName()<<"' for host '"<<ModuleDescriptor::Config[i][j]->hostName<<"'"<<std::endl;
				if(	stricmp(ModuleDescriptor::Config[i][j]->hostName,Node::Get()->name())==0	||
					stricmp(ModuleDescriptor::Config[i][j]->hostName,"local")==0){

					md=ModuleDescriptor::Config[i][j];
					ModuleDescriptor::Main[hostID][i][j]=md;
					md->hostID=hostID;
					md->applyInitialProjections(hostID);
					if(md->module!=NULL)
						md->module->_start();

					ModuleDescriptor::Config[i][j]=NULL;
				}
			}
	}

	uint16_t	ModuleDescriptor::GetID(uint8_t	hostID,uint16_t	CID){

		for(uint16_t	i=0;i<Main[hostID][CID].count();i++)
			if(Main[hostID][CID][i]==NULL)
				return	i;
		return	(uint16_t)Main[hostID][CID].count();
	}

	char noname[1];
	const char*	ModuleDescriptor::GetName(uint16_t	cid, uint16_t id){
		noname[0] = 0;
		ModuleDescriptor	*m=ModuleDescriptor::Config[cid][id];
		if (m == NULL)
			return noname;
		else
			return m->getName();
	}

	ModuleDescriptor::ModuleDescriptor(const	char	*hostName,_Module	*m,uint16_t	CID,const	char	*name):Projectable<ModuleDescriptor>(module::Node::NoID,(uint16_t)ModuleDescriptor::Config[CID].count()),module(m),CID(CID){

		if(m){

			module->descriptor=this;
			module->_cid=CID;
			module->_id=ID;
		}

		strcpy(this->hostName,hostName);

		if(name){

			this->name=new	char[strlen(name)+1];
			memcpy((void	*)this->name,name,strlen(name)+1);
			Node::Get()->trace(Node::EXECUTION)<<"> Info: Module "<<CID<<"|"<<ID<<" ("<<name<<") created"<<std::endl;
		}
		else
			Node::Get()->trace(Node::EXECUTION)<<"> Info: Module "<<CID<<"|"<<ID<<" created"<<std::endl;
	}

	ModuleDescriptor::ModuleDescriptor(uint8_t	hostID,uint16_t	CID,uint16_t	ID):Projectable<ModuleDescriptor>(hostID,ID),module(NULL),CID(CID),name(NULL){

		if(hostID==Node::Get()->id()){

			module=ModuleRegister::Get(CID)->buildModule();
			module->descriptor=this;
			module->_cid=CID;
			module->_id=ID;
			Node::Get()->trace(Node::EXECUTION)<<"> Info: Module "<<CID<<"|"<<ID<<" created"<<std::endl;
			module->_start();
		}
	}

	ModuleDescriptor::~ModuleDescriptor(){

		if(module!=NULL)
			module->_stop();
		if(name)
			delete[]	name;
	}

	const	char	*ModuleDescriptor::getName(){

		return	name;
	}

	void	ModuleDescriptor::addSubscription_message(uint8_t	hostID,uint16_t	spaceID,uint16_t	MCID){
		if(!projections[hostID][spaceID])
			project(hostID,spaceID);
		P<ModuleEntry>	p=new	ModuleEntry(NodeEntry::Main[DC][MCID].get(this->hostID),this);
		(*projections[hostID][spaceID])->addSubscription(DC,MCID,NodeEntry::Main[DC][MCID][this->hostID].modules.addElementTail(p));
		p=NULL;
	}

	void	ModuleDescriptor::addSubscription_stream(uint8_t	hostID,uint16_t	spaceID,uint16_t	SID){

		if(!projections[hostID][spaceID])
			project(hostID,spaceID);
		P<ModuleEntry>	p=new	ModuleEntry(NodeEntry::Main[ST][SID].get(this->hostID),this);
		(*projections[hostID][spaceID])->addSubscription(ST,SID,NodeEntry::Main[ST][SID][this->hostID].modules.addElementTail(p));
		p=NULL;
	}

	void	ModuleDescriptor::removeSubscription_message(uint8_t	hostID,uint16_t	spaceID,uint16_t	MCID){
		(*projections[hostID][spaceID])->removeSubscription(DC,MCID);
		if(!(*projections[hostID][spaceID])->subscriptionCount[DC]	&&	!(*projections[hostID][spaceID])->subscriptionCount[ST])
			unproject(hostID,spaceID);
	}

	void	ModuleDescriptor::removeSubscription_stream(uint8_t	hostID,uint16_t	spaceID,uint16_t	SID){

		(*projections[hostID][spaceID])->removeSubscription(ST,SID);
		if(!(*projections[hostID][spaceID])->subscriptionCount[DC]	&&	!(*projections[hostID][spaceID])->subscriptionCount[ST])
			unproject(hostID,spaceID);
	}

	void	ModuleDescriptor::removeSubscriptions_message(uint8_t	hostID,uint16_t	spaceID){

		for(uint16_t	i=0;i<NodeEntry::Main[DC].count();i++)
			removeSubscription_message(hostID,spaceID,i);
		if(!(*projections[hostID][spaceID])->subscriptionCount[ST])
			unproject(hostID,spaceID);
	}

	void	ModuleDescriptor::removeSubscriptions_stream(uint8_t	hostID,uint16_t	spaceID){

		for(uint16_t	i=0;i<NodeEntry::Main[ST].count();i++)
			removeSubscription_stream(hostID,spaceID,i);
		if(!(*projections[hostID][spaceID])->subscriptionCount[DC])
			unproject(hostID,spaceID);
	}

	void	ModuleDescriptor::_activate(){
	}

	void	ModuleDescriptor::_deactivate(){
	}

	void	ModuleDescriptor::applyInitialProjections(uint8_t	hostID){

		for(uint32_t	i=0;i<initialProjections.count();i++){

			setActivationLevel(hostID,initialProjections[i].spaceID,initialProjections[i].activationLevel);
			for(uint32_t	j=0;j<initialProjections[i].subscriptions.count();j++){

				if(initialProjections[i].subscriptions[j].MCID==ClassRegister::NoClass)
					addSubscription_stream(hostID,initialProjections[i].spaceID,initialProjections[i].subscriptions[j].SID);
				else
					addSubscription_message(hostID,initialProjections[i].spaceID,initialProjections[i].subscriptions[j].MCID);
			}
		}
	}

	////////////////////////////////////////////////////////////////////////////////////////////////

	Projection<ModuleDescriptor>::Projection(ModuleDescriptor	*projected,Space	*space):_Projection<ModuleDescriptor,Projection<ModuleDescriptor> >(projected,space){

		subscriptionCount[0]=subscriptionCount[1]=0;
	}

	Projection<ModuleDescriptor>::~Projection(){

		if(activationLevel>=space->getActivationThreshold())
			deactivate();

		for(uint32_t	i=0;i<subscriptions[DC].count();i++){

			if(!subscriptions[DC][i])
				continue;
			(*subscriptions[DC][i])=NULL;
			subscriptions[DC][i].remove();
		}
		for(uint32_t	i=0;i<subscriptions[ST].count();i++){

			if(!subscriptions[ST][i])
				continue;
			(*subscriptions[ST][i])=NULL;
			subscriptions[ST][i].remove();
		}
	}

	void	Projection<ModuleDescriptor>::setActivationLevel(float	a){

		if(!space->activationCount){

			activationLevel=a;
			return;
		}

		if(activationLevel<space->getActivationThreshold()){

			if((activationLevel=a)>=space->getActivationThreshold())
				activate();
		}else	if((activationLevel=a)<space->getActivationThreshold())
			deactivate();
	}

	void	Projection<ModuleDescriptor>::updateActivationCount(float	t){

		if(activationLevel<space->getActivationThreshold()	||	space->reactivated){

			if(activationLevel>=t)
				activate();
		}else	if(activationLevel<t)
			deactivate();
	}

	void	Projection<ModuleDescriptor>::activate(){

		projected->activate();

		for(uint32_t	i=0;i<subscriptions[DC].count();i++){

			if(!subscriptions[DC][i])
				continue;
			(*subscriptions[DC][i])->node->incActivation();
		}
		for(uint32_t	i=0;i<subscriptions[ST].count();i++){

			if(!subscriptions[ST][i])
				continue;
			(*subscriptions[ST][i])->node->incActivation();
		}
	}

	void	Projection<ModuleDescriptor>::deactivate(){

		projected->deactivate();

		for(uint32_t	i=0;i<subscriptions[DC].count();i++){

			if(!subscriptions[DC][i])
				continue;
			(*subscriptions[DC][i])->node->decActivation();
		}
		for(uint32_t	i=0;i<subscriptions[ST].count();i++){

			if(!subscriptions[ST][i])
				continue;
			(*subscriptions[ST][i])->node->decActivation();
		}
	}

	void	Projection<ModuleDescriptor>::addSubscription(uint8_t	payloadType,uint16_t	ID,List<P<ModuleEntry>,1024>::Iterator	i){

		subscriptions[payloadType][ID]=i;
		if(space->activationCount	&&	activationLevel>=space->getActivationThreshold())
			(*subscriptions[payloadType][ID])->node->incActivation();
		subscriptionCount[payloadType]++;
	}

	void	Projection<ModuleDescriptor>::removeSubscription(uint8_t	payloadType,uint16_t	ID){

		if(space->activationCount	&&	activationLevel>=space->getActivationThreshold())
			(*subscriptions[payloadType][ID])->node->decActivation();
		*subscriptions[payloadType][ID]=NULL;
		subscriptions[payloadType][ID].remove();
		subscriptionCount[payloadType]--;
	}

}
