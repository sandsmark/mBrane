#ifndef Perf_modules_h
#define Perf_modules_h

#include	"Perf.h"
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
		NODE->send(this,new Ball(0),N::LOCAL);
/*		NODE->deleteModule(1,0);
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
		static	uint32	i=0;
		static	uint64	_begin=Time::Get();
		if(++i==1000){
			uint64	delta=Time::Get()-_begin;
			float32	t=((float32)delta)/1000000;
			std::cout<<"------------ "<<t<<std::endl;
			return;
		}
		OUTPUT<<"ping got Type1"<<std::endl;
		NODE->send(this,new Type2(),N::LOCAL);
	}
	void	react(Type2	*p){
		OUTPUT<<"ping got Type2"<<std::endl;
	}
	template<class	T>	void	react(uint16	sid,T	*p){	//	to stream data
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
	template<class	T>	void	react(uint16	sid,T	*p){	//	to stream data
		OUTPUT<<"pong got data on stream "<<sid<<std::endl;
	}
MODULE_CLASS_END(pong)


#endif /* Perf_modules_h */
