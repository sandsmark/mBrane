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

		if(Projection<C>::activationLevel>=Projection<C>::space->getActivationThreshold())
			deactivate();
	}

	template<class	C>	inline	void	Projection<C>::setActivationLevel(float32	a){

		if(!Projection<C>::space->activationCount){

			Projection<C>::activationLevel=a;
			return;
		}

		if(Projection<C>::activationLevel<Projection<C>::space->getActivationThreshold()){

			if((Projection<C>::activationLevel=a)>=Projection<C>::space->getActivationThreshold())
				activate();
		}else	if((Projection<C>::activationLevel=a)<Projection<C>::space->getActivationThreshold())
			deactivate();
	}

	template<class	C>	inline	void	Projection<C>::updateActivationCount(float32	t){

		if(Projection<C>::activationLevel<Projection<C>::space->getActivationThreshold()	||	Projection<C>::space->reactivated){

			if(Projection<C>::activationLevel>=t)
				activate();
		}else	if(Projection<C>::activationLevel<t)
			deactivate();
	}

	template<class	C>	inline	void	Projection<C>::activate(){

		Projection<C>::projected->activate();
	}

	template<class	C>	inline	void	Projection<C>::deactivate(){

		Projection<C>::projected->deactivate();
	}

	////////////////////////////////////////////////////////////////////////////////////////////////

	template<class	C>	Projectable<C>::Projectable(uint8	hostID,uint16	ID):Object<Memory,_Object,C>(),activationCount(0),hostID(hostID),ID(ID),reactivated(false){
	}

	template<class	C>	Projectable<C>::~Projectable(){

		for(uint8	i=0;i<projections.count();i++)
			for(uint16	j=0;j<projections[i].count();j++)
				unproject(i,j);
	}

	template<class	C>	inline	void	Projectable<C>::project(uint8	hostID,uint16	spaceID){

		projections[hostID][spaceID]=Space::Main[hostID][spaceID]->project(new	Projection<C>((C	*)this,Space::Main[hostID][spaceID]));
	}

	template<class	C>	inline	void	Projectable<C>::unproject(uint8	hostID,uint16	spaceID){
		if(!projections[hostID][spaceID])
			return;
		*projections[hostID][spaceID]=NULL;
		projections[hostID][spaceID].remove();
		projections[hostID][spaceID]=typename List<P<Projection<C> >,16>::Iterator();
	}

	template<class	C>	inline	void	Projectable<C>::setActivationLevel(uint8	hostID,uint16	spaceID,float32	a){
		if(!projections[hostID][spaceID])
			project(hostID,spaceID);
		(*projections[hostID][spaceID])->setActivationLevel(a);
	}

	template<class	C>	inline	void	Projectable<C>::activate(){

		if(++activationCount==1){

			// Node::Get()->trace(Node::EXECUTION)<<" activated"<<std::endl;
			reactivated=true;
			((C	*)this)->_activate();
			reactivated=false;
		}
	}

	template<class	C>	inline	void	Projectable<C>::deactivate(){

		if(--activationCount==0){

			// Node::Get()->trace(Node::EXECUTION)<<" deactivated"<<std::endl;
			((C	*)this)->_deactivate();
		}
	}
}
