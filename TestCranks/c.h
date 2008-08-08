//	user defined

#ifndef	c_h
#define	c_h

#include	"..\Core\mBrane.h"


using	namespace	mBrane;
using	namespace	mBrane::sdk;

//	one could also define (or include) custom memory managers here

template<class	U>	class	C1:	//	to allow derivation
public	Payload<Memory,U>{
public:
	uint32	d1;
};

class	Final_C1:public	C1<Final_C1>{};	//	non derivable

class	RP1:public	RPayload<Memory,RP1>{};

class	Final_C2;
template<class	U>	class	C2:
public	Payload<Memory,U>{
public:
	uint32	d2;
	P<RP1>	p1;
	P<RP1>	p2;
	virtual	uint8		ptrCount()	const{	
		
		return	2;
	}
	virtual	P<_RPayload>	*ptr(uint8	i){

		switch(i){

			case	0:	return	(P<_RPayload>	*)&p1;
			case	1:	return	(P<_RPayload>	*)&p2;
			default:	return	NULL;
		}
	}
};

class	Final_C2:public	C2<Final_C2>{};

template<class	U>	class	C3:
public	C1<U>{
public:
	uint32	d3;
};

class	Final_C3:public	C3<Final_C3>{};

template<class	U>	class	C4:
public	C2<U>{
public:
	uint32	d4;
	P<RP1>	p3;
	P<RP1>	p4;
	virtual	uint8		ptrCount()	const{	
		
		return	C2<U>::ptrCount()+2;
	}
	virtual	P<_RPayload>	*ptr(uint8	i){

		uint8	upCount=C2<U>::ptrCount();
		if(i<upCount)
			return	C2<U>::ptr(i);
		switch(i-upCount){

			case	0:	return	(P<_RPayload>	*)&p3;
			case	1:	return	(P<_RPayload>	*)&p4;
			default:	return	NULL;
		}
	}
};

class	Final_C4:public	C4<Final_C4>{};


#endif