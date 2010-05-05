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
#include	"projection.tpl.cpp"

#include	"../Core/control_messages.h"
#include	"../Core/module_register.h"

#include	"node.h"

#define	DC	0	//	Data and Control
#define	ST	1	//	Streams

using	namespace	mBrane::sdk;

namespace	mBrane{

	thread_ret thread_function_call	RecvThread::ReceiveMessages(void	*args){

		RecvThread	*_this=(RecvThread	*)args;

		SyncEcho	*echo;
		_Payload	*p;
		while(!_this->node->_shutdown){

			uint64 t = Time::Get();
			if(_this->channel	&&	_this->channel->recv(&p)){

				_this->node->processError(_this->entry);
				// continue;
				thread_ret_val(0);
			}
			// printf("RecvThread::ReceiveMessages::recv took %uus...\n", Time::Get()-t);
			uint64 t0, t1, t2, t3;
			uint64 start = Time::Get();
			P<_Payload>	_p=p;
			p->node_recv_ts()=_this->node->time();
			switch(p->cid()){
			case	SyncEcho_CID:	//	non-ref node, compute drift
				// Time Sync description for Non-Reference Node (receiver of SyncEcho)
				// t0: SyncProbe node_send_ts (local time)
				// t1: SyncProbe node_recv_ta (remote time)
				// t2: SyncEcho node_send_ts (remote time)
				// t3: SyncEcho node_recv_ta (local time)
				// RTT:  t3-t0 - (t2-t1) // network transit time
				// timeDrift = t0 - t1 - (RTT/2)
				//  = t0 - t1 - (t3 - t0 - t2 + t1)/2
				t0 = ((SyncEcho*)p)->t0;
				t1 = ((SyncEcho*)p)->t1;
				t2 = p->node_send_ts();
				t3 = start;
				_this->node->timeDrift = t0 - t1 - (t3 - t0 - t2 + t1)/2;
				printf("*** timeDrift = %llu       RTT = %d (+ %d = %d)\n", _this->node->timeDrift, (int32)(t3 - t0 - (t2 - t1)), (int32)(t2 - t1), (int32)(t3 - t0));
			//		((SyncEcho*)p)->t0 - ((SyncEcho*)p)->t1 -
			//		   (p->node_recv_ts() - ((SyncEcho*)p)->t0 - p->node_send_ts() + ((SyncEcho*)p)->t1)/2;
				
				// Time::Get()-((SyncEcho	*)p)->time-(p->node_recv_ts()-p->node_send_ts());
				break;
			case	SyncProbe_CID:	//	ref node, echo
				// Now the sending Node is definitely up and running...
				_this->node->checkSyncProbe(((SyncProbe*)p)->node_id);
				// std::cout<<"> Info: Receiving SyncProbe from "<< ((SyncProbe*)p)->node_id <<" "<<std::endl;
				echo=new	SyncEcho();
				// echo->time=Time::Get();
				echo->t0 = p->node_send_ts();
				echo->t1 = start; // this needs local time, not adjusted time
				((_Payload*)echo)->node_send_ts()=((_Payload*)echo)->send_ts()=Time::Get();
				_this->channel->send(echo);
				delete	echo;
				break;
			default:
//				 std::cout<<"Pushing buffer["<< (unsigned int)(&_this->buffer) <<"]: "<<_p->cid()<<std::endl;fflush(stdout);
				_this->buffer.push(_p);
				break;
			}
			_p=NULL;
		}

		thread_ret_val(0);
	}

	RecvThread::RecvThread(Node	*node,CommChannel	*channel,uint16	entry):Thread(),node(node),channel(channel),entry(entry){
	}

	RecvThread::~RecvThread(){
	}

	////////////////////////////////////////////////////////////////////////////////////////////////

	thread_ret thread_function_call	PushThread::PushJobs(void	*args){

		PushThread	*_this=(PushThread	*)args;

//		std::cout<<"Starting to look for jobs ["<< (unsigned int)(&_this->source) <<"]..."<<std::endl;
		P<_Payload>	_p;
		while(_this->node->isRunning()){

//			std::cout<<"Looking for jobs ["<< (unsigned int)(&_this->source) <<"]..."<<std::endl;
			_p=_this->source->pop();
			_p->recv_ts()=_this->node->time();
//			std::cout<<"Pushing job: "<<_p->cid()<<std::endl;
			if(_p->category()==_Payload::CONTROL)
				_this->node->processControlMessage(_p);
			_this->node->pushJobs(_p);
			_p=NULL;
		}

		thread_ret_val(0);
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
		// message->send_ts()=Time::Get();
		// printf("Scheduling message (%u) for sending...\n", message->cid());
		messageOutputQueue.push(o);
	}

