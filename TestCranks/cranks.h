#ifndef	_cranks_h
#define	_cranks_h

#include	"test.h"


class	CR1:
public Crank<CR1>{
public:
	template<class	P>	void	process(P	*p){
	}
};


#endif