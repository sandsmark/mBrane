/*
* HUMANOBS - mBrane
*
* Eric Nivel
* Center for Analysis and Design of Intelligent Agents
*   Reykjavik University, Menntavegur 1, 101 Reykjavik, Iceland
*   http://cadia.ru.is
* Copyright(c)2012
*
* This software was developed by the above copyright holder as part of 
* the HUMANOBS EU research project, in collaboration with the 
* following parties:
* 
* Autonomous Systems Laboratory
*   Technical University of Madrid, Spain
*   http://www.aslab.org/
*
* Communicative Machines
*   Edinburgh, United Kingdom
*   http://www.cmlabs.com/
*
* Istituto Dalle Molle di Studi sull'Intelligenza Artificiale
*   University of Lugano and SUPSI, Switzerland
*   http://www.idsia.ch/
*
* Institute of Cognitive Sciences and Technologies
*   Consiglio Nazionale delle Ricerche, Italy
*   http://www.istc.cnr.it/
*
* Dipartimento di Ingegneria Informatica
*   University of Palermo, Italy
*   http://roboticslab.dinfo.unipa.it/index.php/Main/HomePage
*
*
* --- HUMANOBS Open-Source BSD License, with CADIA Clause v 1.0 ---
*
* Redistribution and use in source and binary forms, with or without 
* modification, is permitted provided that the following conditions 
* are met:
*
* - Redistributions of source code must retain the above copyright 
* and collaboration notice, this list of conditions and the 
* following disclaimer.
*
* - Redistributions in binary form must reproduce the above copyright 
* notice, this list of conditions and the following
* disclaimer in the documentation and/or other materials provided 
* with the distribution.
*
* - Neither the name of its copyright holders nor the names of its 
* contributors may be used to endorse or promote products 
* derived from this software without specific prior written permission.
*
* - CADIA Clause: The license granted in and to the software under this 
* agreement is a limited-use license. The software may not be used in 
* furtherance of: 
* (i) intentionally causing bodily injury or severe emotional distress 
* to any person; 
* (ii) invading the personal privacy or violating the human rights of 
* any person; or 
* (iii) committing or preparing for any act of war.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
* "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
* LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR 
* A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
* OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
* SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
* LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, 
* DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY 
* THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
* OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include	<iostream>

#if defined(WINDOWS)
#include	<conio.h>
#endif

#include	"node.h"

using	namespace	core;
using	namespace	mBrane;


mBrane::Node	*node=NULL;

SharedLibrary	SL;	//	will be initialized with the library loaded by the node; this ensures SL is deleted last, and thus that all user-defined adresses (e.g. __vfptr) are still valid until the very end

bool	signal_handler_function_call	Handler(uint32_t	event){

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
	uint32_t	id;
	uint8_t	data[400];
};

class	LoadM:public	Load{
public:
	LoadM(uint32_t	i){id=i;}
	void	*operator	new(size_t	s){	return	malloc(s);	}
	void	operator	delete(void	*o){	free(o);	}
};

class	LoadC:public	Load{
private:
	static	Memory	*Allocator;
public:
	LoadC(uint32_t	i){id=i;}
	void	*operator	new(size_t	s){	return	Allocator->alloc();	}
	void	operator	delete(void	*o){	Allocator->dealloc(o);	}
};

Memory	*LoadC::Allocator=Memory::GetStatic(sizeof(LoadC));

template<class	L>	int64_t	RAM_perf_probe(){	//	the real test is to allocate/deallocate randomly, using different sizes

	L	*store[LOAD_COUNT];

	int64_t	start;
	int64_t	end;

	start=Time::Get();
	for(uint32_t	i=0;i<LOAD_COUNT;i++){

		store[i]=new	L(i);
		delete	store[i];
	}
	//for(uint32_t	i=0;i<LOAD_COUNT;i++)
	//	std::cout<<store[i]->id<<std::endl;
	//for(uint32_t	i=0;i<LOAD_COUNT;i++){

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
