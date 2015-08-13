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

namespace	mBrane{
	namespace	sdk{
		namespace	payloads{

			template<class	U>	ControlMessage<U>::ControlMessage()/*:Payload<U,Memory>()*/{

				this->_metaData = ControlMessage<U>::_MetaData | ControlMessage<U>::CONTROL;
			}

			template<class	U>	ControlMessage<U>::~ControlMessage(){
			}
			
			////////////////////////////////////////////////////////////////////////////////////////////////

                        template<class	U,class	M>	inline	StreamData<U,M>::StreamData(uint16_t	sid):Payload<U,M>(),_StreamData(sid){

				this->_metaData = StreamData<U,M>::_MetaData | StreamData<U,M>::STREAM;
			}

			template<class	U,class	M>	inline	StreamData<U,M>::~StreamData(){
			}

			template<class	U,class	M>	_StreamData	*StreamData<U,M>::as_StreamData(){

                                return	(_StreamData	*)(((uint8_t	*)this)+sizeof(Payload<U,M>));
			}

			////////////////////////////////////////////////////////////////////////////////////////////////

			template<class	U,class	M>	Message<U,M>::Message():Payload<U,M>(),_Message(){
				
				this->_metaData = Message<U,M>::_MetaData | Message<U,M>::DATA;
			}

			template<class	U,class	M>	Message<U,M>::~Message(){
			}

			template<class	U,class	M>	_Message	*Message<U,M>::as_Message(){

                                return	(_Message	*)(((uint8_t	*)this)+sizeof(Payload<U,M>));
			}

			////////////////////////////////////////////////////////////////////////////////////////////////

			template<class	U,class	M>	SharedObject<U,M>::SharedObject():Message<U,M>(){

				module::Node::Get()->addSharedObject(this);
			}

			template<class	U,class	M>	SharedObject<U,M>::~SharedObject(){
			}

			template<class	U,class	M>	void	SharedObject<U,M>::decRef(){

				if(_Payload::getOID()==0x00FFFFFF	||	isConstant())	//	object has not been sent yet: it has not been smart pointed by the cache: treat as a normal object.
					_Object::decRef();
				else{

                                        int32_t	ref_count=Atomic::Decrement32(&this->refCount);
					switch(ref_count){
					case	1:
						module::Node::Get()->markUnused(this);
						break;
					case	0:
						delete	this;
						break;
					}
				}
			}

			template<class	U,class	M>	bool	SharedObject<U,M>::isShared(){

				return	true;
			}
	
			template<class	U,class	M>	bool	SharedObject<U,M>::isConstant(){

				return	this->getNID()==0x80;
			}

			////////////////////////////////////////////////////////////////////////////////////////////////

                        template<class	S,typename	T>	void	*CStorage<S,T>::New(uint32_t	size){

                                uint32_t	capacity=size-sizeof(CStorage<S,T>);	//	capacity is now the size of the array
				capacity=capacity/sizeof(T);	//	now capacity equals the number of elements in the array
				return	new(capacity)	CStorage<S,T>();
			}

			template<class	S,typename	T>	void	*CStorage<S,T>::operator	new(size_t	s){

				return	NULL;
			}

                        template<class	S,typename	T>	void	*CStorage<S,T>::operator	new(size_t	s,uint32_t	capacity){

                                uint32_t	normalizedSize;
                                uint32_t	size=sizeof(CStorage<S,T>)+capacity*sizeof(T);
				CStorage<S,T>	*o=(CStorage<S,T>	*)S::Alloc(size,normalizedSize);
				o->_size=size;
				o->_capacity=capacity;
				o->_metaData=S::_MetaData;
				return	o;
			}

			template<class	S,typename	T>	void	CStorage<S,T>::operator	delete(void	*o){

				S::Dealloc(((CStorage<S,T>	*)o)->_size,o);
			}

			template<class	S,typename	T>	CStorage<S,T>::CStorage():S(){

                                _data=(T	*)(((uint8_t	*)this)+sizeof(S)+sizeof(uint32_t)+sizeof(uint32_t)+sizeof(T	*));
			}

			template<class	S,typename	T>	CStorage<S,T>::~CStorage(){
			}

			template<class	S,typename	T>	size_t	CStorage<S,T>::size()	const{

				return	_size;
			}

                        template<class	S,typename	T>	uint32_t	CStorage<S,T>::getCapacity()	const{

				return	_capacity;
			}

                        template<class	S,typename	T>	T	&CStorage<S,T>::data(uint32_t	i)	const{

				return	_data[i];
			}

                        template<class	S,typename	T>	T	&CStorage<S,T>::data(uint32_t	i){

				return	_data[i];
			}

			template<class	S,typename	T>	T	*CStorage<S,T>::data(){

				return	_data;
			}
		}
	}
}
