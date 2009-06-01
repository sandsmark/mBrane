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
	}

	////////////////////////////////////////////////////////////////////////////////////////////////

	template<class	C>	inline	Projection<C>::Projection(C	*projected,Space	*space):_Projection<C,Projection<C> >(projected,space){
	}

	template<class	C>	inline	Projection<C>::~Projection(){

		if(activationLevel>=space->getActivationThreshold())
			deactivate();
	}

	template<class	C>	inline	void	Projection<C>::setActivationLevel(float32	a){

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

	template<class	C>	inline	void	Projection<C>::updateActivationCount(float32	t){

		if(activationLevel<space->getActivationThreshold()	||	space->reactivated){

			if(activationLevel>=t)
				activate();
		}else	if(activationLevel<t)
			deactivate();
	}

	template<class	C>	inline	void	Projection<C>::activate(){

		projected->activate();
	}

	template<class	C>	inline	void	Projection<C>::deactivate(){

		projected->deactivate();
	}

	////////////////////////////////////////////////////////////////////////////////////////////////

	template<class	C>	Projectable<C>::Projectable(uint16	ID):Object<Memory,_Object,C>(),activationCount(0),ID(ID),reactivated(false){
	}

	template<class	C>	Projectable<C>::~Projectable(){

		for(uint32	i=0;i<projections.count();i++)
			unproject(i);
	}

	template<class	C>	inline	void	Projectable<C>::project(uint16	spaceID){

		projections[spaceID]=Space::Main[spaceID]->project(new	Projection<C>((C	*)this,Space::Main[spaceID]));
	}

	template<class	C>	inline	void	Projectable<C>::unproject(uint16	spaceID){

		if(!projections[spaceID])
			return;
		*projections[spaceID]=NULL;
		projections[spaceID].remove();
		projections[spaceID]=List<P<Projection<C> >,16>::Iterator();
	}

	template<class	C>	inline	void	Projectable<C>::setActivationLevel(uint16	spaceID,float32	a){

		if(!projections[spaceID])
			project(spaceID);
		(*projections[spaceID])->setActivationLevel(a);
	}

	template<class	C>	inline	void	Projectable<C>::activate(){

		if(++activationCount==1){

			((C	*)this)->trace();
			Node::Get()->trace(Node::EXECUTION)<<" activated\n";
			reactivated=true;
			((C	*)this)->_activate();
			reactivated=false;
		}
	}

	template<class	C>	inline	void	Projectable<C>::deactivate(){

		if(--activationCount==0){

			((C	*)this)->trace();
			Node::Get()->trace(Node::EXECUTION)<<" deactivated\n";
			((C	*)this)->_deactivate();
		}
	}
}