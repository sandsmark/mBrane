//	module.cpp
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

#include	"module.h"
#include	"module_node.h"
#include	"utils.h"


namespace	mBrane{
	namespace	sdk{
		namespace	module{

			void	_Module::New(uint16	CID,uint16	ID,uint16	clusterCID,uint16	clusterID){

				Node::Get()->buildModule(CID,ID,clusterCID,clusterID);
			}

			_Module::_Module():	_Object(),_priority(0),processor(NULL){
			}

			_Module::~_Module(){
			}

			inline	uint16	_Module::cid()	const{

				return	_CID;
			}

			inline	uint16	_Module::id()	const{

				return	_ID;
			}

			inline	uint16	_Module::cluster_cid()	const{

				return	_clusterCID;
			}

			inline	uint16	_Module::cluster_id()	const{

				return	_clusterID;
			}

			inline	uint8	&_Module::priority(){

				return	_priority;
			}

			inline	bool	_Module::canMigrate(){

				return	_canMigrate;
			}

			inline	bool	_Module::canBeSwapped(){

				return	_canBeSwapped;
			}

			uint32	_Module::dumpSize(){

				return	0;
			}

			_Payload	*_Module::dump(){

				return	NULL;
			}

			void	_Module::load(_Payload	*chunk){
			}

			void	_Module::start(){
			}

			void	_Module::stop(){
			}

			inline	void	_Module::swapOut(){
			}

			inline	void	_Module::swapIn(){
			}

			inline	void	_Module::migrateOut(){
			}

			inline	void	_Module::migrateIn(){
			}

			inline	int64	_Module::time()	const{

				return	Node::Get()->time();
			}

			void	_Module::sleep(int64	d){

				if(processor	&&	d)
					processor->block();
				Thread::Sleep(d);
			}

			void	_Module::wait(Thread	**threads,uint32	threadCount){

				if(processor)
					processor->block();
				Thread::Wait(threads,threadCount);
			}
			
			void	_Module::wait(Thread	*_thread){

				if(processor)
					processor->block();
				Thread::Wait(_thread);
			}
			
			inline	void	_Module::send(_Payload	*p)	const{

				Node::Get()->send(this,p);
			}
		}
	}
}
