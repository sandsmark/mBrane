//	space.cpp
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

#include	"space.h"
#include	"node.h"


namespace	mBrane{

	Array<P<Space>,16>	Space::Main;

	Space	*Space::Get(const	char	*name){

		if(strcmp(name,"root")==0)
			return	Space::Main[0];
		for(uint16	i=0;i<Space::Main.count();i++)
			if(strcmp(Space::Main[i]->name,name)==0)
				return	Space::Main[i];
		return	NULL;
	}

	Space	*Space::New(XMLNode	&n){

		const	char	*name=n.getAttribute("name");
		if(!name){

			std::cout<<"> Error: Space::name is missing\n";
			return	NULL;
		}

		const	char	*_activationThreshold=n.getAttribute("activation_threshold");
		if(!_activationThreshold){

			std::cout<<"> Error: Space::activation_threshold is missing\n";
			return	NULL;
		}

		Space	*s=new	Space(name);
		Space::Main[Space::Main.count()]=s;

		uint16	projectionCount=n.nChildNode("Projection");
		if(!projectionCount)	//	when no projection is defined, the space is projected on root at the highest activation level
			s->setActivationLevel(0,1);
		else{
			
			for(uint16	i=0;i<projectionCount;i++){

				XMLNode	projection=n.getChildNode("Projection",i);
				const	char	*spaceName=projection.getAttribute("space");	//	to be projected on
				if(!spaceName){

					std::cout<<"> Error: Space: "<<name<<" ::Projection::name is Missing\n";
					goto	error;
				}
				const	char	*_activationLevel=projection.getAttribute("activation_level");
				if(!_activationLevel){

					std::cout<<"> Error: Space: "<<name<<" ::Projection::activation_level is Missing\n";
					goto	error;
				}
				Space	*_s=Space::Get(spaceName);
				if(!_s){

					std::cout<<"> Error: Space "<<spaceName<<" does not exist\n";
					goto	error;
				}
				s->setActivationLevel(_s->ID,(float32)atof(_activationLevel));
			}
		}

		s->setActivationThreshold((float32)atof(_activationThreshold));

		return	s;
error:	Space::Main[s->ID]=NULL;
		return	NULL;
	}

	void	Space::Init(){

		Space::Main[0]=new	Space("Root");	//	root space
		Space::Main[0]->activationCount=1;	//	root is always active
		Space::Main[0]->setActivationThreshold(1.0);	//	TODO: read initial threshold from config file
	}

	uint16	Space::GetID(){

		for(uint16	i=0;i<Main.count();i++)
			if(Main[i]==NULL)
				return	i;
		return	Main.count();
	}

	Space::Space(const	char	*name):Projectable<Space>((uint16)Space::Main.count()){

		if(name){

			this->name=new	char[strlen(name)+1];
			memcpy((void	*)this->name,name,strlen(name)+1);
		}

		Node::Get()->trace(Node::EXECUTION)<<"Space "<<ID<<" created\n";
	}

	Space::~Space(){

		if(name)
			delete[]	name;

		Node::Get()->trace(Node::EXECUTION)<<"Space "<<ID<<" deleted\n";
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

	inline	void	Space::trace(){

		Node::Get()->trace(Node::EXECUTION)<<"Space "<<ID;
	}
}