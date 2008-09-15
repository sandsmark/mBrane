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

#include	"node.h"
#include	"..\Core\control_messages.h"


using	namespace	mBrane::sdk::payloads;

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
				break;
			default:
				_this->buffer.push(_p);
				_this->node->inputSync->release();
				break;
			}
		}

		return	0;
	}

	RecvThread::RecvThread(Node	*node,CommChannel	*channel,uint16	entry):Thread(),node(node),channel(channel),entry(entry){
	}

	RecvThread::~RecvThread(){
	}
}