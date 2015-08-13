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

#include	"class_register.h"


namespace	mBrane{
	namespace	sdk{

                template<class	P,class	U,class	M>	uint64_t	___Payload<P,U,M>::_MetaData=ClassRegister::Load<U>();

                template<class	P,class	U,class	M>	inline	void	*___Payload<P,U,M>::New(uint32_t	size){

			return	new	U();
		}

                template<class	P,class	U,class	M>	inline	uint16_t	___Payload<P,U,M>::CID(){

                        return	(uint16_t)(_MetaData>>16);
		}

                template<class	P,class	U,class	M>	void	*___Payload<P,U,M>::Alloc(uint32_t	requested_size,uint32_t	&normalized_size){

			return	M::GetDynamic(requested_size)->alloc(normalized_size);
		}

                template<class	P,class	U,class	M>	void	___Payload<P,U,M>::Dealloc(uint32_t	requested_size,void	*o){

			M::GetDynamic(requested_size)->dealloc(o);
		}

		template<class	P,class	U,class	M>	inline	void	*___Payload<P,U,M>::operator	new(size_t	s){

			U	*p=(U	*)Object<M,P,U>::operator	new(s);
			p->_metaData=_MetaData;
			return	p;
		}
		
		template<class	P,class	U,class	M>	inline	void	___Payload<P,U,M>::operator	delete(void	*o){

			Object<M,P,U>::operator	delete(o);
		}

		template<class	P,class	U,class	M>	inline	size_t	___Payload<P,U,M>::Offset(){

			return	offsetof(U,_metaData);
		}

		template<class	P,class	U,class	M>	inline	___Payload<P,U,M>::___Payload():Object<M,P,U>(){

			this->_metaData=___Payload<P,U,M>::_MetaData;
		}

		template<class	P,class	U,class	M>	inline	___Payload<P,U,M>::~___Payload(){
		}

                template<class	P,class	U,class	M>	uint16_t	___Payload<P,U,M>::cid()	const{
			
                        return	(uint16_t)(this->_metaData>>16);
		}

		template<class	P,class	U,class	M>	size_t	___Payload<P,U,M>::size()	const{

			return	sizeof(U);
		}

		////////////////////////////////////////////////////////////////////////////////////

		template<class	U,class	M>	inline	Payload<U,M>::Payload():___Payload<_Payload,U,M>(){
		}

		template<class	U,class	M>	inline	Payload<U,M>::~Payload(){
		}

		////////////////////////////////////////////////////////////////////////////////////

		template<class	U,class	M>	inline	RPayload<U,M>::RPayload():___Payload<_RPayload,U,M>(){
		}

		template<class	U,class	M>	inline	RPayload<U,M>::~RPayload(){
		}

		////////////////////////////////////////////////////////////////////////////////////

		template<class	C,class	U,class	M>	inline	PayloadAdapter<C,U,M>::PayloadAdapter():Payload<U,M>(),C(){
		}

		template<class	C,class	U,class	M>	inline	PayloadAdapter<C,U,M>::~PayloadAdapter(){
		}

		////////////////////////////////////////////////////////////////////////////////////

		template<class	C,class	U,class	M>	inline	RPayloadAdapter<C,U,M>::RPayloadAdapter():RPayload<U,M>(),C(){
		}

		template<class	C,class	U,class	M>	inline	RPayloadAdapter<C,U,M>::~RPayloadAdapter(){
		}
	}
}
