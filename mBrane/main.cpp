#include	<iostream>

#include	"node.h"


using	namespace	mBrane;
using	namespace	mBrane::sdk;
/*
class	M:
public	DynamicData<Message<M> >{
public:
	uint32	a;
	uint32	b;
	size_t	dynamicSize()	const{	return	0;	}
};

class	CM:public	ControlMessage<CM>{
public:
	uint32	a;
	uint32	b;
};*/

mBrane::Node	*node=NULL;

bool	signal_handler_function_call	Handler(uint32	event){

	if(!node)
		return	false;

	switch(event){
	case	CTRL_C_EVENT:
    case	CTRL_CLOSE_EVENT:
      	node->shutdown();
		delete	node;
		return	true;
    case	CTRL_BREAK_EVENT:
    case	CTRL_LOGOFF_EVENT:
    case	CTRL_SHUTDOWN_EVENT:
		node->shutdown();
		delete	node;
		return	false;
    default:
		return false; 
	}
}

int	main(int	argc,char	**argv){

	Time::Init(1000);

	mBrane::SignalHandler::Add((signal_handler)Handler);

	if(argc!=3){
	
		std::cout<<"usage: mBrane <boot delay in ms> <config file name>\n";
		return	0;
	}

	Thread::Sleep(atoi(argv[1]));

	node=mBrane::Node::New(argv[2]);
	if(node){

		node->run();
		node->shutdown();
		delete	node;
	}
/*
	TimeProbe	probe;
	Timer	_timer;
	while(1){
	probe.set();
	_timer.start(1000,0);
	_timer.wait();
	probe.check();
	std::cout<<"probe1: "<<probe.us()<<std::endl;
	}
*/
	/*
	_Payload		*m=new	M();
	_ControlMessage	*_cm=(_ControlMessage	*)m;
	_Message		*_m=(_Message	*)m;
	_m->senderModule_cid();
	m->isDynamicData();
	_DynamicData	*_dd=m->operator	_DynamicData	*();
	//	_DynamicData	*_dd=(_DynamicData	*)m;	//	compiler bug: doesn't call the cast operator

	size_t	s=_dd->dynamicSize();
*/
	return	0;
}