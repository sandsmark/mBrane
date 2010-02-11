#ifndef Perf_RR_h
#define Perf_RR_h

uint64 t1;

MODULE_CLASS_BEGIN(RRMaster,Module<RRMaster>)
private:
	uint64 tStart;
	uint64 tEnd;
	uint64 t2;
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
		tStart = t1 = NODE->time();
		NODE->send(this,new Ball1(0),N::PRIMARY);
	}

	void	react(Ball9 *p){
		uint32 t;
		int32 counter = p->num;
		if (counter == 1)
			printf("RoundRobin test running...\n");
		t2 = NODE->time();
	//	printf("Module 9 Send/Rec time:               %u\n", (uint32) (t2 - t1));
	//	if ((t = (uint32)(t2-t1)) > 100000)
	//		printf("*** RR[%u] single run test took %uus for the %dth cycle, %.3fus per msg (%p)\n",
	//			cRun, t, counter, ((double)t)/((double)9), p);
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
		uint64 now = NODE->time();
		//printf("Module Rec time (%d,%d,%d,%d):                             1   %u\n",
		//	(uint32) (now - p->send_ts()),
		//	(uint32) (now - p->node_send_ts()),
		//	(uint32) (now - p->node_recv_ts()),
		//	(uint32) (now - p->recv_ts()),
		//	(uint32) (now - t1));
		int32 counter = p->num;
//		printf( "Ball1 triggered...\n");
		//	if (counter % 1000 == 0)
		//		printf("RR5 %d msgs so far (%p)\n",
		//			counter, ball);
		NODE->send(this,new Ball2(counter),N::PRIMARY);
	}
	void	react(Ball2 *p){
		uint64 now = NODE->time();
		//printf("Module Rec time (%d,%d,%d,%d):                             2   %u\n",
		//	(uint32) (now - p->send_ts()),
		//	(uint32) (now - p->node_send_ts()),
		//	(uint32) (now - p->node_recv_ts()),
		//	(uint32) (now - p->recv_ts()),
		//	(uint32) (now - t1));
//		printf( "Ball2 triggered...\n");
		int32 counter = p->num;
		NODE->send(this,new Ball3(counter),N::PRIMARY);
	}
	void	react(Ball3 *p){
		uint64 now = NODE->time();
		//printf("Module Rec time (%d,%d,%d,%d):                             3   %u\n",
		//	(uint32) (now - p->send_ts()),
		//	(uint32) (now - p->node_send_ts()),
		//	(uint32) (now - p->node_recv_ts()),
		//	(uint32) (now - p->recv_ts()),
		//	(uint32) (now - t1));
//		printf( "Ball3 triggered...\n");
		int32 counter = p->num;
		NODE->send(this,new Ball4(counter),N::PRIMARY);
	}
	void	react(Ball4 *p){
		uint64 now = NODE->time();
		//printf("Module Rec time (%d,%d,%d,%d):                             4   %u\n",
		//	(uint32) (now - p->send_ts()),
		//	(uint32) (now - p->node_send_ts()),
		//	(uint32) (now - p->node_recv_ts()),
		//	(uint32) (now - p->recv_ts()),
		//	(uint32) (now - t1));
//		printf( "Ball4 triggered...\n");
		int32 counter = p->num;
		NODE->send(this,new Ball5(counter),N::PRIMARY);
	}
	void	react(Ball5 *p){
		uint64 now = NODE->time();
		//printf("Module Rec time (%d,%d,%d,%d):                             5   %u\n",
		//	(uint32) (now - p->send_ts()),
		//	(uint32) (now - p->node_send_ts()),
		//	(uint32) (now - p->node_recv_ts()),
		//	(uint32) (now - p->recv_ts()),
		//	(uint32) (now - t1));
//		printf( "Ball5 triggered...\n");
		int32 counter = p->num;
		NODE->send(this,new Ball6(counter),N::PRIMARY);
	}
	void	react(Ball6 *p){
		uint64 now = NODE->time();
		//printf("Module Rec time (%d,%d,%d,%d):                             6   %u\n",
		//	(uint32) (now - p->send_ts()),
		//	(uint32) (now - p->node_send_ts()),
		//	(uint32) (now - p->node_recv_ts()),
		//	(uint32) (now - p->recv_ts()),
		//	(uint32) (now - t1));
		int32 counter = p->num;
		NODE->send(this,new Ball7(counter),N::PRIMARY);
	}
	void	react(Ball7 *p){
		uint64 now = NODE->time();
		//printf("Module Rec time (%d,%d,%d,%d):                             7   %u\n",
		//	(uint32) (now - p->send_ts()),
		//	(uint32) (now - p->node_send_ts()),
		//	(uint32) (now - p->node_recv_ts()),
		//	(uint32) (now - p->recv_ts()),
		//	(uint32) (now - t1));
//		printf( "Ball7 triggered...\n");
		int32 counter = p->num;
		NODE->send(this,new Ball8(counter),N::PRIMARY);
	}
	void	react(Ball8 *p){
		uint64 now = NODE->time();
		//printf("Module Rec time (%d,%d,%d,%d):                             8   %u\n",
		//	(uint32) (now - p->send_ts()),
		//	(uint32) (now - p->node_send_ts()),
		//	(uint32) (now - p->node_recv_ts()),
		//	(uint32) (now - p->recv_ts()),
		//	(uint32) (now - t1));
//		printf( "Ball8 triggered...\n");
		int32 counter = p->num;
		NODE->send(this,new Ball9(counter),N::PRIMARY);
	}
MODULE_CLASS_END(RRModule)


#endif /* Perf_RR_h */
