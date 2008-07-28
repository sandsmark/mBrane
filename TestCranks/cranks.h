#ifndef	_cranks_h
#define	_cranks_h

#include	"test.h"


class	CR1:
public Crank<CR1>{
public:
	CR1(uint16	_ID);
	~CR1();
	template<class	P>	void	process(P	*p){
	}
	//	define specializations as needed
	template<class	P>	bool	preview(P	*p){
		return	false;
	}
	//	define specializations as needed
};


#endif