#ifndef	_cranks_h
#define	_cranks_h

#include	"test.h"


class	CR1:
public Crank<CR1>{
public:
	CR1(uint16	_ID):Crank<CR1>(_ID){}
	template<class	P>	void	process(P	*p){
	}
};


#endif