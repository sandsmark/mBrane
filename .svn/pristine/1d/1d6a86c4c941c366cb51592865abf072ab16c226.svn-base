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

#include	"network_id.h"

#include	<cstring>

namespace	mBrane{

	uint16	NetworkID::DiscoveryIDSize=0;

	uint16	NetworkID::Size=0;
	
	uint16	NetworkID::CtrlIDSize[2]={	0,	0	};
	
	uint16	NetworkID::DataIDSize[2]={	0,	0	};
	
	uint16	NetworkID::StreamIDSize[2]={	0,	0	};
	
	NetworkID::NetworkID():data(NULL){
	}

	NetworkID::NetworkID(uint8	NID,Node::Network	description,uint8	nameSize,char	*name){

		Size=DiscoveryIDSize;
		headerSize=3+nameSize;
		switch(description){
		case	Node::PRIMARY:		Size+=CtrlIDSize[Node::PRIMARY]+DataIDSize[Node::PRIMARY]+StreamIDSize[Node::PRIMARY];	break;
		case	Node::SECONDARY:	Size+=CtrlIDSize[Node::SECONDARY]+DataIDSize[Node::SECONDARY]+StreamIDSize[Node::SECONDARY];	break;
		case	Node::BOTH:			Size+=CtrlIDSize[Node::PRIMARY]+DataIDSize[Node::PRIMARY]+StreamIDSize[Node::PRIMARY]+CtrlIDSize[Node::SECONDARY]+DataIDSize[Node::SECONDARY]+StreamIDSize[Node::SECONDARY];	break;
		}
		data=new	uint8[headerSize+Size];
		data[0]=NID;
		data[1]=description;
		data[2]=nameSize;
		memcpy(data+3,name,nameSize);
	}

	NetworkID::~NetworkID(){

		if(data)
			delete	data;
	}

	uint8	NetworkID::NID()	const{

		return	data[0];
	}

	uint8	NetworkID::setNID(uint8 newNID)	{

		data[0]=newNID;
		return 0;
	}

	Node::Network	NetworkID::network()	const{

		return	(Node::Network)data[1];
	}
	
	char	*NetworkID::name()	const{

		return	(char	*)(data+3);
	}

	uint8	*NetworkID::at(InterfaceType	t)	const{

		switch(t){
		case	DISCOVERY:				return	data+headerSize;
		case	CONTROL_PRIMARY:
			switch(data[1]){
			case	Node::BOTH:
			case	Node::PRIMARY:		return	data+headerSize+DiscoveryIDSize;
			case	Node::SECONDARY:	return	NULL;
			}
		case	DATA_PRIMARY:
			switch(data[1]){
			case	Node::BOTH:
			case	Node::PRIMARY:		return	data+headerSize+DiscoveryIDSize+CtrlIDSize[Node::PRIMARY];
			case	Node::SECONDARY:	return	NULL;
			}
		case	STREAM_PRIMARY:
			switch(data[1]){
			case	Node::BOTH:
			case	Node::PRIMARY:		return	data+headerSize+DiscoveryIDSize+CtrlIDSize[Node::PRIMARY]+DataIDSize[Node::PRIMARY];
			case	Node::SECONDARY:	return	NULL;
			}
		case	CONTROL_SECONDARY:
			switch(data[1]){
			case	Node::PRIMARY:		return	NULL;
			case	Node::SECONDARY:	return	data+headerSize+DiscoveryIDSize;
			case	Node::BOTH:			return	data+headerSize+DiscoveryIDSize+CtrlIDSize[Node::PRIMARY]+DataIDSize[Node::PRIMARY]+StreamIDSize[Node::PRIMARY];
			}
		case	DATA_SECONDARY:
			switch(data[1]){
			case	Node::PRIMARY:		return	NULL;
			case	Node::SECONDARY:	return	data+headerSize+DiscoveryIDSize+CtrlIDSize[Node::SECONDARY];
			case	Node::BOTH:			return	data+headerSize+DiscoveryIDSize+CtrlIDSize[Node::PRIMARY]+DataIDSize[Node::PRIMARY]+StreamIDSize[Node::PRIMARY]+CtrlIDSize[Node::SECONDARY];
			}
		case	STREAM_SECONDARY:
			switch(data[1]){
			case	Node::PRIMARY:		return	NULL;
			case	Node::SECONDARY:	return	data+headerSize+DiscoveryIDSize+CtrlIDSize[Node::SECONDARY]+DataIDSize[Node::SECONDARY];
			case	Node::BOTH:			return	data+headerSize+DiscoveryIDSize+CtrlIDSize[Node::PRIMARY]+DataIDSize[Node::PRIMARY]+StreamIDSize[Node::PRIMARY]+CtrlIDSize[Node::SECONDARY]+DataIDSize[Node::SECONDARY];
			}
		}
		return	NULL;
	}
}
