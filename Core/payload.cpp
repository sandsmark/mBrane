// payload.cpp
//
// Author: Eric Nivel
//
//

#include	<memory>
#include	"payload.h"
#include	"node.h"


namespace	mBrane{
	namespace	sdk{

		inline	_Payload::_Payload():_Object(){
		}

		inline	_Payload::~_Payload(){
		}

		inline	void	_Payload::init(uint8	*_buffer,size_t	_size){

			this->_buffer=_buffer;
			this->_size=_size;
		}

		inline	uint8	*_Payload::buffer()	const{

			return	_buffer;
		}

		inline	size_t	_Payload::size()	const{

			return	_size;
		}

		uint8	_Payload::ptrCount()	const{

			return	0;
		}

		_Payload	*_Payload::ptr(uint8	i)	const{

			return	NULL;
		}

		inline	int64	&_Payload::send_ts(){

			return	_send_ts;
		}
				
		inline	int64	&_Payload::recv_ts(){

			return	_recv_ts;
		}

		inline	int8	_Payload::_send(){

			int8	r;
			for(uint8	i=0;i<ptrCount();i++)
				if(r=ptr(i)->_send()<0)
					return	r;
			return	Node::Get()->send(buffer(),size());
		}

		inline	int8	_Payload::_recv(){

			int8	r;
			for(uint8	i=0;i<ptrCount();i++)
				if(r=ptr(i)->_recv()<0)
					return	r;
			return	Node::Get()->receive(buffer(),size());
		}

		inline	int8	_Payload::send(){

			int8	r;
			if(r=Node::Get()->beginTransmission()<0)
				return	r;
			if(r=_send()<0)
				return	r;
			return	Node::Get()->endTransmission();
		}

		inline	int8	_Payload::recv(){

			int8	r;
			if(r=Node::Get()->beginReception()<0)
				return	r;
			if(r=_recv()<0)
				return	r;
			return	Node::Get()->endReception();
		}

		////////////////////////////////////////////////////////////////////////////////////

		inline	_PP::_PP():__P(){
		}

		inline	_PP::~_PP(){
		}

		inline	_PP::operator	_Payload	*(){

			return	(_Payload	*)object;
		}

		inline	_PP	&_PP::operator	=(_Payload	*o){

			__P::operator	=(o);
			return	*this;
		}

		inline	_PP	&_PP::operator	=(_PP	&p){

			__P::operator	=(p);
			return	*this;
		}
	}
}