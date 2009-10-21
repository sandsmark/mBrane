#include	<iostream>

#if defined(WINDOWS)
#include	<conio.h>
#endif

#include	"node.h"
#include	"payload_utils.h"

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

class	_A{
public:
	uint32	a;
};

class	B:public	Payload<B,DynamicData,Memory>{
public:
	uint32	b;
	size_t	dynamicSize()	const{return	0;	}
	//B():DynamicData<RPayload,B>(){	b=10;	}
};

mBrane::Node	*node=NULL;

bool	signal_handler_function_call	Handler(uint32	event){

	if(!node)
		return	false;

#if defined(WINDOWS)
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
#endif

}

int	main(int	argc,char	**argv){

	Time::Init(1000);

	mBrane::SignalHandler::Add((signal_handler)Handler);

	if(argc!=4){
	
		std::cout<<"usage: mBrane <boot delay in ms> <config file name> <OR-ed trace levels: (msb) application | network | execution (lsb)>"<<std::endl;
		return	0;
	}

	Thread::Sleep(atoi(argv[1]));

	node=mBrane::Node::New(argv[2],atoi(argv[3]));

	// We could not initialise everything, bailing out
	if (!node){

		std::cout<<"Press a key to quit."<<std::endl;
		std::cin.ignore(0,'\n');
		std::cin.get();
		return 1;
	}

	std::cout<<"Running\n";

	node->run();

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
/*
	B	b;

	payloads::Array<_A>	a;
	a.ensure(10);
	for(uint32	i=0;i<40;++i)
		a[i].a=i;
	*/
	return	0;
}
