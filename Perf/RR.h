#ifndef Perf_RR_h
#define Perf_RR_h


MODULE_CLASS_BEGIN(RRMaster,Module<RRMaster>)
private:
	uint64 tStart;
	uint64 tEnd;
	uint32 cRun;
	uint32 cc;
public:
	void	start(){
		tStart= 0;
		tEnd=0;
		cRun=0;
		cc=0;
	}
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
			tStart=tEnd;
			printf("RR[%u] test took %uus for %d msgs, %.3fus per msg (%p)\n",cRun, t, counter, ((double)t)/((double)counter), p);
			// OUTPUT<<"RR[" << cRun << "] test took "<<t<<"us for "<<counter<<" msgs, "<<((double)t)/((double)counter)<<"us per msg ("<<<<")"<<std::endl;
			//OUTPUT<<"Test got to '"<<counter*runCount<<"' so far..."<<std::endl;
			cRun++;
			NODE->send(this,new Ball1(0),N::LOCAL);
		}
		else {
		//	if (counter % 1000 == 0)
		//		printf("RR[%u] %d msgs so far (%p)\n",cRun,counter, p);
		//	if (counter % 100 == 0)
		//		OUTPUT<<"RR[" << cRun << "] "<<counter<<" msgs so far..."<<std::endl;
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
		//	if (counter % 1000 == 0)
		//		printf("RR5 %d msgs so far (%p)\n",
		//			counter, ball);
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
