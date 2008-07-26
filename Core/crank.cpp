// crank.cpp
//
// Author: Eric Nivel
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

#include	"crank.h"
#include	"node.h"

#define	CRANK_INPUT_QUEUE_SIZE	512

namespace	mBrane{
	namespace	sdk{

		void	_Crank::Build(uint16	CID){

			Node::Get()->buildCrank(CID);
		}

		_Crank::_Crank(uint16	_ID):CircularBuffer<P<_Payload> >(CRANK_INPUT_QUEUE_SIZE),_ID(_ID){
		}

		_Crank::~_Crank(){
		}

		inline	uint16	_Crank::id()	const{

			return	_ID;
		}

		inline	void	_Crank::send(_Message	*m){

			Node::Get()->send(_ID,m);
		}

		inline	void	_Crank::send(_ControlMessage	*m){

			Node::Get()->send(_ID,m);
		}

		inline	void	_Crank::send(_StreamData	*m){

			Node::Get()->send(_ID,m);
		}

		inline	int64	_Crank::time(){

			return	Node::Get()->time();
		}

		inline	void	_Crank::peek(int32	depth){

			Iterator	i;
			uint32	d=0;
			for(i=begin();i!=end()	&&	d<depth;i++,d++){

				_Payload	*p=(_Payload	*)(P<_Payload>)i;
				if(!p)
					continue;
				if(notify(p,true))
					((P<_Payload>)i)=NULL;
			}
		}
	}
}
