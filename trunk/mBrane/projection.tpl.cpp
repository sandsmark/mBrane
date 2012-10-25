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
