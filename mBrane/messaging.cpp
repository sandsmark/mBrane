//	messaging.cpp
//
//	Author: Eric Nivel
//
//	BSD license:
//	Copyright (c) 2008, Eric Nivel
//	All rights reserved.
//	Redistribution and use in source and binary forms, with or without
//	modification, are permitted provided that the following conditions are met:
//
//   - Redistributions of source code must retain the above copyright
//     notice, this list of conditions and the following disclaimer.
//   - Redistributions in binary form must reproduce the above copyright
//     notice, this list of conditions and the following disclaimer in the
//     documentation and/or other materials provided with the distribution.
//   - Neither the name of Eric Nivel nor the
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

#include	"messaging.h"

#include	"..\Core\control_messages.h"
#include	"..\Core\module_register.h"

#include	"node.h"

#define	DC	0	//	Data and Control
#define	ST	1	//	Streams

using	namespace	mBrane::sdk;

namespace	mBrane{

	uint32	thread_function_call	RecvThread::ReceiveMessages(void	*args){

		RecvThread	*_this=(RecvThread	*)args;

		SyncEcho	*echo;
		_Payload	*p;
		while(!_this->node->_shutdown){

			if(_this->channel	&&	_this->channel->recv(&p)){

				_this->node->processError(_this->entry);
				continue;
			}

			P<_Payload>	_p=p;
			switch(p->cid()){
			case	SyncEcho_CID:	//	non-ref node, compute drift
				_this->node->timeDrift=Time::Get()-((SyncEcho	*)p)->time-(p->node_recv_ts()-p->node_send_ts());
				break;
			case	SyncProbe_CID:	//	ref node, echo
				echo=new	SyncEcho();
				echo->time=Time::Get();
				_this->channel->send(echo);
				delete	echo;
				break;
			default:
				_this->buffer.push(_p);
				_p=NULL;
				break;
			}
		}

		return	0;
	}

	RecvThread::RecvThread(Node	*node,CommChannel	*channel,uint16	entry):Thread(),node(node),channel(channel),entry(entry){
	}

	RecvThread::~RecvThread(){
	}

	////////////////////////////////////////////////////////////////////////////////////////////////

	uint32	thread_function_call	PushThread::PushJobs(void	*args){

		PushThread	*_this=(PushThread	*)args;

		P<_Payload>	*_p;
		while(!_this->node->_shutdown){

			_p=_this->source->pop();
			if((*_p)->category()==_Payload::CONTROL)
				_this->node->processControlMessage(*_p);
			_this->node->pushJobs(*_p);
			*_p=NULL;
		}

		return	0;
	}

	PushThread::PushThread(Node	*node,Pipe11<P<_Payload>,MESSAGE_OUTPUT_BLOCK_SIZE>	*source):Thread(),node(node),source(source){
	}

	PushThread::~PushThread(){
	}

	////////////////////////////////////////////////////////////////////////////////////////////////

	Messaging::Messaging():sendThread(NULL){
	}

	Messaging::~Messaging(){

		for(uint32	i=0;i<recvThreads.count();i++){

			if(recvThreads[i]){

				delete	recvThreads[i];
				recvThreads[i]=NULL;
			}
		}

		if(sendThread)
			delete	sendThread;
		sendThread=NULL;

		for(uint32	i=0;i<pushThreads.count();i++){

			if(pushThreads[i]){

				delete	pushThreads[i];
				pushThreads[i]=NULL;
			}
		}
	}

	void	Messaging::send(_Payload	*message,module::Node::Network	network){

		MessageSlot	o;
		o.p=message;
		o.network=network;
		message->send_ts()=Time::Get();
		messageOutputQueue.push(o);
	}

	void	Messaging::start(){

		sendThread=Thread::New<Thread>(SendMessages,(Node*)this);

		pushThreads[0]=new	PushThread((Node*)this,&messageInputQueue);
		pushThreads[0]->start(PushThread::PushJobs);
		for(uint32	i=0;i<recvThreads.count();i++){

			pushThreads[i+1]=new	PushThread((Node*)this,&recvThreads[i]->buffer);
			pushThreads[i+1]->start(PushThread::PushJobs);
		}
	}

