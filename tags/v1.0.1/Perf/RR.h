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
	void	loadParameters(const	std::vector<word32>	&numbers,const	std::vector<std::string>	&strings){
		int32	a=numbers[0];
		float32	f=*reinterpret_cast<float32	*>(&a);
		std::cout<<"RR Module got parameters: "<<f<<" "<<numbers[1]<<" "<<strings[0]<<std::endl;
	}
	void	start(){
		tStart= 0;
		tEnd=0;
		t1=t2=0;
		cRun=0;
		cc=0;

		NODE->addConstantObject(new	Shared(),"a_constant");
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
			printf("RoundRobin test %u running...\n", cRun);
		t2 = NODE->time();
	//	printf("Module 9 Send/Rec time:               %u\n", (uint32) (t2 - t1));
	//	if ((t = (uint32)(t2-t1)) > 10000)
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
//		printf( "Ball4 triggered %d...\n", p->num);
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
//		printf( "Ball5 triggered %d...\n", p->num);
		int32 counter = p->num;

		//ActivateModule* am = new ActivateModule();
		//am->host_id = 1;
		//am->module_cid = 2;
		//am->module_id = 3;
		//am->space_id = 4;
		//am->activationLevel = 5;
		//Error::PrintBinary(am, am->size(), true, "ActivateModule Message Structure");

		//Error::PrintBinary(p, p->size(), true, "Ball5 remote Message Structure");
		//Ball5* ball = new Ball5(2);
		//Error::PrintBinary(ball, ball->size(), true, "Ball5 local Message Structure");
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
//		printf( "Ball8 triggered %d...\n", p->num);
		int32 counter = p->num;
		NODE->send(this,new Ball9(counter),N::PRIMARY);
	}
MODULE_CLASS_END(RRModule)

MODULE_CLASS_BEGIN(SizeTest,Module<SizeTest>)
public:
	void	start(){ }
	void	stop(){	}
	template<class	T>	Decision	decide(T	*p){return	WAIT;}
	template<class	T>	void	react(T	*p){}

	void	react(SystemReady	*p){
		ActivateModule* am = new ActivateModule();
		am->host_id = 1;
		am->module_cid = 2;
		am->module_id = 3;
		am->space_id = 4;
		am->activationLevel = 5;
		Error::PrintBinary(am, am->size(), true, "ActivateModule Message Structure");

		Ball1* ball = new Ball1();
		ball->d1 = 1;
		ball->num = 2;
		ball->num2 = 3;
		Error::PrintBinary(ball, ball->size(), true, "Ball1 Message Structure");
	}

MODULE_CLASS_END(SizeTest)

typedef	char* (*LoomOutput)(uint32&, uint32&, uint32);
typedef	bool (*LoomInput)(uint32, char*, uint32);

MODULE_CLASS_BEGIN(Loom,Module<Loom>)
public:
	Thread* thread;
	SharedLibrary* lib;
	LoomOutput waitForEvent;
	LoomInput processEvent;

	void	start(){
		lib = SharedLibrary::New("modules/mBrane_d.dll");
		if (lib) {
			waitForEvent = lib->getFunction<LoomOutput>("WaitForEvent");
			processEvent = lib->getFunction<LoomInput>("ProcessEvent");
		}
		else {
			waitForEvent = NULL;
			processEvent = NULL;
		}
		thread = NULL;
	}
	void	stop(){
		delete(thread);
		thread = NULL;
		delete(lib);
		lib = NULL;
		waitForEvent = NULL;
		processEvent = NULL;
	}
	template<class	T>	Decision	decide(T	*p){return	WAIT;}
	template<class	T>	void	react(T	*p){}

	void	react(SystemReady	*p){
		thread = Thread::New<Thread>(run, this);
		uint32 type = 0;
		char* data = NULL;
		uint32 dataSize = 0;

		// Output data to Loom
		if (processEvent)
			processEvent(type, data, dataSize);
	}

	void	react(Ball1	*p){
		uint32 type = 0;
		char* data = NULL;
		uint32 dataSize = 0;

		// Output data to Loom
		if (processEvent) {
			processEvent(type, data, dataSize);
		}
	}

	static thread_ret thread_function_call run(void	*args) {

		Loom* _this = (Loom*) args;
		if (!_this)
			thread_ret_val(0);

		uint32 type = 0;
		char* data = NULL;
		uint32 dataSize = 0;

		while (_this->waitForEvent) {
			// wait for new input from lib function
			if ( (data = _this->waitForEvent(type, dataSize, 100)) ) {
				// if there, post output
			}
		}

		thread_ret_val(0);
	}

MODULE_CLASS_END(Loom)


#endif /* Perf_RR_h */
