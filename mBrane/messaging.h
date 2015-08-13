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

#ifndef	mBrane_messaging_h
#define	mBrane_messaging_h

#define	MESSAGE_INPUT_BLOCK_SIZE	64
#define	MESSAGE_OUTPUT_BLOCK_SIZE	64
#define	JOBS_BLOCK_SIZE				128

#include	"mBrane.h"
#include	"pipe.h"
#include	"../Core/list.h"
#include	"../Core/control_messages.h"

#include	"module_descriptor.h"

using	namespace	mBrane::sdk;
using	namespace	mBrane::sdk::module;

namespace	mBrane{

	typedef	struct	_Job{
		P<_Payload>	p;
		_Module		*m;
		_Job(_Payload	*p=NULL,_Module		*m=NULL):m(m){this->p=p;}
	}Job;

	class	Node;

	class	RecvThread:
	public	Thread{
	public:
		static	thread_ret thread_function_call	ReceiveMessages(void	*args);
		Pipe11<P<_Payload>,MESSAGE_INPUT_BLOCK_SIZE>	buffer;	//	incoming messages from remote nodes
		Node			*node;
		CommChannel		*channel;
		uint8_t 		sourceNID;
		RecvThread(Node	*node,CommChannel	*channel,uint8_t sourceNID);
		~RecvThread();
	};

	class	PushThread:
	public	Thread{
	public:
		static	thread_ret thread_function_call	PushJobs(void	*args);
		Node	*const	node;
		Pipe11<P<_Payload>,MESSAGE_OUTPUT_BLOCK_SIZE>	*source;
		PushThread(Node	*node,Pipe11<P<_Payload>,MESSAGE_OUTPUT_BLOCK_SIZE>	*source);
		~PushThread();
	};

	class	GarbageCollector:
	public	Thread{
	private:
		Timer	timer;
	public:
		static	thread_ret thread_function_call	Run(void	*args);
		Node	*const	node;
		GarbageCollector(Node	*node);
		~GarbageCollector();
	};

	class	Executing;
	class	XThread;
	class	mbrane_dll	Messaging{
	friend	class	RecvThread;
	friend	class	PushThread;
	friend	class	XThread;
	friend	class	Executing;
	friend	class	GarbageCollector;
	private:
		CriticalSection	moduleCS;	//	control access to spaces, modules descriptors and projections in processControlMessages.
		CriticalSection	spaceCS;
		CriticalSection	projectionCS;
	protected:
		typedef	struct{
			module::Node::Network	network;
			uint8_t 				destinationNode;	//	0xFF means unspecified.
			P<_Payload>				p;
		}MessageSlot;
		Pipe11<P<_Payload>,MESSAGE_INPUT_BLOCK_SIZE>	messageInputQueue;	//	incoming local messages
		PipeN1<MessageSlot,MESSAGE_OUTPUT_BLOCK_SIZE>	messageOutputQueue;	//	outgoing messages

		PipeNN<Job,JOBS_BLOCK_SIZE>	jobs;

		Array<RecvThread	*,MESSAGE_INPUT_BLOCK_SIZE>	recvThreads;
		Thread											*sendThread;
		static	thread_ret thread_function_call			SendMessages(void	*args);
		Array<PushThread	*,MESSAGE_INPUT_BLOCK_SIZE>	pushThreads;	//	one for each message source (recvThread->buffer plus input queue); push jobs in the job pipe
		
		//TODO: add a pointer to processControlMessage plugin here.

		//	Cache.
		class	ConstantEntry{
		public:
			std::string		name;
			P<__Payload>	object;
			ConstantEntry(){}
			ConstantEntry(std::string	name,__Payload	*object):name(name),object(object){}
		};
		std::vector<ConstantEntry>			constants;	//	indexed by the IDs.
		UNORDERED_MAP<uint32,P<_Payload> >	cache;		//	shared objects residing on the node (local if they have been sent at least once, and foreign); indexed by full IDs.
		std::vector<UNORDERED_SET<uint32> >	lookup;		//	shared objects's full IDs known as being held by remote nodes; vector indexed by NIDs.
		CriticalSection						cacheCS;	//	concurrency: Messaging::processControlMessage, CommChannel::send, CommChannel::recv.

		//	Deletion handling.
		uint8_t 					pendingAck;				//	number of ack to wait for.
		UNORDERED_SET<_Payload	*>	pendingDeletions[2];	//	2-buffered list of objects to be deleted. Any object in here is smart pointed by the cache, and its ref count is 1.
		uint8_t 					pendingDeletions_GC;	//	index for access from GC::Run.
		uint8_t 					pendingDeletions_SO;	//	index for access from SharedObject::decRef.
		CriticalSection				pendingDeletionsCS;		//	concurrency GarbageCollector::Run, CommChannel::recv.
		uint32_t 					GCPeriod;				//	at which the GC kicks in; in ms.
		GarbageCollector			*GC;

		Messaging();
		~Messaging();

		bool	loadConfig(XMLNode	&n);

		void	start();
		void	shutdown();
		void	send(_Payload	*message,module::Node::Network	network);
		void	send(_Payload	*message,uint8_t nodeID,module::Node::Network	network);
		void	pushJobs(_Payload	*p,NodeEntry	&e);
		void	pushJobs(_Payload	*p);
		void	processControlMessage(_Payload	*p);
	};
}

#include	"networking.h"

#endif
