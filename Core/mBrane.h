// mBrane.h
//
// Author: Eric Nivel
//
//

#ifndef	mBrane_h
#define	mBrane_h

#include	"memory.h"
#include	"crank.h"


#define	USER_CLASSES_BEGIN	\
	template<class	U>	class	Crank:	\
public	mBrane::sdk::_Crank{	\
protected:	\
	static	mBrane::uint16	_CID;	\
	Crank(uint16	_ID):_Crank(_ID){}	\
public:	\
	void	notify(_Payload	*p){	\
		switch(p->cid()){
	
#define	CLASS(C)	\
			case	__COUNTER__:	((U	*)this)->process((C	*)p);	return;

#define	USER_CLASSES_END	\
			default:	return;	\
		}	\
	}	\
};	\
template<class	U>	mBrane::uint16	Crank<U>::_CID;


#endif