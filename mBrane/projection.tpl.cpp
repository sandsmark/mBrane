//	projection.tpl.cpp
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

	template<class	C>	inline	Projection<C>::Projection(C	*projected,Space	*space):Object<Memory,_Object,Projection<C> >(),projected(projected),space(space),activationLevel(0){
	}

	template<class	C>	inline	Projection<C>::~Projection(){

		if(activationLevel>=space->getActivationThreshold())
			projected->activationCount--;
	}

	template<class	C>	inline	void	Projection<C>::setActivationLevel(float32	a){

		if(activationLevel<space->getActivationThreshold()){

			if(a>=space->getActivationThreshold())
				projected->activationCount++;
		}else	if(a<space->getActivationThreshold())
				projected->activationCount--;
		activationLevel=a;
	}

	template<class	C>	inline	void	Projection<C>::updateActivationCount(float32	t){

		if(activationLevel<space->getActivationThreshold()){

			if(activationLevel>=t)
				projected->activationCount++;
		}else	if(activationLevel<t)
			projected->activationCount--;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////

	template<class	C>	Projectable<C>::Projectable():activationCount(0){
	}

	template<class	C>	Projectable<C>::~Projectable(){

		for(uint32	i=0;i<projections.count();i++){

			((P<Projection<C> >)projections[i])=NULL;
			projections[i].remove();
		}
	}

	template<class	C>	inline	void	Projectable<C>::project(uint16	spaceID){

		projections[spaceID]=Space::Main[spaceID]->project(new	Projection<C>((C	*)this,Space::Main[spaceID]));
	}

	template<class	C>	inline	void	Projectable<C>::unproject(uint16	spaceID){

		if(projections[spaceID]==Array<typename	List<P<Projection<C> > >::Iterator>::NullIndex)
			return;
		((P<Projection<C> >)projections[spaceID])=NULL;
		projections[spaceID].remove();
		projections[spaceID]=Array<typename	List<P<Projection<C> > >::Iterator>::NullIndex;
	}

	template<class	C>	inline	void	Projectable<C>::setActivationLevel(uint16	spaceID,float32	a){

		if(!projections[spaceID])
			project(spaceID);
		((P<Projection<C> >)projections[spaceID])->setActivationLevel(a);
	}
}