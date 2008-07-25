// crank.cpp
//
// Author: Eric Nivel
//
//

#include	"crank.h"
#include	"node.h"


namespace	mBrane{
	namespace	sdk{

		_Crank::_Crank(uint16	_ID):_ID(_ID){
		}

		_Crank::~_Crank(){
		}

		inline	uint16	_Crank::ID()	const{

			return	_ID;
		}

		inline	void	_Crank::send(_Message	*m){

			Node::Get()->send(_ID,m);
		}

		inline	void	_Crank::send(_ControlMessage	*m){

			Node::Get()->send(_ID,m);
		}

		inline	int64	_Crank::time(){

			return	Node::Get()->time();
		}
	}
}