	void	Messaging::start(){

		sendThread=Thread::New<Thread>(SendMessages,(Node*)this);

		pushThreads[0]=new	PushThread((Node*)this,&messageInputQueue);
		pushThreads[0]->start(PushThread::PushJobs);

		//for(uint32	i=0;i<recvThreads.count();i++){

		//	pushThreads[i+1]=new	PushThread((Node*)this,&recvThreads[i]->buffer);
		//	pushThreads[i+1]->start(PushThread::PushJobs);
		//}
	}

	void	Messaging::shutdown(){

		Thread::TerminateAndWait(sendThread);
		for(uint32	i=0;i<recvThreads.count();i++)
			Thread::TerminateAndWait(*recvThreads.get(i));
		for(uint32	i=0;i<pushThreads.count();i++)
			Thread::TerminateAndWait(*pushThreads.get(i));
	}

	inline	void	Messaging::pushJobs(_Payload	*p,NodeEntry	&e){

		uint32	act;
		e.getActivation(act);
		if(act){

			List<P<ModuleEntry>,1024>			&modules=e.modules;
			
			for(List<P<ModuleEntry>,1024>::Iterator	i=modules.begin();i;++i){

				if(!(*i)->descriptor->module)
					continue;

				if((*i)->descriptor->module->isReady()	&&	(*i)->descriptor->activationCount) {
					Job j(p, (*i)->descriptor->module);
					jobs.push(j);
				}
			}
		}
	}

	inline	void	Messaging::pushJobs(_Payload	*p){

		//	find local receiving modules; insert {p,module} pairs in pipeline
		switch(p->category()){
		case	_Payload::CONTROL:
		case	_Payload::DATA:
			pushJobs(p,NodeEntry::Main[DC][p->cid()][((Node	*)this)->_ID]);
			break;
		case	_Payload::STREAM:
			pushJobs(p,NodeEntry::Main[ST][p->as_StreamData()->sid()][((Node	*)this)->_ID]);
			break;
		}
	}

	void	Messaging::processControlMessage(_Payload	*p){

		switch(p->cid()){
		case	SetThreshold_CID:
			projectionCS.enter();
			Space::Main[((SetThreshold	*)p)->host_id][((SetThreshold	*)p)->space_id]->setActivationThreshold(((SetThreshold	*)p)->threshold);
			projectionCS.leave();
			break;
		case	ActivateModule_CID:
			projectionCS.enter();
			ModuleDescriptor::Main[((ActivateModule	*)p)->host_id][((ActivateModule	*)p)->module_cid][((ActivateModule	*)p)->module_id]->setActivationLevel(((ActivateModule	*)p)->host_id,((ActivateModule	*)p)->space_id,((ActivateModule	*)p)->activationLevel);
			projectionCS.leave();
			break;
		case	ActivateSpace_CID:
			projectionCS.enter();
			Space::Main[((ActivateSpace	*)p)->host_id][((ActivateSpace	*)p)->target_sid]->setActivationLevel(((ActivateSpace	*)p)->host_id,((ActivateSpace	*)p)->space_id,((ActivateSpace	*)p)->activationLevel);
			projectionCS.leave();
			break;
		case	SubscribeMessage_CID:
			projectionCS.enter();
			ModuleDescriptor::Main[((SubscribeMessage	*)p)->host_id][((SubscribeMessage	*)p)->module_cid][((SubscribeMessage	*)p)->module_id]->addSubscription_message(((SubscribeMessage	*)p)->host_id,((SubscribeMessage	*)p)->space_id,((SubscribeMessage	*)p)->message_cid);
			projectionCS.leave();
			break;
		case	SubscribeStream_CID:
			projectionCS.enter();
			ModuleDescriptor::Main[((SubscribeMessage	*)p)->host_id][((SubscribeMessage	*)p)->module_cid][((SubscribeMessage	*)p)->module_id]->addSubscription_stream(((SubscribeMessage	*)p)->host_id,((SubscribeStream	*)p)->space_id,((SubscribeStream	*)p)->stream_id);
			projectionCS.leave();
			break;
		case	UnsubscribeMessage_CID:
			projectionCS.enter();
			ModuleDescriptor::Main[((UnsubscribeMessage	*)p)->host_id][((UnsubscribeMessage	*)p)->module_cid][((UnsubscribeMessage	*)p)->module_id]->removeSubscription_message(((UnsubscribeMessage	*)p)->host_id,((SubscribeMessage	*)p)->space_id,((SubscribeMessage	*)p)->message_cid);
			projectionCS.leave();
			break;
		case	UnsubscribeStream_CID:
			projectionCS.enter();
			ModuleDescriptor::Main[((UnsubscribeStream	*)p)->host_id][((UnsubscribeStream	*)p)->module_cid][((UnsubscribeStream	*)p)->module_id]->removeSubscription_stream(((UnsubscribeStream	*)p)->host_id,((SubscribeStream	*)p)->space_id,((SubscribeStream	*)p)->stream_id);
			projectionCS.leave();
			break;
		case	CreateModule_CID:{
			uint16	module_cid=((CreateModule	*)p)->module_cid;
			uint16	host_id=((CreateModule	*)p)->host_id;
			moduleCS.enter();
			uint16	module_id=ModuleDescriptor::GetID(host_id,module_cid);
			ModuleDescriptor::Main[host_id][module_cid][module_id]=new	ModuleDescriptor(((CreateModule	*)p)->host_id,module_cid,module_id);
			moduleCS.leave();
			break;
		}case	DeleteModule_CID:{
			uint16	module_cid=((DeleteModule	*)p)->module_cid;
			uint16	module_id=((DeleteModule	*)p)->module_id;
			uint16	host_id=((DeleteModule	*)p)->host_id;
			projectionCS.enter();
			_Module	*m=ModuleDescriptor::Main[host_id][module_cid][module_id]->module;
			ModuleDescriptor::Main[host_id][module_cid][module_id]=NULL;
			if(m) {
				Job j(new KillModule(), m);
				jobs.push(j);
			}
			projectionCS.leave();
			break;
		}case	CreateSpace_CID:
			spaceCS.enter();
			Space::Main[((CreateSpace	*)p)->host_id][Space::GetID(((CreateSpace	*)p)->host_id)]=new	Space(((CreateSpace	*)p)->host_id);
			spaceCS.leave();
			break;
		case	DeleteSpace_CID:
			projectionCS.enter();
			Space::Main[((DeleteSpace	*)p)->host_id][((DeleteSpace	*)p)->space_id]=NULL;
			projectionCS.leave();
			break;
		default:	//	call the plugin if any (e.g. rMem).
			break;
		}
	}

