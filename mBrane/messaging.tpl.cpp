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

#define	INITIAL_MESSAGING_LATENCY	1000

using	namespace	mBrane::sdk::payloads;

namespace	mBrane{

	template<class	Engine>	Messaging<Engine>::Messaging():Engine(),sendThread(NULL){

		inputSync=new	Semaphore(0,65535);
	}

	template<class	Engine>	Messaging<Engine>::~Messaging(){

		for(uint32	i=0;i<recvThreads.count();i++){

			if(recvThreads[i])
				delete	recvThreads[i];
		}
		if(sendThread)
			delete	sendThread;
		delete	inputSync;
	}
	
	template<class	Engine>	inline	void	Messaging<Engine>::send(uint16	NID,const	_Crank	*sender,_Payload	*message,bool	local){

		message->send_ts()=Time::Get();
		((_ControlMessage	*)message)->senderNode_id()=NID;
		//	TODO:	if message, fill in sender info
		OutputSlot	o;
		o.p=message;
		o.local=local;
		messageOutputQueue.push(o);
	}

	template<class	Engine>	inline	void	Messaging<Engine>::send(uint16	NID,_Payload	*message,bool	local){

		message->send_ts()=Time::Get();
		((_ControlMessage	*)message)->senderNode_id()=NID;
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

	template<class	Engine>	void	Messaging<Engine>::pushJobs(_Payload	*p){

		//	find local receiving cranks (from pub-sub structure); insert {p,crank} pairs in pipeline
		Array<PublishingSubscribing::NodeEntry>	*nodeEntries=((Node	*)this)->getNodeEntries(p->cid(),((_ControlMessage	*)p)->mid());
		if(nodeEntries){	//	else: mid has never been subscribed for before

			((Node	*)this)->routesCS.enter();
			if(nodeEntries->operator[](((Node	*)this)->_ID).activationCount){

				List<_Crank	*>	*l=nodeEntries->get(((Node	*)this)->_ID)->cranks;
				if(l){

					Job	j;
					List<_Crank	*>::Iterator	i;
					for(i=l->begin();i!=l->end();i++){

						if(((_Crank	*)i)->active()){

							j.p=p;
							j.c=(_Crank	*)i;
							jobs.push(j);
						}
					}
				}
			}
			((Node	*)this)->routesCS.leave();
		}
	}

	////////////////////////////////////////////////////////////////////////////////////////////////

	template<class	Engine>	uint32	thread_function_call	Messaging<Engine>::SendMessages(void	*args){

		Node	*node=(Node	*)args;

		uint16	r;

		OutputSlot	*o;
		_Payload	*p;
		while(!node->_shutdown){

			o=node->messageOutputQueue.pop();
			p=o->p;
			if(o->local){

				Array<PublishingSubscribing::NodeEntry>	*nodeEntries=node->getNodeEntries(p->cid(),((_ControlMessage	*)p)->mid());
				if(nodeEntries){	//	else: mid has never been subscribed for before

					P<_Payload>	_p=p;
					node->routesCS.enter();
					uint32	act=nodeEntries->get(node->_ID)->activationCount;
					node->routesCS.leave();
					if(act){

						node->messageInputQueue.push(_p);
						node->inputSync->release();
					}
				}
			}else	if(p->isControlMessage()){

				for(uint16	i=0;i<node->controlChannels.count();i++){

					if(node->controlChannels[i]	&&	node->controlChannels[i]->send(p))
						node->processError(Networking::CONTROL,i);
				}
			}else{

				//	find target remote nodes; send on data/stream channels; push in messageInputQueue if the local node is a target
				Array<PublishingSubscribing::NodeEntry>	*nodeEntries=node->getNodeEntries(p->cid(),((_ControlMessage	*)p)->mid());
				if(nodeEntries){	//	else: mid has never been subscribed for before

					P<_Payload>	_p=p;
					node->routesCS.enter();
					if(p->isMessage()){

						for(uint32	i=0;i<nodeEntries->count();i++){

							if(nodeEntries->get(i)->activationCount){

								if(i==node->_ID){

									node->messageInputQueue.push(_p);
									node->inputSync->release();
								}else	if(r=node->dataChannels[i]->data->send(p))
									node->processError(Networking::DATA,i);
							}
						}
					}else{	//	must be stream

						for(uint32	i=0;i<nodeEntries->count();i++){

							if(nodeEntries->get(i)->activationCount){

								if(i==node->_ID){

									node->messageInputQueue.push(_p);
									node->inputSync->release();
								}else	if(r=node->dataChannels[i]->stream->send(p))
									node->processError(Networking::STREAM,i);
							}
						}
					}
					node->routesCS.leave();
				}
			}

			if(node->isTimeReference)
				node->lastSyncTime=p->node_send_ts();

			o->p=NULL;
		}

		return	0;
	}
}