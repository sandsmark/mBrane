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

#ifndef	mBrane_sdk_network_interface_h
#define	mBrane_sdk_network_interface_h

#include	"utils.h"
#include	"xml_parser.h"
#include	"mdaemon_node.h"


namespace	mBrane{
	namespace	sdk{

		class	CommChannel;
		class	ConnectedCommChannel;
		class	mBrane_dll	NetworkInterface{
		public:
			typedef	enum{
				UDP=0,
				TCP=1,
				RM=2,
				IB=3	//	MPI2
			}Protocol;
		protected:
			Protocol	_protocol;
			NetworkInterface(Protocol	_protocol);
		public:
			typedef	NetworkInterface	*(*Load)(XMLNode	&,mdaemon::Node	*n);	//	function exported by the shared library
			virtual	~NetworkInterface();
			Protocol	protocol()	const;
			virtual	bool	operator	==(NetworkInterface	&i)=0;
			virtual	bool	operator	!=(NetworkInterface	&i)=0;
			virtual	bool	canBroadcast()=0;	//	as opposed to connected mode
			virtual	uint16_t start()=0;	//	initialize the network interface; returns 0 if successful
			virtual	uint16_t stop()=0;	//	the network interface; returns 0 if successful
			virtual	uint16_t getIDSize()=0;	//	node net ID to be broadcast
			virtual	void	fillID(uint8_t *ID)=0;	//	with relevant parameters (different from Node::_ID; ex: IP addr and port)
			virtual	uint16_t newChannel(uint8_t *ID,CommChannel	**channel)=0;	//	create a new channel (bcast ID=local node net ID, or connected ID=remote node net ID); returns 0 if successful
			virtual	uint16_t acceptConnection(ConnectedCommChannel	**channel,int32_t timeout,bool	&timedout)=0;	//	listen to connect attempts and creates a new channel accordingly; returns 0 if successful
		};

		class	_Payload;
		class	mBrane_dll	CommChannel{
		protected:
			CommChannel();	//	initialization to be performed in subclasses' constructors
			int16_t _send(__Payload	*c,uint8_t destinationNID);
			int16_t _recv(__Payload	**c,uint8_t sourceNID);
			CriticalSection	commSendCS;
			CriticalSection	commRecvCS;
			uint8* sendBuffer;
			uint32 sendBufferLen;
			uint32 sendBufferPos;
		public:
			virtual	~CommChannel();	//	shutdown to be performed in subclasses' destructors
			virtual	int16_t send(uint8_t *b,size_t	s)=0;	//	return 0 if successfull, error code (>0) otherwise
			virtual	int16_t recv(uint8_t *b,size_t	s,bool	peek=false)=0;
			virtual bool	isConnected() = 0;
			virtual bool	disconnect() = 0;
			int16_t bufferedSend(uint8 *b,size_t s, bool sendNow = false);
			int16_t send(_Payload	*p,uint8_t destinationNID);	//	return 0 if successfull, error code (>0) otherwise. destinationNID used only for connected comm channels.
			int16_t recv(_Payload	**p,uint8_t sourceNID);
		};

		class	mBrane_dll	ConnectedCommChannel:
		public	CommChannel{
		protected:
			ConnectedCommChannel();
		public:
			virtual	~ConnectedCommChannel();
		};

		class	mBrane_dll	BroadcastCommChannel:
		public	CommChannel{
		protected:
			BroadcastCommChannel();
		public:
			virtual	~BroadcastCommChannel();
		};
	}
}


#endif
