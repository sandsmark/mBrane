#include	"..\Core\node.h"
#include	<iostream>
#include	"..\Core\message.h"

using	namespace	mBrane::sdk;

//class	_Message:public	Message<_Message>{};

int	main(int	argc,char	**argv){

	if(argc!=2){
	
		std::cout<<"usage: mBrane <config file name>\n";
		return	0;
	}

	Node	*n=new	Node(argv[1]);
	n->run();
//	_Message	*m=new	_Message();
//	delete	m;
	return	0;
}