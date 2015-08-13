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

#include	"space.h"
#include	"projection.tpl.cpp"
#include	"node.h"


namespace	mBrane{

	Array<P<Space>,16>			Space::Config;

	Array<Array<P<Space>,16>,8>	Space::Main;

	Space	*Space::Get(const	char	*name){

		if(stricmp(name,"root")==0)
			return	Space::Config[0];
		for(uint16	i=0;i<Space::Config.count();i++)
			if(stricmp(Space::Config[i]->name,name)==0)
				return	Space::Config[i];
		return	NULL;
	}

	Space	*Space::New(XMLNode	&n){

		const	char	*name=n.getAttribute("name");
		if(!name){

			std::cout<<"> Error: Space::name is missing"<<std::endl;
			return	NULL;
		}

		const	char	*_activationThreshold=n.getAttribute("activation_threshold");
		if(!_activationThreshold){

			std::cout<<"> Error: Space::activation_threshold is missing"<<std::endl;
			return	NULL;
		}

		Space	*s=new	Space(module::Node::NoID,name);
		Space::Config[Space::Config.count()]=s;

		uint16	projectionCount=n.nChildNode("Projection");
		if(!projectionCount){	//	when no projection is defined, the space is projected on root at the highest activation level.

			s->initialProjections[0].spaceID=0;
			s->initialProjections[0].activationLevel=1.0;
		}else{
			
			for(uint16	i=0;i<projectionCount;i++){

				XMLNode	projection=n.getChildNode("Projection",i);
				const	char	*spaceName=projection.getAttribute("space");	//	to be projected on.
				if(!spaceName){

					std::cout<<"> Error: Space: "<<name<<" ::Projection::name is missing"<<std::endl;
					goto	error;
				}
				const	char	*_activationLevel=projection.getAttribute("activation_level");
				if(!_activationLevel){

					std::cout<<"> Error: Space: "<<name<<" ::Projection::activation_level is missing"<<std::endl;
					goto	error;
				}
				Space	*_s=Space::Get(spaceName);
				if(!_s){

					std::cout<<"> Error: Space "<<spaceName<<" does not exist"<<std::endl;
					goto	error;
				}
				s->initialProjections[i].spaceID=_s->ID;
				s->initialProjections[i].activationLevel=(float32)atof(_activationLevel);
			}
		}

		s->initialActivationThreshold=(float32)atof(_activationThreshold);

		return	s;
error:	Space::Config[s->ID]=NULL;
		return	NULL;
	}

	void	Space::InitRoot(){

		Space::Config[0]=new	Space(module::Node::NoID,"Root");	//	root space
		Space::Config[0]->activationCount=1;	//	root is always active
		Space::Config[0]->initialActivationThreshold=1.0;	//	TODO: read initial threshold from config file
	}

	void	Space::Init(uint8	hostID){

		for(uint32	i=0;i<Space::Config.count();i++){

			Space::Main[hostID][i]=Space::Config[i];
			Space::Main[hostID][i]->hostID=hostID;
			Space::Main[hostID][i]->applyInitialProjections(hostID);

		//	Space::Config[i]=NULL;
		}		
	}

	uint16	Space::GetID(uint8	hostID){

		for(uint16	i=0;i<Main[hostID].count();i++)
			if(Main[hostID][i]==NULL)
				return	i;
		return	(uint16)Main[hostID].count();
	}

	Space::Space(uint8	hostID,const	char	*name):Projectable<Space>(hostID,(uint16)Config.count()){

		if(name){

			this->name=new	char[strlen(name)+1];
			memcpy((void	*)this->name,name,strlen(name)+1);
		}

		Node::Get()->trace(Node::EXECUTION)<<"> Info: Space "<<ID<<" created"<<std::endl;
	}

	Space::~Space(){

		if(name)
			delete[]	name;

		Node::Get()->trace(Node::EXECUTION)<<"Space "<<ID<<" deleted"<<std::endl;
	}

	const	char	*Space::getName(){

		return	name;
	}

	void	Space::setActivationThreshold(float32	thr){

		_activationThreshold=thr;
		if(!activationCount)
			return;
		_activate();
	}

	float32	Space::getActivationThreshold(){

		return	_activationThreshold;
	}

	List<P<Projection<ModuleDescriptor> >,16>::Iterator	Space::project(Projection<ModuleDescriptor>	*p){
		P<Projection<ModuleDescriptor> >	_p=p;
		return	moduleDescriptors.addElementTail(_p);
	}

	List<P<Projection<Space> >,16>::Iterator	Space::project(Projection<Space>	*p){

		P<Projection<Space> >	_p=p;
		return	spaces.addElementTail(_p);
	}

	inline	void	Space::_activate(){	//	called whenever a check on the children is needed (setThreshold and setActivationLevel)

		List<P<Projection<Space> >,16>::Iterator	i;
		for(i=spaces.begin();i;++i)
			(*i)->updateActivationCount(_activationThreshold);
		List<P<Projection<ModuleDescriptor> >,16>::Iterator	j;
		for(j=moduleDescriptors.begin();j;++j)
			(*j)->updateActivationCount(_activationThreshold);
	}

	inline	void	Space::_deactivate(){

		List<P<Projection<Space> >,16>::Iterator	i;
		for(i=spaces.begin();i;++i)
			(*i)->deactivate();
		List<P<Projection<ModuleDescriptor> >,16>::Iterator	j;
		for(j=moduleDescriptors.begin();j;++j)
			(*j)->deactivate();
	}

	void	Space::applyInitialProjections(uint8	hostID){

		for(uint32	i=0;i<initialProjections.count();i++)
			setActivationLevel(hostID,initialProjections[i].spaceID,initialProjections[i].activationLevel);
		
		setActivationThreshold(initialActivationThreshold);
	}
}
