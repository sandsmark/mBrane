//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/ //_/_/ //_/_/  main.cpp //_/_/ //_/_/  Eric Nivel //_/_/  Center for Analysis and Design of Intelligent Agents //_/_/    Reykjavik University, Menntavegur 1, 101 Reykjavik, Iceland //_/_/    http://cadia.ru.is
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

#include	<iostream>

#if defined(WINDOWS)
#include	<conio.h>
#endif

#include	"node.h"

using	namespace	core;
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
#include	"../Core/payload_utils.h"

int	main(int	argc,char	**argv){

	//SyncProbe* probe=new	SyncProbe();
	//printf("SyncProbe size is: %u\n\n", probe->size());
	//Error::PrintBinary(probe, probe->size(), true, "SyncProbe Content");
	//exit(0);

	LoadControlMessageMetaData();

	Time::Init(1000);

#ifndef	SAND_BOX

	SignalHandler::Add((signal_handler)Handler);

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

	// std::cout<<"Running\n";

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
