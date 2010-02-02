#ifndef Perf_RR_h
#define Perf_RR_h


MODULE_CLASS_BEGIN(RRMaster,Module<RRMaster>)
private:
	uint64 tStart;
	uint64 tEnd;
	uint64 t1, t2;
	uint32 cRun;
	uint32 cc;
public:
	void	start(){
		tStart= 0;
		tEnd=0;
		t1=t2=0;
		cRun=0;
		cc=0;
	}
	void	stop(){	}
	template<class	T>	Decision	decide(T	*p){return	WAIT;}
	template<class	T>	void	react(T	*p){}

	void	react(SystemReady	*p){
	//	Thread::Sleep(3000);
		printf("RRMaster starting RoundRobin test, please wait...\n");
		tStart = t1 = Time::Get();
		NODE->send(this,new Ball1(0),N::PRIMARY);
	}

	void	react(Ball9 *p){
		uint32 t;
		int32 counter = p->num;
		if (counter == 1)
			printf("RoundRobin test running...\n");
		t2 = Time::Get();
		if ((t = (uint32)(t2-t1)) > 100000)
			printf("*** RR[%u] single run test took %uus for the %dth cycle, %.3fus per msg (%p)\n",
				cRun, t, counter, ((double)t)/((double)9), p);
		if (counter == 10000) {
			tEnd = t2;
			t = (uint32)(tEnd-tStart);
			printf("RR[%u] test took %uus for %d cycle, %.3fus per cycles, %.3fus per msg (%p)\n",cRun, t, counter, ((double)t)/((double)counter), ((double)t)/((double)counter*9), p);
			// OUTPUT<<"RR[" << cRun << "] test took "<<t<<"us for "<<counter<<" msgs, "<<((double)t)/((double)counter)<<"us per msg ("<<<<")"<<std::endl;
			//OUTPUT<<"Test got to '"<<counter*runCount<<"' so far..."<<std::endl;
			cRun++;
			tStart=tEnd;
			t1 = t2;
			NODE->send(this,new Ball1(0),N::PRIMARY);
		}
		else {
		//	if (counter % 1000 == 0)
		//		printf("RR[%u] %d msgs so far (%p)\n",cRun,counter, p);
		//	if (counter % 100 == 0)
		//		OUTPUT<<"RR[" << cRun << "] "<<counter<<" msgs so far..."<<std::endl;
			t1 = t2;
			NODE->send(this,new Ball1(counter+1),N::PRIMARY);
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
//		printf( "Ball1 triggered...\n");
		//	if (counter % 1000 == 0)
		//		printf("RR5 %d msgs so far (%p)\n",
		//			counter, ball);
		NODE->send(this,new Ball2(counter),N::PRIMARY);
	}
	void	react(Ball2 *p){
//		printf( "Ball2 triggered...\n");
		int32 counter = p->num;
		NODE->send(this,new Ball3(counter),N::PRIMARY);
	}
	void	react(Ball3 *p){
//		printf( "Ball3 triggered...\n");
		int32 counter = p->num;
		NODE->send(this,new Ball4(counter),N::PRIMARY);
	}
	void	react(Ball4 *p){
//		printf( "Ball4 triggered...\n");
		int32 counter = p->num;
		NODE->send(this,new Ball5(counter),N::PRIMARY);
	}
	void	react(Ball5 *p){
//		printf( "Ball5 triggered...\n");
		int32 counter = p->num;
		NODE->send(this,new Ball6(counter),N::PRIMARY);
	}
	void	react(Ball6 *p){
//		printf( "Ball6 triggered...\n");
		int32 counter = p->num;
		NODE->send(this,new Ball7(counter),N::PRIMARY);
	}
	void	react(Ball7 *p){
//		printf( "Ball7 triggered...\n");
		int32 counter = p->num;
		NODE->send(this,new Ball8(counter),N::PRIMARY);
	}
	void	react(Ball8 *p){
//		printf( "Ball8 triggered...\n");
		int32 counter = p->num;
		NODE->send(this,new Ball9(counter),N::PRIMARY);
	}
MODULE_CLASS_END(RRModule)


#endif /* Perf_RR_h */
