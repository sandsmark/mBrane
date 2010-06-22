#ifndef	Perf_class_h
#define	Perf_class_h

#include	"../Core/mBrane.h"
#include	"../Core/message.h"

using	namespace	mBrane;
using	namespace	mBrane::sdk;
using	namespace	mBrane::sdk::payloads;

template<class	U>	class	_Ball:	//	to allow derivation
public	payloads::Message<U,Memory>{
public:
	uint32	d1;
};

class	Ball:public		_Ball<Ball>{
public:
	int32	id;
	Ball():_Ball<Ball>(),id(-1){}
	Ball(int32	id):_Ball<Ball>(),id(id){}
};
class	ReturnBall:public		_Ball<ReturnBall>{
public:
	int32	id;
	ReturnBall():_Ball<ReturnBall>(),id(-1){}
	ReturnBall(int32	id):_Ball<ReturnBall>(),id(id){}
};
class	Type1:public	payloads::Message<Type1,Memory>{};
class	Type2:public	payloads::Message<Type2,Memory>{};

class	Ball1:public _Ball<Ball1>{
public:
	int32	num;
	int32	num2;
	Ball1():_Ball<Ball1>(),num(-1),num2(10){}
	Ball1(int32	num):_Ball<Ball1>(),num(num),num2(num+1){}
};

class	Ball2:public _Ball<Ball2>{
public:
	int32	num;
	int32	num2;
	Ball2():_Ball<Ball2>(),num(-1),num2(10){}
	Ball2(int32	num):_Ball<Ball2>(),num(num),num2(num+1){}
};

class	Ball3:public _Ball<Ball3>{
public:
	int32	num;
	int32	num2;
	Ball3():_Ball<Ball3>(),num(-1),num2(10){}
	Ball3(int32	num):_Ball<Ball3>(),num(num),num2(num+1){}
};

class	Ball4:public _Ball<Ball4>{
public:
	int32	num;
	int32	num2;
	Ball4():_Ball<Ball4>(),num(-1),num2(10){}
	Ball4(int32	num):_Ball<Ball4>(),num(num),num2(num+1){}
};

class	Ball5:public _Ball<Ball5>{
public:
	int32	num;
	int32	num2;
	Ball5():_Ball<Ball5>(),num(-1),num2(10){}
	Ball5(int32	num):_Ball<Ball5>(),num(num),num2(num+1){}
};

class	Ball6:public _Ball<Ball6>{
public:
	int32	num;
	int32	num2;
	Ball6():_Ball<Ball6>(),num(-1),num2(10){}
	Ball6(int32	num):_Ball<Ball6>(),num(num),num2(num+1){}
};

class	Ball7:public _Ball<Ball7>{
public:
	int32	num;
	int32	num2;
	Ball7():_Ball<Ball7>(),num(-1),num2(10){}
	Ball7(int32	num):_Ball<Ball7>(),num(num),num2(num+1){}
};

class	Ball8:public _Ball<Ball8>{
public:
	int32	num;
	int32	num2;
	Ball8():_Ball<Ball8>(),num(-1),num2(10){}
	Ball8(int32	num):_Ball<Ball8>(),num(num),num2(num+1){}
};

class	Ball9:public _Ball<Ball9>{
public:
	int32	num;
	int32	num2;
	Ball9():_Ball<Ball9>(),num(-1),num2(10){}
	Ball9(int32	num):_Ball<Ball9>(),num(num),num2(num+1){}
};

//	For testing the cache.

class	Shared:public	SharedObject<Shared,Memory>{
public:
};

class	Constant:public	ConstantObject<Constant,Memory>{
public:
};



#endif /* Perf_class.h */
