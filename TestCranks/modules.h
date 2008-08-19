#ifndef	_modules_h
#define	_modules_h

#include	"test.h"


class	CR1:
public Module<CR1>{
public:
	CR1(uint16	_ID);
	~CR1();
	Decision	decide(_Payload	*p){
		//	switch(p->cid()){
		//	case	CLASS_ID(C_FINAL):
		//		if(doing x)
		//			return	PREEMPT;
		//	}
		return	WAIT;
	}
	template<class	P>	void	react(P	*p){
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


#endif