	void	Messaging::shutdown(){

		Thread::Wait(sendThread);
		for(uint32	i=0;i<recvThreads.count();i++)
			Thread::Wait(*recvThreads.get(i));
		for(uint32	i=0;i<pushThreads.count();i++)
			Thread::Wait(*pushThreads.get(i));
	}

	void	Messaging::processControlMessage(_Payload	*p){

		switch(p->cid()){
		case	SetThreshold_CID:
			NodeEntry::CS[DC].enter();
			NodeEntry::CS[ST].enter();
			Space::Main[((SetThreshold	*)p)->space_id]->setActivationThreshold(((SetThreshold	*)p)->threshold);
			NodeEntry::CS[DC].leave();
			NodeEntry::CS[ST].leave();
			break;
		case	ActivateModule_CID:
			NodeEntry::CS[DC].enter();
			NodeEntry::CS[ST].enter();
			ModuleDescriptor::Main[((ActivateModule	*)p)->module_cid][((ActivateModule	*)p)->module_id]->setActivationLevel(((ActivateModule	*)p)->space_id,((ActivateModule	*)p)->activationLevel);
			NodeEntry::CS[DC].leave();
			NodeEntry::CS[ST].leave();
			break;
		case	ActivateSpace_CID:
			NodeEntry::CS[DC].enter();
			NodeEntry::CS[ST].enter();
			Space::Main[((ActivateSpace	*)p)->target_sid]->setActivationLevel(((ActivateSpace	*)p)->space_id,((ActivateSpace	*)p)->activationLevel);
			NodeEntry::CS[DC].leave();
			NodeEntry::CS[ST].leave();
			break;
		case	SubscribeMessage_CID:
			NodeEntry::CS[DC].enter();
			ModuleDescriptor::Main[((SubscribeMessage	*)p)->module_cid][((SubscribeMessage	*)p)->module_id]->addSubscription_message(((SubscribeMessage	*)p)->space_id,((SubscribeMessage	*)p)->message_cid);
			NodeEntry::CS[DC].leave();
			break;
		case	SubscribeStream_CID:
			NodeEntry::CS[ST].enter();
			ModuleDescriptor::Main[((SubscribeMessage	*)p)->module_cid][((SubscribeMessage	*)p)->module_id]->addSubscription_stream(((SubscribeStream	*)p)->space_id,((SubscribeStream	*)p)->stream_id);
			NodeEntry::CS[ST].leave();
			break;
		case	UnsubscribeMessage_CID:
			NodeEntry::CS[DC].enter();
			ModuleDescriptor::Main[((UnsubscribeMessage	*)p)->module_cid][((UnsubscribeMessage	*)p)->module_id]->removeSubscription_message(((SubscribeMessage	*)p)->space_id,((SubscribeMessage	*)p)->message_cid);
			NodeEntry::CS[DC].leave();
			break;
		case	UnsubscribeStream_CID:
			NodeEntry::CS[ST].enter();
			ModuleDescriptor::Main[((UnsubscribeStream	*)p)->module_cid][((UnsubscribeStream	*)p)->module_id]->removeSubscription_stream(((SubscribeStream	*)p)->space_id,((SubscribeStream	*)p)->stream_id);
			NodeEntry::CS[ST].leave();
			break;
		case	CreateModule_CID:{
			NodeEntry::CS[DC].enter();
			NodeEntry::CS[ST].enter();
			uint16	module_cid=((CreateModule	*)p)->module_cid;
			uint16	module_id=ModuleDescriptor::GetID(module_cid);
			ModuleDescriptor::Main[module_cid][module_id]=new	ModuleDescriptor(((CreateModule	*)p)->node_id,module_cid,module_id);
			NodeEntry::CS[DC].leave();
			NodeEntry::CS[ST].leave();
			break;
		}case	DeleteModule_CID:{
			NodeEntry::CS[DC].enter();
			NodeEntry::CS[ST].enter();
			uint16	module_cid=((DeleteModule	*)p)->module_cid;
			uint16	module_id=((DeleteModule	*)p)->module_id;
			_Module	*m=ModuleDescriptor::Main[module_cid][module_id]->module;
			if(m)
				jobs.push(Job(new	KillModule(),m));
			ModuleDescriptor::Main[module_cid][module_id]=NULL;
			NodeEntry::CS[DC].leave();
			NodeEntry::CS[ST].leave();
			break;
		}case	CreateSpace_CID:{
			NodeEntry::CS[DC].enter();
			NodeEntry::CS[ST].enter();
			Space::Main[Space::GetID()]=new	Space();
			NodeEntry::CS[DC].leave();
			NodeEntry::CS[ST].leave();
			break;
		}case	DeleteSpace_CID:{
			NodeEntry::CS[DC].enter();
			NodeEntry::CS[ST].enter();
			uint16	space_id=((DeleteSpace	*)p)->space_id;
			Space::Main[space_id]=NULL;
			NodeEntry::CS[DC].leave();
			NodeEntry::CS[ST].leave();
			break;
		}default:
			break;
		}
	}

