#ifndef	application_modules_h
#define	application_modules_h

#include	"test.h"


MODULE_CLASS_BEGIN(Module0,Module<Module0>)
	template<class	T>	Decision	decide(T	*p){
		return	WAIT;
	}
	template<class	T>	void	react(T	*p){	//	to messages
		std::cout<<"Module0 got a message...\n";
	}
	template<class	T>	void	react(uint16	sid,T	*p){	//	to stream data
		std::cout<<"Module0 got data on stream...\n";
	}
	//	define specializations as needed
	//	alternative: 1 method and a switch, ex:
	//	template<class	P>	void	react(P	*p){
	//		switch(p->cid(){
	//		case	CLASS_ID(FINAL_C1):	... break;
	//		...
	//		}
	//	}
MODULE_CLASS_END(Module0)


MODULE_CLASS_BEGIN(Module1,Module<Module1>)
	template<class	T>	Decision	decide(T	*p){
		return	WAIT;
	}
	template<class	T>	void	react(T	*p){	//	to messages
		std::cout<<"Module1 got a message...\n";
	}
	template<class	T>	void	react(uint16	sid,T	*p){	//	to stream data
		std::cout<<"Module1 got data on stream...\n";
	}
	//	define specializations as needed
	//	alternative: 1 method and a switch, ex:
	//	template<class	P>	void	react(P	*p){
	//		switch(p->cid(){
	//		case	CLASS_ID(FINAL_C1):	... break;
	//		...
	//		}
	//	}
MODULE_CLASS_END(Module1)

#endif