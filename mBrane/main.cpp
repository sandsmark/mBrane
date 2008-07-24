#include	<iostream>

#include	"node.h"


using	namespace	mBrane;
using	namespace	mBrane::sdk;

class	M:public	Message<M>{
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
	M	*m=new	M();
	_Message	*_m=m;

	_Payload	*pp=_m->operator	_Payload	*();
	_Payload	*p=(_Payload	*)_m;	//	compiler does not call the operator

	P<M>	mp=m;
	
	return	0;
}