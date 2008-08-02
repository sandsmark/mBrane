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

int	main(int	argc,char	**argv){

	if(argc!=3){
	
		std::cout<<"usage: mBrane <boot delay in ms> <config file name>\n";
		return	0;
	}

	Thread::Sleep(atoi(argv[1]));

	mBrane::Node	*n=mBrane::Node::New(argv[2]);
	if(n){

		if(!n->loadApplication()){

			n->run();
			n->shutdown();
			n->unloadApplication();
		}
		delete	n;
	}
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