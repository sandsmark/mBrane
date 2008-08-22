//	user defined

#ifndef	application_c_h
#define	application_c_h

#include	"..\Core\mBrane.h"


using	namespace	mBrane;
using	namespace	mBrane::sdk;
using	namespace	mBrane::sdk::payloads;

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
	static	uint8		PtrCount(){	
		
		return	2;
	}
	static	P<_RPayload>	*Ptr(_RPayload	*p,uint8	i){

		switch(i){

			case	0:	return	PTR(U,p,p1);
			case	1:	return	PTR(U,p,p2);
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
	static	uint8		PtrCount(){	
		
		return	C2<U>::ptrCount()+2;
	}
	static	P<_RPayload>	*Ptr(_RPayload	*p,uint8	i){

		uint8	upCount=C2<U>::PtrCount();
		if(i<upCount)
			return	C2<U>::Ptr(i);
		switch(i-upCount){

			case	0:	return	PTR(U,p,p3);
			case	1:	return	PTR(U,p,p4);
			default:	return	NULL;
		}
	}
};

class	Final_C4:public	C4<Final_C4>{};

class	Final_SD1:
public	StreamData<Final_SD1>{
public:
	int64	timeCode;
	float32	frame[3*1024*768];
};


#endif