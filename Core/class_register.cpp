// class_register.cpp
//
// Author: Eric Nivel
//
//

#include	<memory>
#include	"class_register.h"


namespace	mBrane{
	namespace	sdk{

		ClassRegister::Array::Array():_array(NULL),_count(0){
		}

		ClassRegister::Array::~Array(){

			if(_array)
				delete[]	_array;
		}

		ClassRegister	*ClassRegister::Array::alloc(uint16	&CID){

			if(_array)
				realloc(_array,(++_count)*sizeof(ClassRegister));
			else
				_array=(ClassRegister	*)malloc((++_count)*sizeof(ClassRegister));
			CID=_count-1;
			return	_array+CID;
		}

		inline	ClassRegister	*ClassRegister::Array::get(uint16	CID){

			return	_array+CID;
		}

		inline	uint16	ClassRegister::Array::count()	const{

			return	_count;
		}
	
		////////////////////////////////////////////////////////////////////////////////////

		ClassRegister::Array	ClassRegister::Classes;

		inline	ClassRegister	*ClassRegister::Get(uint16	CID){

			return	Classes.get(CID);
		}

		uint16	ClassRegister::Count(){

			return	Classes.count();
		}

		ClassRegister::ClassRegister():allocator(NULL){
		}

		ClassRegister::~ClassRegister(){
		}
	}
}