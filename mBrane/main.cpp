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

mBrane::Node	*node=NULL;

bool	signal_handler_function_call	Handler(uint32	event){

	if(!node)
		return	false;

	switch(event){
	case	CTRL_C_EVENT:
    case	CTRL_CLOSE_EVENT:
      	node->shutdown();
		node->unloadApplication();
		delete	node;
		return	true;
    case	CTRL_BREAK_EVENT:
    case	CTRL_LOGOFF_EVENT:
    case	CTRL_SHUTDOWN_EVENT:
		node->shutdown();
		node->unloadApplication();
		delete	node;
		return	false;
    default:
		return false; 
	}
}

int	main(int	argc,char	**argv){

	Time::Init();

	mBrane::SignalHandler::Add((signal_handler)Handler);

	if(argc!=3){
	
		std::cout<<"usage: mBrane <boot delay in ms> <config file name>\n";
		return	0;
	}
/*
	Thread::Sleep(atoi(argv[1]));

	node=mBrane::Node::New(argv[2]);
	if(node){

		if(!node->loadApplication()){

			node->run();
			node->shutdown();
			node->unloadApplication();
		}
		delete	node;
	}*/

	TimeProbe	probe;
	Timer	_timer;
	while(1){
	probe.set();
	_timer.start(1000,0);
	_timer.wait();
	probe.check();
	std::cout<<"probe1: "<<probe.us()<<std::endl;
	}
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