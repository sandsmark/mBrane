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

#include	<string.h>

namespace	mBrane{
	namespace	sdk{
		namespace	payloads{
/*
			template<typename	T>	void	*Storage<T>::New(uint32	size){	
				
				uint32	ns;
				return	new(size,ns)	Storage();
			}
			
			template<typename	T>	void	*Storage<T>::operator	new(size_t	s,uint32	size,uint32	&normalizedSize){

				Storage<T>	*storage=(Storage<T>	*)Memory::GetDynamic(size)->alloc(normalizedSize);
				storage->normalizedSize=normalizedSize;
				storage->count=(size-sizeof(Storage<T>))/sizeof(T);
				return	storage;
			}
			
			template<typename	T>	void	Storage<T>::operator	delete(void	*storage){

				Memory::GetDynamic(((Storage<T>	*)storage)->getNormalizedSize())->dealloc(storage);
			}
			
			template<typename	T>	inline	Storage<T>::Storage(){	
				
				this->_metaData=Storage<T>::_MetaData;
				this->data=((T	*)(((uint8	*)this)+offsetof(Storage<T>,data)+sizeof(T	*)));
			}

			template<typename	T>	Storage<T>::~Storage(){
			}

			template<typename	T>	inline	uint32	Storage<T>::getNormalizedSize(){

				return	normalizedSize;
			}

			template<typename	T>	inline	void	Storage<T>::setNormalizedSize(uint32	size){	
				
				normalizedSize=size;
			}
				
			template<typename	T>	T	&Storage<T>::operator	[](uint32	i){	
				
				return	data[i];
			}
			
			template<typename	T>	size_t	Storage<T>::size()	const{	
				
				return	sizeof(Storage<T>)+sizeof(T)*count;
			}

			////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

			template<typename	T>	void	*Storage<P<T> >::New(uint32	size){	
				
				uint32	ns;
				return	new(size,ns)	Storage();
			}
			
			template<typename	T>	void	*Storage<P<T> >::operator	new(size_t	s,uint32	size,uint32	&normalizedSize){

				Storage<T>	*storage=(Storage<P<T> >	*)Memory::GetDynamic(size)->alloc(normalizedSize);
				storage->normalizedSize=normalizedSize;
				storage->count=(size-sizeof(Storage<P<T> >))/sizeof(T);
				return	storage;
			}
			
			template<typename	T>	void	Storage<P<T> >::operator	delete(void	*storage){

				Memory::GetDynamic(((Storage<P<T> >	*)storage)->getNormalizedSize())->dealloc(storage);
			}
			
			template<typename	T>	inline	Storage<P<T> >::Storage(){	
				
				this->_metaData=Storage<P<T> >::_MetaData;
				this->data=((P<T>	*)(((uint8	*)this)+offsetof(Storage<P<T> >,data)+sizeof(P<T>	*)));
			}

			template<typename	T>	Storage<P<T> >::~Storage(){
			}

			template<typename	T>	inline	uint32	Storage<P<T> >::getNormalizedSize(){

				return	normalizedSize;
			}

			template<typename	T>	inline	void	Storage<P<T> >::setNormalizedSize(uint32	size){	
				
				normalizedSize=size;
			}
				
			template<typename	T>	P<T>	&Storage<P<T> >::operator	[](uint32	i){	
				
				return	data[i];
			}
			
			template<typename	T>	size_t	Storage<P<T> >::size()	const{	
				
				return	sizeof(Storage<P<T> >)+sizeof(P<T>)*count;
			}
				
			template<typename	T>	uint16	Storage<P<T> >::ptrCount()	const{
				
				return	count;
			}
			
			template<typename	T>	__Payload	*Storage<P<T> >::getPtr(uint16	i)	const{
				
				return	data[i];
			}
			
			template<typename	T>	void	Storage<P<T> >::setPtr(uint16	i,__Payload	*p){
				
				data[i]=(T	*)p;
			}

			////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

			template<typename	T>	Array<T>::Array():_data(NULL),_maxCount(0){
			}

			template<typename	T>	Array<T>::~Array(){
			}

			template<typename	T>	inline	void	Array<T>::ensure(uint32	count){

				if(count<=_maxCount)
					return;
				uint32	s=count*sizeof(T)+sizeof(Storage<T>);
				uint32	normalizedSize;
				Storage<T>	*newStorage=new(s,normalizedSize)	Storage<T>();
				newStorage->setNormalizedSize(normalizedSize);
				if(_data!=NULL)
					memcpy(newStorage->data,_data->data,_data->count*sizeof(T));
				_data=newStorage;
				_maxCount=(normalizedSize-sizeof(Storage<T>))/sizeof(T);
			}

			template<typename	T>	inline	uint32	Array<T>::count()	const{

				return	_data->count;
			}

			template<typename	T>	inline	T	&Array<T>::operator	[](uint32	i){

				ensure(i+1);
				if(i>=_data->count)
					_data->count=i+1;
				return	_data->operator	[](i);
			}

			template<typename	T>	inline	uint8	*Array<T>::asBytes(uint32	i){

				return	(uint8	*)(_data+i);
			}

			template<typename	T>	inline	uint16	Array<T>::ptrCount()	const{

				return	1;
			}

			template<typename	T>	inline	__Payload	*Array<T>::getPtr(uint16	i)	const{

				return	_data;
			}

			template<typename	T>	inline	void	Array<T>::setPtr(uint16	i,__Payload	*p){

				_data=(Storage<T>	*)p;
			}*/
		}
	}
}
