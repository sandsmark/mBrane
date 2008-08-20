//	messaging.tpl.cpp
//
//	Author: Eric Nivel
//
//	BSD license:
//	Copyright (c) 2008, Eric Nivel, Thor List
//	All rights reserved.
//	Redistribution and use in source and binary forms, with or without
//	modification, are permitted provided that the following conditions are met:
//
//   - Redistributions of source code must retain the above copyright
//     notice, this list of conditions and the following disclaimer.
//   - Redistributions in binary form must reproduce the above copyright
//     notice, this list of conditions and the following disclaimer in the
//     documentation and/or other materials provided with the distribution.
//   - Neither the name of Eric Nivel, Thor List nor the
//     names of their contributors may be used to endorse or promote products
//     derived from this software without specific prior written permission.
//
//	THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND ANY
//	EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
//	WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
//	DISCLAIMED. IN NO EVENT SHALL THE REGENTS AND CONTRIBUTORS BE LIABLE FOR ANY
//	DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
//	(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
//	LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
//	ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//	(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
//	SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#define	INITIAL_NID_ARRAY_LENGTH	16
#define	INITIAL_SID_ARRAY_LENGTH	16

#define	DC	0	//	Data and Control
#define	ST	1	//	Streams

using	namespace	mBrane::sdk::payloads;

namespace	mBrane{

	template<class	Engine>	Messaging<Engine>::Messaging():Engine(),sendThread(NULL){

		inputSync=new	Semaphore(0,65535);

		routes[DC].alloc(ClassRegister::Count());
		for(uint32	i=0;i<routes[DC].count();i++)
			routes[DC][i].alloc(INITIAL_NID_ARRAY_LENGTH);

		routes[ST].alloc(INITIAL_SID_ARRAY_LENGTH);
		for(uint32	i=0;i<routes[ST].count();i++)
			routes[ST][i].alloc(INITIAL_NID_ARRAY_LENGTH);
	}

	template<class	Engine>	Messaging<Engine>::~Messaging(){

		for(uint32	i=0;i<recvThreads.count();i++){

			if(recvThreads[i])
				delete	recvThreads[i];
		}
		if(sendThread)
			delete	sendThread;
		delete	inputSync;

		for(uint32	i=0;i<routes[DC].count();i++)
			for(uint32	j=0;j<routes[DC][i].count();j++)
				delete	routes[DC][i][j].modules;

		for(uint32	i=0;i<routes[ST].count();i++)
			for(uint32	j=0;j<routes[ST][i].count();j++)
				delete	routes[ST][i][j].modules;
	}
	
	template<class	Engine>	inline	void	Messaging<Engine>::send(uint16	NID,const	_Module	*sender,_Payload	*message,bool	local){

		message->send_ts()=Time::Get();
		if(message->category()==_Payload::DATA){

			_Message	*_m=message->operator	_Message	*();
			_m->senderModule_cid()=sender->cid();
			_m->senderModule_iid()=sender->id();
			_m->senderCluster_cid()=sender->cluster_cid();
			_m->senderCluster_iid()=sender->cluster_id();
		}
		OutputSlot	o;
		o.p=message;
		o.local=local;
		messageOutputQueue.push(o);
	}

	template<class	Engine>	inline	void	Messaging<Engine>::send(uint16	NID,_Payload	*message,bool	local){

		message->send_ts()=Time::Get();
		OutputSlot	o;
		o.p=message;
		o.local=local;
		messageOutputQueue.push(o);
	}

	template<class	Engine>	void	Messaging<Engine>::start(){

		sendThread=Thread::New<Thread>(SendMessages,this);
		Engine::start();
	}

	template<class	Engine>	void	Messaging<Engine>::shutdown(){

		Thread::Wait(sendThread);
		Thread::Wait((Thread	**)recvThreads.data(),recvThreads.count());
		Thread::Wait(jobFeeder);
		Engine::shutdown();
	}

	template<class	Engine>	void	Messaging<Engine>::processControlMessage(_Payload	*p){

		switch(p->cid()){
		//	TODO:	case	xxx_CID:
		default:
			break;
		}
	}

	template<class	Engine>	void	Messaging<Engine>::pushJobs(_Payload	*p,NodeEntry	&e){

		if(e.activationCount){

			Job	j;
			List<_Module	*>				*modules=e.modules;
			List<_Module	*>::Iterator	i;
			for(i=modules->begin();i!=modules->end();i++){

				if(((_Module	*)i)->active()){

					j.p=p;
					j.c=(_Module	*)i;
					jobs.push(j);
				}
			}
		}
	}

	template<class	Engine>	void	Messaging<Engine>::pushJobs(_Payload	*p){

		//	find local receiving modules; insert {p,module} pairs in pipeline
		switch(p->category()){
		case	_Payload::CONTROL:
		case	_Payload::DATA:
			routesCS[DC].enter();
			pushJobs(p,routes[ST][p->cid()][((Node	*)this)->_ID]);
			routesCS[DC].leave();
		case	_Payload::STREAM:
			routesCS[ST].enter();
			pushJobs(p,routes[ST][p->operator	_StreamData	*()->sid()][((Node	*)this)->_ID]);
			routesCS[ST].leave();
		}
	}

	////////////////////////////////////////////////////////////////////////////////////////////////

	template<class	Engine>	uint32	thread_function_call	Messaging<Engine>::SendMessages(void	*args){

		Node	*node=(Node	*)args;

		uint16	r;

		OutputSlot	*o;
		_Payload	*p;
		P<_Payload>	_p;
		while(!node->_shutdown){

			o=node->messageOutputQueue.pop();
			p=o->p;
			_p=p;
			_Payload::Category	cat=p->category();
			if(o->local){
				
				uint32	act;
				if(cat==_Payload::STREAM){

					node->routesCS[ST].enter();
					act=node->routes[ST][p->operator	_StreamData	*()->sid()][node->_ID].activationCount;
					node->routesCS[ST].leave();
				}else{

					node->routesCS[DC].enter();
					act=node->routes[DC][p->cid()][node->_ID].activationCount;
					node->routesCS[DC].leave();
				}
				if(act){

					node->messageInputQueue.push(_p);
					node->inputSync->release();
				}
			}else	if(cat==_Payload::CONTROL){

				if(node->network==PRIMARY	||	node->network==BOTH)
					node->broadcastControlMessage(PRIMARY,p);
				if(node->network==SECONDARY	||	node->network==BOTH)
					node->broadcastControlMessage(SECONDARY,p);
			}else{
				//	find target remote nodes; send on data/stream channels; push in messageInputQueue if the local node is a target
				if(cat==_Payload::DATA){

					uint16	cid=p->cid();
					node->routesCS[DC].enter();
					for(uint32	i=0;i<node->routes[DC][cid].count();i++){

						if(node->routes[DC][cid][i].activationCount){

							if(i==node->_ID){

								node->messageInputQueue.push(_p);
								node->inputSync->release();
							}else	node->sendData(i,p);
						}
					}
					node->routesCS[DC].leave();
				}else{	//	must be stream

					uint16	sid=p->operator	_StreamData	*()->sid();
					node->routesCS[ST].enter();
					for(uint32	i=0;i<node->routes[ST][sid].count();i++){

						if(node->routes[DC][sid][i].activationCount){

							if(i==node->_ID){

								node->messageInputQueue.push(_p);
								node->inputSync->release();
							}else	node->sendStreamData(i,p);
						}
					}
					node->routesCS[ST].leave();
				}
			}

			if(node->isTimeReference)
				node->lastSyncTime=p->node_send_ts();

			o->p=NULL;
		}

		return	0;
	}
}