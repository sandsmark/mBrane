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

#ifndef	mBrane_module_descriptor_h
#define	mBrane_module_descriptor_h

#include	"../Core/list.h"
#include	"../Core/module.h"
#include	"xml_parser.h"

#include	"projection.h"


using	namespace	mBrane::sdk;
using	namespace	mBrane::sdk::module;

namespace	mBrane{

	class	NodeEntry;
	class	ModuleEntry:
	public	Object<Memory,_Object,ModuleEntry>{
	public:
		NodeEntry			*node;
		ModuleDescriptor	*descriptor;
		ModuleEntry(NodeEntry	*n,ModuleDescriptor	*m);
		~ModuleEntry();
	};

	class	NodeEntry:
	public	CriticalSection{	//	guards against Messaging::SendMessages.
	private:
		uint32_t activationCount;
	public:
		static	Array<Array<NodeEntry,32>,128>	Main[2];	//	0: Data and Control: message class -> nodes -> modules, 1: Streams: stream id -> nodes -> modules
		NodeEntry();
		~NodeEntry();
		void	incActivation(){	enter();activationCount++;leave();	}
		void	decActivation(){	enter();activationCount--;leave();	}
		void	getActivation(uint32_t &a){	enter();a=activationCount;leave();	}
		List<P<ModuleEntry>,1024>	modules;
	};

	template<>	class	Projection<ModuleDescriptor>:
	public	_Projection<ModuleDescriptor,Projection<ModuleDescriptor> >{
	public:
		Array<List<P<ModuleEntry>,1024>::Iterator,128>	subscriptions[2];		//	0: indexed by message class ID (MCID), 1: indexed by stream ID (SID)
		uint16_t 										subscriptionCount[2];	//	idem
		Projection(ModuleDescriptor	*projected,Space	*space);
		~Projection();
		void	activate();
		void	deactivate();
		void	setActivationLevel(float	a);
		void	updateActivationCount(float	t);
		void	addSubscription(uint8_t payloadType,uint16_t ID,List<P<ModuleEntry>,1024>::Iterator	i);
		void	removeSubscription(uint8_t payloadType,uint16_t ID);
	};

	//	Module proxy.
	class	ModuleDescriptor:
	public	Projectable<ModuleDescriptor>{
	private:
		class	_Subscription{
		public:
			uint16_t MCID;
			uint16_t SID;
		};
		class	_Projection{
		public:
			uint16_t 				spaceID;
			float					activationLevel;
			Array<_Subscription,8>	subscriptions;
		};
		Array<_Projection,32>	initialProjections;
		const	char	*name;
	public:
		uint16_t CID;
		static	Array<Array<P<ModuleDescriptor>,128>,32>			Config;	//	indexed by module descriptor class ID | ID; temporary: used at config time when node IDs are not known; tranfered in Node::start in Main.
		static	Array<Array<Array<P<ModuleDescriptor>,128>,32>,8>	Main;	//	indexed by host ID | module descriptor class ID | ID.
		static	ModuleDescriptor									*New(XMLNode	&n);
		static	void												Init(uint8_t hostID);	//	resolves host name into ID, copies Config in Main, apply initial projections.
		static	uint16_t 											GetID(uint8_t hostID,uint16_t CID);	//	returns the first available slot in Main[hostID][CID].
		static	const char*											GetName(uint16_t cid, uint16_t id);	//	returns the name of CID.
		Host::host_name	hostName;	//	resolved in hostID at Node::run() time
		P<_Module>	module;	//	NULL if remote
		//_Module	*module;
		ModuleDescriptor(const	char	*hostName,_Module	*m,uint16_t CID,const	char	*name);	//	invoked at Node::loadApplication() time.
		ModuleDescriptor(uint8_t hostID,uint16_t CID,uint16_t ID);									//	invoked dynamically.
		~ModuleDescriptor();
		void	applyInitialProjections(uint8_t hostID);
		const	char	*getName();
		void	_activate();
		void	_deactivate();
		void	addSubscription_message(uint8_t hostID,uint16_t spaceID,uint16_t MCID);
		void	addSubscription_stream(uint8_t hostID,uint16_t spaceID,uint16_t SID);
		void	removeSubscription_message(uint8_t hostID,uint16_t spaceID,uint16_t MCID);
		void	removeSubscription_stream(uint8_t hostID,uint16_t spaceID,uint16_t SID);
		void	removeSubscriptions_message(uint8_t hostID,uint16_t spaceID);
		void	removeSubscriptions_stream(uint8_t hostID,uint16_t spaceID);
	};
}


#endif
