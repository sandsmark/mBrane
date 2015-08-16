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

#include	<memory>
#include	"payload.h"


namespace	mBrane{
	namespace	sdk{

		__Payload::__Payload(){
		}

		__Payload::~__Payload(){
		}

		void	__Payload::init(){
		}

		uint16_t	__Payload::ptrCount()	const{

			return	0;
		}

		__Payload	*__Payload::getPtr(uint16_t	i)	const{

			return	NULL;
		}

		void	__Payload::setPtr(uint16_t	i,__Payload	*p){
		}

		bool	__Payload::isShared()	const{

			return	false;
		}

		bool	__Payload::isConstant()	const{

			return	false;
		}

		////////////////////////////////////////////////////////////////////////////////////

		uint32_t	_Payload::LastConstantOID=0;
		uint32_t	_Payload::LastSharedOID=0;

		_Payload::_Payload():__Payload(){
		}

		_Payload::~_Payload(){
		}

		_Payload::Category	_Payload::category()	const{

			return	(_Payload::Category)(_metaData	&	0x0000000000000003);
		}

		uint64_t	&_Payload::node_send_ts(){

			return	_node_send_ts;
		}

		uint64_t	&_Payload::node_recv_ts(){

			return	_node_recv_ts;
		}

		uint64_t	&_Payload::send_ts(){

			return	_send_ts;
		}
				
		uint64_t	&_Payload::recv_ts(){

			return	_recv_ts;
		}

		payloads::_Message	*_Payload::as_Message(){

			return	NULL;
		}

		payloads::_StreamData	*_Payload::as_StreamData(){

			return	NULL;
		}

		void	_Payload::setOID(uint8_t	NID){

			uint64_t	oid=NID	&	0x7F;
			oid<<=24;
			oid|=LastSharedOID++;
			_metaData&=0x00000000FFFFFFFF;
			_metaData|=(oid<<32);
		}

		void	_Payload::setOID(){

			uint64_t	oid=0x80000000;
			oid|=LastConstantOID++;
			_metaData&=0x00000000FFFFFFFF;
			_metaData|=(oid<<32);
		}

		uint32_t	_Payload::getOID()	const{

			return	_metaData>>32;
		}

		uint32_t	_Payload::getID()	const{

			return	(_metaData>>32)	&	0x00FFFFFF;
		}

		uint8_t	_Payload::getNID()	const{

			return	_metaData>>56;
		}

		////////////////////////////////////////////////////////////////////////////////////

		_RPayload::_RPayload():__Payload(){
		}

		_RPayload::~_RPayload(){
		}
	}
}