	inline	void	Messaging::pushJobs(_Payload	*p,NodeEntry	&e){

		if(e.activationCount){

			List<P<ModuleEntry>,1024>			&modules=e.modules;
			List<P<ModuleEntry>,1024>::Iterator	i;
			for(i=modules.begin();i;++i){

				if(!(*i)->descriptor->module)
					continue;

				if((*i)->descriptor->module->isReady()	&&	(*i)->descriptor->activationCount)
					jobs.push(Job(p,(*i)->descriptor->module));
			}
		}
	}

	inline	void	Messaging::pushJobs(_Payload	*p){

		//	find local receiving modules; insert {p,module} pairs in pipeline
		switch(p->category()){
		case	_Payload::CONTROL:
		case	_Payload::DATA:
			NodeEntry::CS[DC].enter();
			pushJobs(p,NodeEntry::Main[DC][p->cid()][((Node	*)this)->_ID]);
			NodeEntry::CS[DC].leave();
			break;
		case	_Payload::STREAM:
			NodeEntry::CS[ST].enter();
			pushJobs(p,NodeEntry::Main[ST][p->operator	_StreamData	*()->sid()][((Node	*)this)->_ID]);
			NodeEntry::CS[ST].leave();
			break;
		}
	}

	////////////////////////////////////////////////////////////////////////////////////////////////

	uint32	thread_function_call	Messaging::SendMessages(void	*args){

		Node	*node=(Node	*)args;

		MessageSlot	*out;
		_Payload	*p;
		P<_Payload>	_p;
		while(!node->_shutdown){

			out=node->messageOutputQueue.pop();
			p=out->p;
			_p=p;
			_Payload::Category	cat=p->category();
			if(out->network==module::Node::LOCAL){
				
				uint32	act;
				if(cat==_Payload::STREAM){

					NodeEntry::CS[ST].enter();
					act=NodeEntry::Main[ST][p->operator	_StreamData	*()->sid()][node->_ID].activationCount;
					NodeEntry::CS[ST].leave();
				}else{

					NodeEntry::CS[DC].enter();
					act=NodeEntry::Main[DC][p->cid()][node->_ID].activationCount;
					NodeEntry::CS[DC].leave();
				}
				if(act)
					node->messageInputQueue.push(_p);
			}else{

				if(cat==_Payload::CONTROL){

					node->broadcastControlMessage(p,out->network);
					node->messageInputQueue.push(_p);
				}else	if(cat==_Payload::STREAM){	//	find target remote nodes; send on data/stream channels; push in messageInputQueue if the local node is a target

					uint16	sid=p->operator	_StreamData	*()->sid();
					NodeEntry::CS[ST].enter();
					for(uint16	i=0;i<NodeEntry::Main[ST][sid].count();i++){

						if(NodeEntry::Main[DC][sid][i].activationCount){

							if(i==node->_ID)
								node->messageInputQueue.push(_p);
							else	
								node->sendStreamData(i,p,out->network);
						}
					}
					NodeEntry::CS[ST].leave();
				}else{

					uint16	cid=p->cid();
					NodeEntry::CS[DC].enter();
					for(uint16	i=0;i<NodeEntry::Main[DC][cid].count();i++){

						if(NodeEntry::Main[DC][cid][i].activationCount){

							if(i==node->_ID)
								node->messageInputQueue.push(_p);
							else	
								node->sendData(i,p,out->network);
						}
					}
					NodeEntry::CS[DC].leave();
				}
			}

			out->p=NULL;
			_p=NULL;
		}

		return	0;
	}
}