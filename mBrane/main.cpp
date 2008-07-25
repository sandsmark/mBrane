#include	<iostream>

#include	"node.h"


using	namespace	mBrane;
using	namespace	mBrane::sdk;

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

int	main(int	argc,char	**argv){

	if(argc!=2){
	
		std::cout<<"usage: mBrane <config file name>\n";
		return	0;
	}
/*
	Node	n(argv[1]);
	n.loadApplication();
	n.run();
	n.shutdown();
	n.unloadApplication();
*/
	CM	*cm=new	CM();
	_ControlMessage	*_cm=cm;

	_Payload	*pp=_cm->operator	_Payload	*();
	_Payload	*p=(_Payload	*)_cm;	//	compiler does not call the operator

	P<CM>	cmp=cm;

	M	*m=new	M();
	_Message	*_m=m;

	_ControlMessage	*pm=_m->operator	_ControlMessage	*();

	P<M>	mp=m;
	
	return	0;
}