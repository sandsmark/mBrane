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

#include	<string.h>

namespace	mBrane
{
namespace	sdk
{
namespace	payloads
{
/*
			template<typename	T>	void	*Storage<T>::New(uint32_t	size){

				uint32_t	ns;
				return	new(size,ns)	Storage();
			}

			template<typename	T>	void	*Storage<T>::operator	new(size_t	s,uint32_t	size,uint32_t	&normalizedSize){

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
				this->data=((T	*)(((uint8_t	*)this)+offsetof(Storage<T>,data)+sizeof(T	*)));
			}

			template<typename	T>	Storage<T>::~Storage(){
			}

			template<typename	T>	inline	uint32_t	Storage<T>::getNormalizedSize(){

				return	normalizedSize;
			}

			template<typename	T>	inline	void	Storage<T>::setNormalizedSize(uint32_t	size){

				normalizedSize=size;
			}

			template<typename	T>	T	&Storage<T>::operator	[](uint32_t	i){

				return	data[i];
			}

			template<typename	T>	size_t	Storage<T>::size()	const{

				return	sizeof(Storage<T>)+sizeof(T)*count;
			}

			////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

			template<typename	T>	void	*Storage<P<T> >::New(uint32_t	size){

				uint32_t	ns;
				return	new(size,ns)	Storage();
			}

			template<typename	T>	void	*Storage<P<T> >::operator	new(size_t	s,uint32_t	size,uint32_t	&normalizedSize){

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
				this->data=((P<T>	*)(((uint8_t	*)this)+offsetof(Storage<P<T> >,data)+sizeof(P<T>	*)));
			}

			template<typename	T>	Storage<P<T> >::~Storage(){
			}

			template<typename	T>	inline	uint32_t	Storage<P<T> >::getNormalizedSize(){

				return	normalizedSize;
			}

			template<typename	T>	inline	void	Storage<P<T> >::setNormalizedSize(uint32_t	size){

				normalizedSize=size;
			}

			template<typename	T>	P<T>	&Storage<P<T> >::operator	[](uint32_t	i){

				return	data[i];
			}

			template<typename	T>	size_t	Storage<P<T> >::size()	const{

				return	sizeof(Storage<P<T> >)+sizeof(P<T>)*count;
			}

			template<typename	T>	uint16_t	Storage<P<T> >::ptrCount()	const{

				return	count;
			}

			template<typename	T>	__Payload	*Storage<P<T> >::getPtr(uint16_t	i)	const{

				return	data[i];
			}

			template<typename	T>	void	Storage<P<T> >::setPtr(uint16_t	i,__Payload	*p){

				data[i]=(T	*)p;
			}

			////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

			template<typename	T>	Array<T>::Array():_data(NULL),_maxCount(0){
			}

			template<typename	T>	Array<T>::~Array(){
			}

			template<typename	T>	inline	void	Array<T>::ensure(uint32_t	count){

				if(count<=_maxCount)
					return;
				uint32_t	s=count*sizeof(T)+sizeof(Storage<T>);
				uint32_t	normalizedSize;
				Storage<T>	*newStorage=new(s,normalizedSize)	Storage<T>();
				newStorage->setNormalizedSize(normalizedSize);
				if(_data!=NULL)
					memcpy(newStorage->data,_data->data,_data->count*sizeof(T));
				_data=newStorage;
				_maxCount=(normalizedSize-sizeof(Storage<T>))/sizeof(T);
			}

			template<typename	T>	inline	uint32_t	Array<T>::count()	const{

				return	_data->count;
			}

			template<typename	T>	inline	T	&Array<T>::operator	[](uint32_t	i){

				ensure(i+1);
				if(i>=_data->count)
					_data->count=i+1;
				return	_data->operator	[](i);
			}

			template<typename	T>	inline	uint8_t	*Array<T>::asBytes(uint32_t	i){

				return	(uint8_t	*)(_data+i);
			}

			template<typename	T>	inline	uint16_t	Array<T>::ptrCount()	const{

				return	1;
			}

			template<typename	T>	inline	__Payload	*Array<T>::getPtr(uint16_t	i)	const{

				return	_data;
			}

			template<typename	T>	inline	void	Array<T>::setPtr(uint16_t	i,__Payload	*p){

				_data=(Storage<T>	*)p;
			}*/
}
}
}
