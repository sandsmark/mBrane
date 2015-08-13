//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
//_/_/
//_/_/ HUMANOBS - mBrane - PingPong
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

#ifndef pingpong_modules_h
#define pingpong_modules_h

#include	"pingpong.h"
#include	"../Core/module_node.h"

#define	N		module::Node
#define	NODE	module::Node::Get()
#define	OUTPUT	NODE->trace(N::APPLICATION)

class	pong;
MODULE_CLASS_BEGIN(ping,Module<ping>)
	void	start(){
	}
	void	stop(){
	}
	template<class	T>	Decision	decide(T	*p){
		return	WAIT;
	}
	template<class	T>	void	react(T	*p){	//	to messages
		OUTPUT<<"ping got a message..."<<std::endl;
	}
	void	react(SystemReady	*p){
		OUTPUT<<"ping got SystemReady"<<std::endl;
/*		NODE->send(this,new Ball(0),N::LOCAL);
		NODE->deleteModule(1,0);
		NODE->send(this,new Ball(1),N::LOCAL);
		NODE->newModule(this,Module<pong>::CID());							//	module_id=0; ping does not catch notifications on new modules, so we have to know the new module's id, see below.
		NODE->activateModule(this,Module<pong>::CID(),0,1,1.0);				//	the correct way to do that is to have it done in the new module: this solves the issue above, at the expense of removing the initial subscriptions in the configuration file.
		NODE->subscribeMessage(this,Module<pong>::CID(),0,1,CLASS_ID(Ball));//	idem.
		NODE->activateSpace(this,0,1,0);
		NODE->activateSpace(this,0,1,1);
		NODE->send(this,new Ball(2),N::LOCAL);
		NODE->newSpace(this);												//	space_id=3.
		NODE->activateSpace(this,0,3,1);
		NODE->setSpaceThreshold(this,3,0.5);
		NODE->newModule(this,Module<pong>::CID());							//	module_id=1.
		NODE->send(this,new Ball(3),N::LOCAL);
		NODE->subscribeMessage(this,Module<pong>::CID(),1,3,CLASS_ID(Ball));
		NODE->unsubscribeMessage(this,Module<pong>::CID(),1,3,CLASS_ID(Ball));
		NODE->subscribeMessage(this,Module<pong>::CID(),1,3,CLASS_ID(Ball));
		NODE->activateModule(this,Module<pong>::CID(),1,3,1.0);
		NODE->send(this,new Ball(4),N::LOCAL);
		NODE->deleteSpace(3);
		NODE->send(this,new Ball(5),N::LOCAL);
		NODE->unsubscribeMessage(this,Module<pong>::CID(),0,1,CLASS_ID(Ball));
*/
		NODE->send(this,new Type2(),N::LOCAL);
	}
	void	react(Ball	*p){
		OUTPUT<<"ping got the ball "<<p->id<<std::endl;
	}
	void	react(Type1	*p){
		static	uint32_t i=0;
		static	uint64_t _begin=Time::Get();
		if(++i==10){
			uint64_t delta=Time::Get()-_begin;
			float	t=((float)delta)/1000000; // convert from us to ms (1000) per message (1000)
			std::cout<<"------------ "<<t<<std::endl;
			return;
		}
		OUTPUT<<"ping got Type1"<<std::endl;
		NODE->send(this,new Type2(),N::LOCAL);
	}
	void	react(Type2	*p){
		OUTPUT<<"ping got Type2"<<std::endl;
	}
	template<class	T>	void	react(uint16_t sid,T	*p){	//	to stream data
		OUTPUT<<"ping got data on stream "<<sid<<std::endl;
	}
MODULE_CLASS_END(ping)

MODULE_CLASS_BEGIN(pong,Module<pong>)
	void	start(){
	}
	void	stop(){
	}
	template<class	T>	Decision	decide(T	*p){
		return	WAIT;
	}
	template<class	T>	void	react(T	*p){	//	to messages
		OUTPUT<<"pong "<<_id<<" got a message..."<<std::endl;
	}
	void	react(SystemReady	*p){
		OUTPUT<<"pong got SystemReady"<<std::endl;
	}
	void	react(Ball	*p){
		OUTPUT<<"pong "<<_id<<" got the ball "<<p->id<<std::endl;
	}
	void	react(Type1	*p){
		OUTPUT<<"pong got Type1"<<std::endl;
	}
	void	react(Type2	*p){
		OUTPUT<<"pong got Type2"<<std::endl;
		NODE->send(this,new Type1(),N::LOCAL);
	}
	template<class	T>	void	react(uint16_t sid,T	*p){	//	to stream data
		OUTPUT<<"pong got data on stream "<<sid<<std::endl;
	}
MODULE_CLASS_END(pong)


#endif /* pingpong_modules_h */
