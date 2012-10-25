//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/ //_/_/ //_/_/  payload.cpp //_/_/ //_/_/  Eric Nivel //_/_/  Center for Analysis and Design of Intelligent Agents //_/_/    Reykjavik University, Menntavegur 1, 101 Reykjavik, Iceland //_/_/    http://cadia.ru.is
//_/_/  Copyright©2012 //_/_/ //_/_/  This software was developed by the above copyright holder as part of  //_/_/  the HUMANOBS EU research project, in collaboration with the  //_/_/  following parties:
//_/_/   //_/_/  Autonomous Systems Laboratory //_/_/    Technical University of Madrid, Spain //_/_/    http://www.aslab.org/ //_/_/ //_/_/  Communicative Machines //_/_/    Edinburgh, United Kingdom //_/_/    http://www.cmlabs.com/ //_/_/ //_/_/  Istituto Dalle Molle di Studi sull'Intelligenza Artificiale //_/_/    University of Lugano and SUPSI, Switzerland //_/_/    http://www.idsia.ch/ //_/_/ //_/_/  Institute of Cognitive Sciences and Technologies //_/_/    Consiglio Nazionale delle Ricerche, Italy //_/_/    http://www.istc.cnr.it/
//_/_/
//_/_/  Dipartimento di Ingegneria Informatica
//_/_/    University of Palermo, Italy
//_/_/    http://roboticslab.dinfo.unipa.it/index.php/Main/HomePage
//_/_/ //_/_/
//_/_/  --- HUMANOBS Open-Source BSD License, with CADIA Clause v 1.0 ---
//_/_/
//_/_/  Redistribution and use in source and binary forms, with or without 
//_/_/  modification, is permitted provided that the following conditions 
//_/_/  are met:
//_/_/
//_/_/  - Redistributions of source code must retain the above copyright 
//_/_/    and collaboration notice, this list of conditions and the 
//_/_/    following disclaimer.
//_/_/
//_/_/  - Redistributions in binary form must reproduce the above copyright 
//_/_/    notice, this list of conditions and the following
//_/_/    disclaimer in the documentation and/or other materials provided 
//_/_/    with the distribution.
//_/_/
//_/_/  - Neither the name of its copyright holders nor the names of its 
//_/_/    contributors may be used to endorse or promote products 
//_/_/    derived from this software without specific prior written permission.
//_/_/
//_/_/  - CADIA Clause: The license granted in and to the software under this 
//_/_/     agreement is a limited-use license. The software may not be used in 
//_/_/     furtherance of: 
//_/_/     (i) intentionally causing bodily injury or severe emotional distress 
//_/_/         to any person; 
//_/_/     (ii) invading the personal privacy or violating the human rights of 
//_/_/         any person; or 
//_/_/     (iii) committing  or preparing for any act of war.
//_/_/
//_/_/  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
//_/_/  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
//_/_/  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR 
//_/_/  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
//_/_/  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
//_/_/  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
//_/_/  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, 
//_/_/  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY 
//_/_/  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
//_/_/  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
//_/_/  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//_/_/
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

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

		uint16	__Payload::ptrCount()	const{

			return	0;
		}

		__Payload	*__Payload::getPtr(uint16	i)	const{

			return	NULL;
		}

		void	__Payload::setPtr(uint16	i,__Payload	*p){
		}

		bool	__Payload::isShared()	const{

			return	false;
		}

		bool	__Payload::isConstant()	const{

			return	false;
		}

		////////////////////////////////////////////////////////////////////////////////////

		uint32	_Payload::LastConstantOID=0;
		uint32	_Payload::LastSharedOID=0;

		_Payload::_Payload():__Payload(){
		}

		_Payload::~_Payload(){
		}

		_Payload::Category	_Payload::category()	const{

			return	(_Payload::Category)(_metaData	&	0x0000000000000003);
		}

		uint64	&_Payload::node_send_ts(){

			return	_node_send_ts;
		}

		uint64	&_Payload::node_recv_ts(){

			return	_node_recv_ts;
		}

		uint64	&_Payload::send_ts(){

			return	_send_ts;
		}
				
		uint64	&_Payload::recv_ts(){

			return	_recv_ts;
		}

		payloads::_Message	*_Payload::as_Message(){

			return	NULL;
		}

		payloads::_StreamData	*_Payload::as_StreamData(){

			return	NULL;
		}

		void	_Payload::setOID(uint8	NID){

			uint64	oid=NID	&	0x7F;
			oid<<=24;
			oid|=LastSharedOID++;
			_metaData&=0x00000000FFFFFFFF;
			_metaData|=(oid<<32);
		}

		void	_Payload::setOID(){

			uint64	oid=0x80000000;
			oid|=LastConstantOID++;
			_metaData&=0x00000000FFFFFFFF;
			_metaData|=(oid<<32);
		}

		uint32	_Payload::getOID()	const{

			return	_metaData>>32;
		}

		uint32	_Payload::getID()	const{

			return	(_metaData>>32)	&	0x00FFFFFF;
		}

		uint8	_Payload::getNID()	const{

			return	_metaData>>56;
		}

		////////////////////////////////////////////////////////////////////////////////////

		_RPayload::_RPayload():__Payload(){
		}

		_RPayload::~_RPayload(){
		}
	}
}
