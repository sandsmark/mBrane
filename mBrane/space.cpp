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


namespace	mBrane{

	Array<P<Space> >	Space::Main;

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

			std::cout<<"Error: Space::name is missing\n";
			return	NULL;
		}

		const	char	*_activationThreshold=n.getAttribute("activation_threshold");
		if(!_activationThreshold){

			std::cout<<"Error: Space::activation_threshold is missing\n";
			return	NULL;
		}

		Space	*s=new	Space(name);
		s->setActivationThreshold(atoi(_activationThreshold));

		uint16	projectionCount=n.nChildNode("Projection");
		if(!projectionCount)
			s->setActivationLevel(0,1);
		else	for(uint16	i=0;i<projectionCount;i++){

			XMLNode	projection=n.getChildNode("Projection",i);
			const	char	*spaceName=projection.getAttribute("space");	//	to be projected on
			if(!spaceName){

				std::cout<<"Error: Space: "<<name<<" ::Projection::name is Missing\n";
				goto	error;
			}
			const	char	*_activationLevel=projection.getAttribute("activation_level");
			if(!_activationLevel){

				std::cout<<"Error: Space: "<<name<<" ::Projection::activation_level is Missing\n";
				goto	error;
			}
			Space	*_s=Space::Get(spaceName);
			if(!_s){

				std::cout<<"Error: Space "<<spaceName<<" does not exist\n";
				goto	error;
			}
			s->setActivationLevel(_s->ID,atoi(_activationLevel));
		}

		return	s;
error:	delete	s;
		return	NULL;
	}

	void	Space::Init(){

		for(uint16	i=0;i<Space::Main.count();i++)
			Space::Main[i]->setActivationThreshold(Space::Main[i]->getActivationThreshold());
	}

	Space::Space(const	char	*name):Projectable<Space>(Space::Main.count()),activationCount(0){

		if(name){

			this->name=new	char[strlen(name)];
			memcpy((void	*)this->name,name,strlen(name));
		}
	}

	Space::~Space(){

		delete[]	name;
	}

	const	char	*Space::getName(){

		return	name;
	}

	void	Space::setActivationThreshold(float32	thr){

		List<P<Projection<ModuleDescriptor> > >::Iterator	p_module;
		for(p_module=moduleDescriptors.begin();p_module!=moduleDescriptors.end();p_module++)
			((P<Projection<ModuleDescriptor> >)p_module)->updateActivationCount(thr);
		List<P<Projection<Space> > >::Iterator	p_space;
		for(p_space=spaces.begin();p_space!=spaces.end();p_space++)
			((P<Projection<Space> >)p_space)->updateActivationCount(thr);
		_activationThreshold=thr;
	}

	float32	Space::getActivationThreshold(){

		return	_activationThreshold;
	}

	List<P<Projection<ModuleDescriptor> > >::Iterator	Space::project(Projection<ModuleDescriptor>	*p){

		P<Projection<ModuleDescriptor> >	_p=p;
		return	moduleDescriptors.addElementTail(_p);
	}

	List<P<Projection<Space> > >::Iterator	Space::project(Projection<Space>	*p){

		P<Projection<Space> >	_p=p;
		return	spaces.addElementTail(_p);
	}

	inline	void	Space::activate(){

		List<P<Projection<Space> > >::Iterator	i;
		for(i=spaces.begin();i!=spaces.end();i++)
			((P<Projection<Space> >)i)->activate();
		List<P<Projection<ModuleDescriptor> > >::Iterator	j;
		for(j=moduleDescriptors.begin();j!=moduleDescriptors.end();j++)
			((P<Projection<ModuleDescriptor> >)j)->activate();
	}

	inline	void	Space::deactivate(){

		List<P<Projection<Space> > >::Iterator	i;
		for(i=spaces.begin();i!=spaces.end();i++)
			((P<Projection<Space> >)i)->deactivate();
		List<P<Projection<ModuleDescriptor> > >::Iterator	j;
		for(j=moduleDescriptors.begin();j!=moduleDescriptors.end();j++)
			((P<Projection<ModuleDescriptor> >)j)->deactivate();
	}
}