	////////////////////////////////////////////////////////////////////////////////////////////////

	thread_ret thread_function_call	Messaging::SendMessages(void	*args){

		Node	*node=(Node	*)args;

		MessageSlot	out;
		_Payload	*p;
		uint32		act;
		uint32		nodeCount;
		while(!node->_shutdown){

			out=node->messageOutputQueue.pop();
			p=out.p;
			p->node_send_ts()=node->time();
			_Payload::Category	cat=p->category();
			if(out.network==module::Node::LOCAL){
				
				p->node_recv_ts()=node->time();
			//	printf("Sending message (%u) locally...\n", p->cid());
				if(cat==_Payload::STREAM)
					NodeEntry::Main[ST][p->as_StreamData()->sid()][node->_ID].getActivation(act);
				else
					NodeEntry::Main[DC][p->cid()][node->_ID].getActivation(act);
				if(act)
					node->messageInputQueue.push(out.p);
			}else{

			//	printf("Sending message (%u)...\n", p->cid());
				switch(cat){
				case	_Payload::CONTROL:
			//		printf("Sending message (%u) as control...\n", p->cid());
					node->broadcastControlMessage(p,out.network);
					p->node_recv_ts()=node->time();
					node->messageInputQueue.push(out.p);
					break;
				case	_Payload::STREAM:	//	find target remote nodes; send on data/stream channels; push in messageInputQueue if the local node is a target
					{
					uint16	sid=p->as_StreamData()->sid();
					nodeCount=NodeEntry::Main[ST][sid].count();
					if (nodeCount == 0)
						printf("*** No activation for any node for stream message cid %u ***\n", p->cid());
					for(uint16	i=0;i<nodeCount;i++){

						NodeEntry::Main[ST][sid][i].getActivation(act);
						if(act){

							if(i==node->_ID) {
							//	printf("Sending message (%u) as stream locally...\n", p->cid());
								p->node_recv_ts()=node->time();
								node->messageInputQueue.push(out.p);
							}
							else {
							//	printf("Sending message (%u) as stream network...\n", p->cid());
								node->sendStreamData(i,p,out.network);
							}
						}
						else {
						//	printf("No activation for node %u for stream message (%u)...\n", i, p->cid());
						}
					}
					}break;
				case	_Payload::DATA:
					{
		//uint64 t1 = Time::Get();
					uint16	cid=p->cid();
					nodeCount=NodeEntry::Main[DC][cid].count();
					if (nodeCount == 0)
						printf("*** No activation for any node for data message cid %u ***\n", p->cid());
					for(uint16	i=0;i<nodeCount;i++){

						NodeEntry::Main[DC][cid][i].getActivation(act);
						if(act){

							if(i==node->_ID) {
							//	printf("Sending message (%u) as data locally...\n", p->cid());
								p->node_recv_ts()=node->time();
								node->messageInputQueue.push(out.p);
							}
							else {
							//	printf("Sending message (%u) as data network...\n", p->cid());
								node->sendData(i,p,out.network);
							}
						}
						else {
						//	printf("No activation for node %u for data message (%u)...\n", i, p->cid());
						}
					}
		//printf("SendMessages Send time:        %u\n", (uint32) (Time::Get() - t1));
					}
				}
			}

			out.p=NULL;
		}

		thread_ret_val(0);
	}
}
