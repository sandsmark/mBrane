#ifndef	pingpong_class_h
#define	pingpong_class_h

#include	"../Core/mBrane.h"
#include	"../Core/message.h"

using	namespace	mBrane;
using	namespace	mBrane::sdk;
using	namespace	mBrane::sdk::payloads;

template<class	U>	class	_Ball:	//	to allow derivation
public	payloads::Message<U,StaticData,Memory>{
public:
	uint32	d1;
};

class	Ball:public		_Ball<Ball>{
public:
	int32	id;
	Ball():_Ball<Ball>(),id(-1){}
	Ball(int32	id):_Ball<Ball>(),id(id){}
};
class	Type1:public	payloads::Message<Type1,StaticData,Memory>{};
class	Type2:public	payloads::Message<Type2,StaticData,Memory>{};


#endif /* pingpong_class.h */
