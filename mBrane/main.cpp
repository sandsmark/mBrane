#include	<iostream>

#if defined(WINDOWS)
#include	<conio.h>
#endif

#include	"node.h"

using	namespace	mBrane;


mBrane::Node	*node=NULL;

SharedLibrary	SL;	//	will be initialized with the library loaded by the node; this ensures SL is deleted last, and thus that all user-defined adresses (e.g. __vfptr) are still valid until the very end

bool	signal_handler_function_call	Handler(uint32	event){

	if(!node)
		return	false;

#if defined(WINDOWS)
	switch(event){
	case	CTRL_C_EVENT:
	case	CTRL_CLOSE_EVENT:
      	node->shutdown();
		delete	node;
		exit(0);
    case	CTRL_BREAK_EVENT:
    case	CTRL_LOGOFF_EVENT:
    case	CTRL_SHUTDOWN_EVENT:
		node->shutdown();
		delete	node;
		exit(0);
    default:
		return false; 
	}
#else
	switch(event){
	case	SIGTERM:
	case	SIGINT:
	case	SIGABRT:
      	node->shutdown();
		delete	node;
		exit(0);
	case	SIGHUP:
	case	SIGFPE:
	case	SIGILL:
	case	SIGSEGV:
    default:
		return false; 
	}
#endif

}

//#define	SAND_BOX

#ifdef	SAND_BOX

#define	MEMORY_PERF_TEST

#ifdef	MEMORY_PERF_TEST

#define	LOAD_COUNT	10000

class	Load{
public:
	uint32	id;
	uint8	data[400];
};

class	LoadM:public	Load{
public:
	LoadM(uint32	i){id=i;}
	void	*operator	new(size_t	s){	return	malloc(s);	}
	void	operator	delete(void	*o){	free(o);	}
};

class	LoadC:public	Load{
private:
	static	Memory	*Allocator;
public:
	LoadC(uint32	i){id=i;}
	void	*operator	new(size_t	s){	return	Allocator->alloc();	}
	void	operator	delete(void	*o){	Allocator->dealloc(o);	}
};

Memory	*LoadC::Allocator=Memory::GetStatic(sizeof(LoadC));

template<class	L>	int64	RAM_perf_probe(){	//	the real test is to allocate/deallocate randomly, using different sizes

	L	*store[LOAD_COUNT];

	int64	start;
	int64	end;

	start=Time::Get();
	for(uint32	i=0;i<LOAD_COUNT;i++){

		store[i]=new	L(i);
		delete	store[i];
	}
	//for(uint32	i=0;i<LOAD_COUNT;i++)
	//	std::cout<<store[i]->id<<std::endl;
	//for(uint32	i=0;i<LOAD_COUNT;i++){

		//store[i]=new	L();
	//	delete	store[i];
	//}
	end=Time::Get();

	return	end-start;
}

#endif

#endif


int	main(int	argc,char	**argv){

	Time::Init(1000);

#ifndef	SAND_BOX

	mBrane::SignalHandler::Add((signal_handler)Handler);

	if(argc!=4){
	
		std::cout<<"usage: mBrane <boot delay in ms> <config file name> <OR-ed trace levels: (msb) application | network | execution (lsb)>"<<std::endl;
		return	0;
	}

	Thread::Sleep(atoi(argv[1]));

	node=mBrane::Node::New(argv[2],SL,atoi(argv[3]));

	// We could not initialise everything, bailing out
	if (!node){

		std::cout<<"Press a key to quit."<<std::endl;
		std::cin.ignore(0,'\n');
		std::cin.get();
		return 1;
	}

	std::cout<<"Running\n";

	node->run();
#else

#ifdef	MEMORY_PERF_TEST

	double	m_perf=(double)RAM_perf_probe<LoadM>();
	double	c_perf=(double)RAM_perf_probe<LoadC>();
	std::cout<<"m perf: "<<m_perf<<std::endl;
	std::cout<<"c perf: "<<c_perf<<std::endl;
	std::cout<<"speedup: x "<<m_perf/c_perf<<std::endl;

#endif

	int	i;std::cin>>i;

#endif

	return	0;
}
