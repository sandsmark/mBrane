#ifndef	c_h
#define	c_h

#include	"..\Core\mBrane.h"


using	namespace	mBrane;
using	namespace	mBrane::sdk;

//	one could also define (or include) custom memory managers here

template<class	U>	class	C1:
public	Payload<Memory,U>{
public:
	uint32	c1;
};

class	_C1:public	C1<_C1>{};

template<class	U>	class	C2:
public	Payload<Memory,U>{
public:
	uint32	c2;
	PP<_C1>	c1;
	virtual	uint8		ptrCount()	const{	
		
		return	Payload<Memory,U>::ptrCount()+1;	
	}
	virtual	_Payload	*ptr(uint8	i)	const{	
		
		if(i>=Payload<Memory,U>::ptrCount()){

			if(i==Payload<Memory,U>::ptrCount())
				return	(_C1	*)c1;
		}
		return	Payload<Memory,U>::ptr(i);	
	}
};

class	__C2:public	C2<__C2>{};

template<class	U>	class	C3:
public	C1<U>{
public:
	uint32	c3;
};

class	_C3:public	C3<_C3>{};


#endif