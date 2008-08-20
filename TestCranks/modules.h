#ifndef	_modules_h
#define	_modules_h

#include	"test.h"


class	CR1:
public Module<CR1>{
public:
	CR1(uint16	ID,uint16	clusterCID,uint16	clusterID);
	~CR1();
	template<class	T>	Decision	decide(T	*p){
		return	WAIT;
	}
	template<class	T>	void	react(T	*p){
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