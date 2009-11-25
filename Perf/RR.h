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
		Ball9 *ball = p;
		int32 counter = ball->num;
		if (counter == 10000) {
			tEnd = Time::Get();
			uint32 t = (uint32)(tEnd-tStart);
			printf("RR[%u] test took %uus for %d msgs, %.3fus per msg (%p)\n",
				cRun, t, counter, ((double)t)/((double)counter), ball);
			// OUTPUT<<"RR[" << cRun << "] test took "<<t<<"us for "<<counter<<" msgs, "<<((double)t)/((double)counter)<<"us per msg ("<<<<")"<<std::endl;
			//OUTPUT<<"Test got to '"<<counter*runCount<<"' so far..."<<std::endl;
			cRun++;
			NODE->send(this,new Ball1(0),N::LOCAL);
		}
		else {
			if (counter % 1000 == 0)
				printf("RR[%u] %d msgs so far (%p)\n",
					cRun, counter, ball);
		//	if (counter % 100 == 0)
		//		OUTPUT<<"RR[" << cRun << "] "<<counter<<" msgs so far..."<<std::endl;
			NODE->send(this,new Ball1(counter+1),N::LOCAL);
		}
		ball = NULL;
	}
MODULE_CLASS_END(RRMaster)

MODULE_CLASS_BEGIN(RRModule,Module<RRModule>)
	void	start(){}
	void	stop(){	}
	template<class	T>	Decision	decide(T	*p){return	WAIT;}
	template<class	T>	void	react(T	*p){}

	void	react(Ball1 *p){
		P<Ball1>	ball = p;
		int32 counter = ball->num;
		//	if (counter % 1000 == 0)
		//		printf("RR5 %d msgs so far (%p)\n",
		//			counter, ball);
		NODE->send(this,new Ball2(counter),N::LOCAL);
		ball = NULL;
	}
	void	react(Ball2 *p){
		P<Ball2>	ball = p;
		int32 counter = ball->num;
		NODE->send(this,new Ball3(counter),N::LOCAL);
		ball = NULL;
	}
	void	react(Ball3 *p){
		P<Ball3>	ball = p;
		int32 counter = ball->num;
		NODE->send(this,new Ball4(counter),N::LOCAL);
		ball = NULL;
	}
	void	react(Ball4 *p){
		P<Ball4>	ball = p;
		int32 counter = ball->num;
		NODE->send(this,new Ball5(counter),N::LOCAL);
		ball = NULL;
	}
	void	react(Ball5 *p){
		P<Ball5>	ball = p;
		int32 counter = ball->num;
		NODE->send(this,new Ball6(counter),N::LOCAL);
		ball = NULL;
	}
	void	react(Ball6 *p){
		P<Ball6>	ball = p;
		int32 counter = ball->num;
		NODE->send(this,new Ball7(counter),N::LOCAL);
		ball = NULL;
	}
	void	react(Ball7 *p){
		P<Ball7>	ball = p;
		int32 counter = ball->num;
		NODE->send(this,new Ball8(counter),N::LOCAL);
		ball = NULL;
	}
	void	react(Ball8 *p){
		P<Ball8>	ball = p;
		int32 counter = ball->num;
		NODE->send(this,new Ball9(counter),N::LOCAL);
		ball = NULL;
	}
MODULE_CLASS_END(RRModule)


#endif /* Perf_RR_h */
