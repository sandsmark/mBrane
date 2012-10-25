//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/ //_/_/ //_/_/  RR.h //_/_/ //_/_/  Eric Nivel //_/_/  Center for Analysis and Design of Intelligent Agents //_/_/    Reykjavik University, Menntavegur 1, 101 Reykjavik, Iceland //_/_/    http://cadia.ru.is
//_/_/  Copyright©2012 //_/_/ //_/_/  This software was developed by the above copyright holder as part of  //_/_/  the HUMANOBS EU research project, in collaboration with the  //_/_/  following parties:
//_/_/   //_/_/  Autonomous Systems Laboratory //_/_/    Technical University of Madrid, Spain //_/_/    http://www.aslab.org/ //_/_/ //_/_/  Communicative Machines //_/_/    Edinburgh, United Kingdom //_/_/    http://www.cmlabs.com/ //_/_/ //_/_/  Istituto Dalle Molle di Studi sull'Intelligenza Artificiale //_/_/    University of Lugano and SUPSI, Switzerland //_/_/    http://www.idsia.ch/ //_/_/ //_/_/  Institute of Cognitive Sciences and Technologies //_/_/    Consiglio Nazionale delle Ricerche, Italy //_/_/    http://www.istc.cnr.it/
//_/_/
//_/_/  Dipartimento di Ingegneria Informatica
//_/_/    University of Palermo, Italy
//_/_/    http://roboticslab.dinfo.unipa.it/index.php/Main/HomePage
//_/_/ //_/_/
//_/_/  --- HUMANOBS Open-Source BSD License, with CADIA Clause v 1.0 ---
//_/_/
//_/_/  Redistribution and use in source and binary forms, with or without 
//_/_/  modification, is permitted provided that the following conditions 
//_/_/  are met:
//_/_/
//_/_/  - Redistributions of source code must retain the above copyright 
//_/_/    and collaboration notice, this list of conditions and the 
//_/_/    following disclaimer.
//_/_/
//_/_/  - Redistributions in binary form must reproduce the above copyright 
//_/_/    notice, this list of conditions and the following
//_/_/    disclaimer in the documentation and/or other materials provided 
//_/_/    with the distribution.
//_/_/
//_/_/  - Neither the name of its copyright holders nor the names of its 
//_/_/    contributors may be used to endorse or promote products 
//_/_/    derived from this software without specific prior written permission.
//_/_/
//_/_/  - CADIA Clause: The license granted in and to the software under this 
//_/_/     agreement is a limited-use license. The software may not be used in 
//_/_/     furtherance of: 
//_/_/     (i) intentionally causing bodily injury or severe emotional distress 
//_/_/         to any person; 
//_/_/     (ii) invading the personal privacy or violating the human rights of 
//_/_/         any person; or 
//_/_/     (iii) committing  or preparing for any act of war.
//_/_/
//_/_/  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
//_/_/  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
//_/_/  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR 
//_/_/  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
//_/_/  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
//_/_/  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
//_/_/  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, 
//_/_/  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY 
//_/_/  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
//_/_/  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
//_/_/  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//_/_/
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

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

typedef	char* (*LoomOutput)(uint32);
typedef	bool (*LoomInput)(char*);

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
		char* data = NULL;

		// Output data to Loom
		if (processEvent)
			processEvent(data);
	}

	void	react(Ball1	*p){
		char* data = NULL;

		// Output data to Loom
		if (processEvent) {
			processEvent(data);
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
			if ( (data = _this->waitForEvent(100)) ) {
				// if there, post output
			}
		}

		thread_ret_val(0);
	}

MODULE_CLASS_END(Loom)


#endif /* Perf_RR_h */
