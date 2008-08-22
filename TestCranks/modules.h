#ifndef	application_modules_h
#define	application_modules_h

#include	"test.h"


class	Module1:
public Module<Module1>{
public:
	Module1(uint16	ID,uint16	clusterCID,uint16	clusterID);
	~Module1();
	template<class	T>	Decision	decide(T	*p){
		return	WAIT;
	}
	template<class	T>	void	react(T	*p){	//	to messages
	}
	template<class	T>	void	react(uint16	sid,T	*p){	//	to stream data
	}
	//	define specializations as needed
	//	alternative: 1 method and a switch, ex:
	//	template<class	P>	void	react(P	*p){
	//		switch(p->cid(){
	//		case	CLASS_ID(C1_FINAL):	... break;
	//		...
	//		}
	//	}
};


MODULE_CLASS(Module1)


#endif