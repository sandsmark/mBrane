#ifndef Perf_modules_h
#define Perf_modules_h

#include	"Perf.h"
#include	"../Core/module_node.h"

#define	N		module::Node
#define	NODE	module::Node::Get()
#define	OUTPUT	NODE->trace(N::APPLICATION)

int64 startTime = 0;
int64 endTime = 0;
int32 runCount = 0;

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
		OUTPUT<<"ping "<<_id<<" got another message..."<<std::endl;
	}
	void	react(SystemReady	*p){
		OUTPUT<<"ping starting PingPong test, please wait..."<<std::endl;
		startTime = Time::Get();
		NODE->send(this,new Ball(0),N::LOCAL);
	}
	void	react(ReturnBall	*p){
		int counter = p->id;
		//OUTPUT<<"Test got to '"<<counter<<"' so far..."<<std::endl;
		if (counter == 500) {
			runCount++;
			if (runCount >= 200) {
				endTime = Time::Get();
				uint32 t = (uint32)(endTime-startTime);
				uint32 c = (uint32)(counter*runCount);
				OUTPUT<<"PingPong test took "<<t<<"us for "<<c<<" msgs, "<<((double)t)/((double)c)<<"us per msg"<<std::endl;
			}
			else {
				//OUTPUT<<"Test got to '"<<counter*runCount<<"' so far..."<<std::endl;
				counter = 0;
				NODE->send(this,new Ball(counter),N::LOCAL);
			}
		}
		else {
			NODE->send(this,new Ball(counter+1),N::LOCAL);
		}
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
		OUTPUT<<"pong "<<_id<<" got another message..."<<std::endl;
	}
	void	react(SystemReady	*p){
	//	OUTPUT<<"pong got SystemReady"<<std::endl;
	}
	void	react(Ball	*p){
	//	OUTPUT<<"pong "<<_id<<" got the ball '"<<p->id<<"'"<<std::endl;
		NODE->send(this,new ReturnBall(p->id),N::LOCAL);
	}
MODULE_CLASS_END(pong)


#endif /* Perf_modules_h */
