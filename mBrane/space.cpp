//	space.cpp
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

#include	"space.h"


namespace	mBrane{

	Array<P<Space> >	Space::Main;

	Space::Space(uint16	ID):ID(ID),activationCount(0){
	}

	Space::~Space(){
	}

	inline	uint16	Space::id(){

		return	ID;
	}

	inline	void	Space::setActivationThreshold(float32	thr){

		_activationThreshold=thr;

		List<P<ModuleDescriptorProjection> >::Iterator	p_module;
		for(p_module=moduleDescriptors.begin();p_module!=moduleDescriptors.end();p_module++)
			((P<ModuleDescriptorProjection>)p_module)->updateActivationCount();

		List<P<Projection<Space> > >::Iterator	p_space;
		for(p_space=spaces.begin();p_space!=spaces.end();p_space++)
			((P<Projection<Space> >)p_space)->updateActivationCount();
	}

	float32	Space::getActivationThreshold(){

		return	_activationThreshold;
	}

	inline	List<P<ModuleDescriptorProjection> >::Iterator	Space::project(ModuleDescriptor	*m,float32	activationLevel){

		P<ModuleDescriptorProjection>	p=new	ModuleDescriptorProjection(m,this,activationLevel);
		return	moduleDescriptors.addElementTail(p);
	}

	inline	List<P<Projection<Space> > >::Iterator	Space::project(Space	*s,float32	activationLevel){

		P<Projection<Space> >	p=new	Projection<Space>(s,this,activationLevel);
		return	spaces.addElementTail(p);
	}
}