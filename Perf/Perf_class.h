//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
//_/_/
//_/_/ HUMANOBS - mBrane - Perf
//_/_/
//_/_/ Eric Nivel
//_/_/ Center for Analysis and Design of Intelligent Agents
//_/_/   Reykjavik University, Menntavegur 1, 101 Reykjavik, Iceland
//_/_/   http://cadia.ru.is
//_/_/ Copyright(c)2012
//_/_/
//_/_/ This software was developed by the above copyright holder as part of 
//_/_/ the HUMANOBS EU research project, in collaboration with the 
//_/_/ following parties:
//_/_/ 
//_/_/ Autonomous Systems Laboratory
//_/_/   Technical University of Madrid, Spain
//_/_/   http://www.aslab.org/
//_/_/
//_/_/ Communicative Machines
//_/_/   Edinburgh, United Kingdom
//_/_/   http://www.cmlabs.com/
//_/_/
//_/_/ Istituto Dalle Molle di Studi sull'Intelligenza Artificiale
//_/_/   University of Lugano and SUPSI, Switzerland
//_/_/   http://www.idsia.ch/
//_/_/
//_/_/ Institute of Cognitive Sciences and Technologies
//_/_/   Consiglio Nazionale delle Ricerche, Italy
//_/_/   http://www.istc.cnr.it/
//_/_/
//_/_/ Dipartimento di Ingegneria Informatica
//_/_/   University of Palermo, Italy
//_/_/   http://roboticslab.dinfo.unipa.it/index.php/Main/HomePage
//_/_/
//_/_/
//_/_/ --- HUMANOBS Open-Source BSD License, with CADIA Clause v 1.0 ---
//_/_/
//_/_/ Redistribution and use in source and binary forms, with or without 
//_/_/ modification, is permitted provided that the following conditions 
//_/_/ are met:
//_/_/
//_/_/ - Redistributions of source code must retain the above copyright 
//_/_/ and collaboration notice, this list of conditions and the 
//_/_/ following disclaimer.
//_/_/
//_/_/ - Redistributions in binary form must reproduce the above copyright 
//_/_/ notice, this list of conditions and the following
//_/_/ disclaimer in the documentation and/or other materials provided 
//_/_/ with the distribution.
//_/_/
//_/_/ - Neither the name of its copyright holders nor the names of its 
//_/_/ contributors may be used to endorse or promote products 
//_/_/ derived from this software without specific prior written permission.
//_/_/
//_/_/ - CADIA Clause: The license granted in and to the software under this 
//_/_/ agreement is a limited-use license. The software may not be used in 
//_/_/ furtherance of: 
//_/_/ (i) intentionally causing bodily injury or severe emotional distress 
//_/_/ to any person; 
//_/_/ (ii) invading the personal privacy or violating the human rights of 
//_/_/ any person; or 
//_/_/ (iii) committing or preparing for any act of war.
//_/_/
//_/_/ THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
//_/_/ "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
//_/_/ LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR 
//_/_/ A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
//_/_/ OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
//_/_/ SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
//_/_/ LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, 
//_/_/ DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY 
//_/_/ THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
//_/_/ (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
//_/_/ OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//_/_/
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

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
	uint32_t d1;
	uint32_t padding;
};

class	Ball:public		_Ball<Ball>{
public:
	int32_t id;
	Ball():_Ball<Ball>(),id(-1){}
	Ball(int32_t id):_Ball<Ball>(),id(id){}
};
class	ReturnBall:public		_Ball<ReturnBall>{
public:
	int32_t id;
	ReturnBall():_Ball<ReturnBall>(),id(-1){}
	ReturnBall(int32_t id):_Ball<ReturnBall>(),id(id){}
};
class	Type1:public	payloads::Message<Type1,Memory>{};
class	Type2:public	payloads::Message<Type2,Memory>{};

class	Ball1:public _Ball<Ball1>{
public:
	int32_t num;
	int32_t num2;
	Ball1():_Ball<Ball1>(),num(-1),num2(10){}
	Ball1(int32_t num):_Ball<Ball1>(),num(num),num2(num+1){}
};

class	Ball2:public _Ball<Ball2>{
public:
	int32_t num;
	int32_t num2;
	Ball2():_Ball<Ball2>(),num(-1),num2(10){}
	Ball2(int32_t num):_Ball<Ball2>(),num(num),num2(num+1){}
};

class	Ball3:public _Ball<Ball3>{
public:
	int32_t num;
	int32_t num2;
	Ball3():_Ball<Ball3>(),num(-1),num2(10){}
	Ball3(int32_t num):_Ball<Ball3>(),num(num),num2(num+1){}
};

class	Ball4:public _Ball<Ball4>{
public:
	int32_t num;
	int32_t num2;
	Ball4():_Ball<Ball4>(),num(-1),num2(10){}
	Ball4(int32_t num):_Ball<Ball4>(),num(num),num2(num+1){}
};

class	Ball5:public _Ball<Ball5>{
public:
	int32_t num;
	int32_t num2;
	Ball5():_Ball<Ball5>(),num(-1),num2(10){}
	Ball5(int32_t num):_Ball<Ball5>(),num(num),num2(num+1){}
};

class	Ball6:public _Ball<Ball6>{
public:
	int32_t num;
	int32_t num2;
	Ball6():_Ball<Ball6>(),num(-1),num2(10){}
	Ball6(int32_t num):_Ball<Ball6>(),num(num),num2(num+1){}
};

class	Ball7:public _Ball<Ball7>{
public:
	int32_t num;
	int32_t num2;
	Ball7():_Ball<Ball7>(),num(-1),num2(10){}
	Ball7(int32_t num):_Ball<Ball7>(),num(num),num2(num+1){}
};

class	Ball8:public _Ball<Ball8>{
public:
	int32_t num;
	int32_t num2;
	Ball8():_Ball<Ball8>(),num(-1),num2(10){}
	Ball8(int32_t num):_Ball<Ball8>(),num(num),num2(num+1){}
};

class	Ball9:public _Ball<Ball9>{
public:
	int32_t num;
	int32_t num2;
	Ball9():_Ball<Ball9>(),num(-1),num2(10){}
	Ball9(int32_t num):_Ball<Ball9>(),num(num),num2(num+1){}
};

//	For testing the cache.

class	Shared:public	SharedObject<Shared,Memory>{
public:
};


#endif /* Perf_class.h */
