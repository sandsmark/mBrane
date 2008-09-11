//	projection.tpl.cpp
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

	template<class	C,class	U>	inline	_Projection<C,U>::_Projection(C	*projected,Space	*space):Object<Memory,_Object,U>(),projected(projected),space(space),activationLevel(0){
	}

	template<class	C,class	U>	inline	_Projection<C,U>::~_Projection(){

		if(activationLevel>=space->getActivationThreshold())
			deactivate();
	}

	template<class	C,class	U>	inline	void	_Projection<C,U>::setActivationLevel(float32	a){

		if(activationLevel<space->getActivationThreshold()){

			if(a>=space->getActivationThreshold())
				activate();
		}else	if(a<space->getActivationThreshold())
			deactivate();
		activationLevel=a;
	}

	template<class	C,class	U>	inline	void	_Projection<C,U>::updateActivationCount(float32	t){

		if(activationLevel<space->getActivationThreshold()){

			if(activationLevel>=t)
				activate();
		}else	if(activationLevel<t)
			deactivate();
	}

	template<class	C,class	U>	inline	void	_Projection<C,U>::activate(){

		projected->activate();
		((U	*)this)->activate();
	}

	template<class	C,class	U>	inline	void	_Projection<C,U>::deactivate(){

		projected->deactivate();
		((U	*)this)->deactivate();
	}

	////////////////////////////////////////////////////////////////////////////////////////////////

	template<class	C>	Projection<C>::Projection(C	*projected,Space	*space):_Projection<C,Projection<C> >(projected,space){
	}

	template<class	C>	Projection<C>::~Projection(){
	}

	////////////////////////////////////////////////////////////////////////////////////////////////

	template<class	C>	Projectable<C>::Projectable(uint16	ID):activationCount(0),ID(ID){
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

	template<class	C>	inline	void	Projectable<C>::activate(){

		activationCount++;
		((C	*)this)->activate();
	}

	template<class	C>	inline	void	Projectable<C>::deactivate(){

		activationCount--;
		((C	*)this)->deactivate();
	}
}