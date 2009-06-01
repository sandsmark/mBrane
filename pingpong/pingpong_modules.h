#ifndef pingpong_modules_h
#define pingpong_modules_h

#include	"pingpong.h"
#include	"../core/module_node.h"

#define	N		module::Node
#define	NODE	module::Node::Get()
#define	OUT		NODE->trace(N::APPLICATION)

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
		OUT<<"ping got a message...\n";
	}
	void	react(SystemReady	*p){
		OUT<<"ping got SystemReady\n";
		NODE->send(this,new Ball(0),N::LOCAL);
		NODE->deleteModule(1,0);
		NODE->send(this,new Ball(1),N::LOCAL);
		NODE->newModule(this,Module<pong>::CID());							//	module_id=0; ping does not catch notifications on new modules, so we have to know the new module's id, see below.
		NODE->activateModule(this,Module<pong>::CID(),0,1,1.0);				//	the correct way to do that is to have it done in the new module: this solves the issue above, at the expense of removing the initial subscriptions in the configuration file.
		NODE->subscribeMessage(this,Module<pong>::CID(),0,1,CLASS_ID(Ball));//	idem.
		NODE->activateSpace(this,0,1,0);
		NODE->activateSpace(this,0,1,1);
		NODE->send(this,new Ball(2),N::LOCAL);
		NODE->send(this,new Type1(),N::LOCAL);
		NODE->newSpace(this);												//	space_id=3.
		NODE->activateSpace(this,0,3,1);
		NODE->setSpaceThreshold(this,3,0.5);
		NODE->newModule(this,Module<pong>::CID());							//	module_id=1.
		NODE->subscribeMessage(this,Module<pong>::CID(),1,3,CLASS_ID(Ball));
		NODE->unsubscribeMessage(this,Module<pong>::CID(),1,3,CLASS_ID(Ball));
		NODE->subscribeMessage(this,Module<pong>::CID(),1,3,CLASS_ID(Ball));
		NODE->activateModule(this,Module<pong>::CID(),1,3,1.0);
		NODE->send(this,new Ball(3),N::LOCAL);
		NODE->deleteSpace(3);
		NODE->send(this,new Ball(4),N::LOCAL);
	}
	void	react(Ball	*p){
		OUT<<"ping got the ball "<<p->id<<"\n";
	}
	void	react(Type1	*p){
		OUT<<"ping got Type1\n";
	}
	void	react(Type2	*p){
		OUT<<"ping got Type2\n";
	}
	template<class	T>	void	react(uint16	sid,T	*p){	//	to stream data
		OUT<<"ping got data on stream "<<sid<<std::endl;
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
		OUT<<"pong "<<_id<<" got a message...\n";
	}
	void	react(SystemReady	*p){
		OUT<<"pong got SystemReady\n";
	}
	void	react(Ball	*p){
		OUT<<"pong "<<_id<<" got the ball "<<p->id<<"\n";
	}
	void	react(Type1	*p){
		OUT<<"pong got Type1\n";
	}
	void	react(Type2	*p){
		OUT<<"pong got Type2\n";
	}
	template<class	T>	void	react(uint16	sid,T	*p){	//	to stream data
		OUT<<"pong got data on stream "<<sid<<std::endl;
	}
MODULE_CLASS_END(pong)


#endif /* pingpong_modules_h */