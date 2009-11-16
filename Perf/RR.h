#ifndef Perf_RR_h
#define Perf_RR_h

uint64 tStart = 0;
uint64 tEnd = 0;
uint32 cRun = 0;
uint32 cc = 0;

MODULE_CLASS_BEGIN(RRMaster,Module<RRMaster>)
	void	start(){}
	void	stop(){	}
	template<class	T>	Decision	decide(T	*p){return	WAIT;}
	template<class	T>	void	react(T	*p){}

	void	react(SystemReady	*p){
		OUTPUT<<"RRMaster starting RoundRobin test, please wait..."<<std::endl;
		tStart = Time::Get();
		NODE->send(this,new Ball1(0),N::LOCAL);
	}

	void	react(Ball9 *p){
		int32 counter = p->num;
		if (counter == 10000) {
			tEnd = Time::Get();
			uint32 t = (uint32)(tEnd-tStart);
			OUTPUT<<"RR[" << cRun << "] test took "<<t<<"us for "<<counter<<" msgs, "<<((double)t)/((double)counter)<<"us per msg"<<std::endl;
			//OUTPUT<<"Test got to '"<<counter*runCount<<"' so far..."<<std::endl;
			cRun++;
			counter = 0;
			NODE->send(this,new Ball1(0),N::LOCAL);
		}
		else {
			NODE->send(this,new Ball1(counter+1),N::LOCAL);
		}
	}
MODULE_CLASS_END(RRMaster)

MODULE_CLASS_BEGIN(RRModule,Module<RRModule>)
	void	start(){}
	void	stop(){	}
	template<class	T>	Decision	decide(T	*p){return	WAIT;}
	template<class	T>	void	react(T	*p){}

	void	react(Ball1 *p){
		int32 counter = p->num;
		NODE->send(this,new Ball2(counter),N::LOCAL);
	}
	void	react(Ball2 *p){
		int32 counter = p->num;
		NODE->send(this,new Ball3(counter),N::LOCAL);
	}
	void	react(Ball3 *p){
		int32 counter = p->num;
		NODE->send(this,new Ball4(counter),N::LOCAL);
	}
	void	react(Ball4 *p){
		int32 counter = p->num;
		NODE->send(this,new Ball5(counter),N::LOCAL);
	}
	void	react(Ball5 *p){
		int32 counter = p->num;
		NODE->send(this,new Ball6(counter),N::LOCAL);
	}
	void	react(Ball6 *p){
		int32 counter = p->num;
		NODE->send(this,new Ball7(counter),N::LOCAL);
	}
	void	react(Ball7 *p){
		int32 counter = p->num;
		NODE->send(this,new Ball8(counter),N::LOCAL);
	}
	void	react(Ball8 *p){
		int32 counter = p->num;
		NODE->send(this,new Ball9(counter),N::LOCAL);
	}
MODULE_CLASS_END(RRModule)


#endif /* Perf_RR_h */
