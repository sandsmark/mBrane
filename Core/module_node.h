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

#ifndef	mBrane_sdk_module_node_h
#define	mBrane_sdk_module_node_h

#include	"payload.h"
#include	<iostream>

namespace	mBrane{
	namespace	sdk{

		namespace	module{

			class	_Module;

			//	Node API, as seen from the module
			class	mBrane_dll	Node{
			private:
				static	Node	*Singleton;
			protected:
				static	std::ostream	*Streams[3];
                                uint8_t	_ID;	//	max: 0x7F
                                Node(uint8_t	ID);
				~Node();
			public:
                                static	const	uint8_t	NoID=0xFF;
				typedef	enum{
					PRIMARY=0,
					SECONDARY=1,
					BOTH=2,
					LOCAL=3,
					EITHER=4
				}Network;
				typedef	enum{
					EXECUTION=0,
					NETWORK=1,
					APPLICATION=2
				}TraceLevel;
				static	Node	*Get();
				static	std::ostream&	trace(TraceLevel	l);
                                uint8_t	id()	const;
				virtual	const	char	*name()=0;
				virtual	void	send(const	_Module	*sender,_Payload	*p,Network	network=PRIMARY)=0;
                                virtual	void	send(const	_Module	*sender,_Payload	*message, uint8_t	nodeID,Network	network=PRIMARY)=0;
                                virtual	uint64_t	time()	const=0;	//	in us since 01/01/70
				virtual	void	newSpace(const	_Module	*sender,Network	network=PRIMARY)=0;	//	names are meaningless for dynamic instances
                                virtual	void	newModule(const	_Module	*sender,uint16_t	CID,Network	network=PRIMARY,const	char	*hostName="local")=0;
                                virtual	void	deleteSpace(uint16_t	ID,Network	network=PRIMARY)=0;
                                virtual	void	deleteModule(uint16_t	CID,uint16_t	ID,Network	network=PRIMARY)=0;
                                virtual	void	activateModule(const	_Module	*sender,uint16_t	module_cid,uint16_t	module_id,uint16_t	space_id,float	activationLevel,Network	network=PRIMARY)=0;
                                virtual	void	activateSpace(const	_Module	*sender,uint16_t	space_id,uint16_t	target_sid,float	activationLevel,Network	network=PRIMARY)=0;
                                virtual	void	setSpaceThreshold(const	_Module	*sender,uint16_t	space_id,float	threshold,Network	network=PRIMARY)=0;
                                virtual	void	subscribeMessage(const	_Module	*sender,uint16_t	module_cid,uint16_t	module_id,uint16_t	space_id,uint16_t	message_cid,Network	network=PRIMARY)=0;
                                virtual	void	unsubscribeMessage(const	_Module	*sender,uint16_t	module_cid,uint16_t	module_id,uint16_t	space_id,uint16_t	message_cid,Network	network=PRIMARY)=0;
                                virtual	void	subscribeStream(const	_Module	*sender,uint16_t	module_cid,uint16_t	module_id,uint16_t	space_id,uint16_t	stream_id,Network	network=PRIMARY)=0;
                                virtual	void	unsubscribeStream(const	_Module	*sender,uint16_t	module_cid,uint16_t	module_id,uint16_t	space_id,uint16_t	stream_id,Network	network=PRIMARY)=0;
                                virtual	const	char	*getSpaceName(uint16_t	hostID,uint16_t	ID)=0;
                                virtual	const	char	*getModuleName(uint16_t	CID)=0;
                                virtual uint8_t	getNodeID(const char* name)=0;
				//	Interface for caching.
				virtual	void		markUnused(_Payload	*p)=0;
				virtual	void		addConstantObject(_Payload	*c,const	std::string	&name)=0;
                                virtual	_Payload	*getConstantObject(uint32_t	OID)=0;
				virtual	_Payload	*getConstantObject(const	std::string	&name)=0;
                                virtual	void		addLookup(uint8_t	sourceNID,uint32_t	OID)=0;
                                virtual	bool		hasLookup(uint8_t	destinationNID,uint32_t	OID)=0;
				virtual	void		addSharedObject(_Payload	*p)=0;
                                virtual	_Payload	*getSharedObject(uint32_t	OID)=0;
				virtual	void		consolidate(_Payload	*p)=0;
			};
		}
	}
}


#endif
