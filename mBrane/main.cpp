#include	<iostream>

#include	"node.h"


using	namespace	mBrane;
using	namespace	mBrane::sdk;
/*
class	M:public	Message<M>{
public:
	uint32	a;
	uint32	b;
};

class	CM:public	ControlMessage<CM>{
public:
	uint32	a;
	uint32	b;
};
*/
//#include	"..\Core\payload_utils.h"

int	main(int	argc,char	**argv){

	Time::Init();

	if(argc!=3){
	
		std::cout<<"usage: mBrane <boot delay in ms> <config file name>\n";
		return	0;
	}
/*
	Thread::Sleep(atoi(argv[1]));

	mBrane::Node	*n=mBrane::Node::New(argv[2]);
	if(n){

		if(!n->loadApplication()){

			n->run();
			n->shutdown();
			n->unloadApplication();
		}
		delete	n;
	}*/
/*
	TimeProbe	probe;
	Timer	_timer;

	probe.set();

	_timer.start(1000,0);
	_timer.wait();

	probe.check();
	std::cout<<"probe1: "<<probe.us()<<std::endl;
*/
/*
	payloads::Array<uint32,16,Memory>	a;
	payloads::Pipe<uint32,16,Memory>	c;
	payloads::List<uint32,16,Memory>	l;
	*/
/*
	CM	*cm=new	CM();
//	P<CM>	cmp=cm;
	_ControlMessage	*_cm=cm;

	_Payload	*pp=_cm->operator	_Payload	*();
	_Payload	*p=(_Payload	*)_cm;	//	compiler does not call the cast operator
	*/
/*
	M	*m=new	M();
	_Message	*_m=m;

	_ControlMessage	*pm=_m->operator	_ControlMessage	*();

	P<M>	mp=m;
	*/
	return	0;
}