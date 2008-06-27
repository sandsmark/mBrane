// node.cpp
//
// Author: Eric Nivel
//
//

#include	"node.h"
#include	<windows.h>
#include	<iostream>

#include	"..\Core\crank.h"


typedef	mBrane::sdk::_Crank	*(__cdecl	*CrankInstantiator)();

namespace	mBrane{
	namespace	node{

		Node::Node():mBrane::sdk::Node(){
			//	read node config file and load plugins etc
			//	read application config file and load cranks etc
		}

		Node::~Node(){
		}

		inline	uint16	Node::ID(){

			return	_ID;
		}

		void	Node::run(){

		    HINSTANCE	lib=LoadLibrary(TEXT("TestCranks"));
			if(lib){ 
			
				CrankInstantiator	instantiator=(CrankInstantiator)GetProcAddress(lib,"NewCR1"); //	test
				if(instantiator){

					mBrane::sdk::_Crank	*c=(instantiator)();
					delete	c;
				}else{

					DWORD	error=GetLastError();
					std::cout<<"GetProcAddress Error: "<<error<<std::endl;
				}

				FreeLibrary(lib);
			}else{

				DWORD	error=GetLastError();
				std::cout<<"LoadLibrary Error: "<<error<<std::endl;
			}
		}

		int8	Node::beginTransmission(){

			return	0;
		}

		int8	Node::endTransmission(){

			return	0;
		}
		
		int8	Node::beginReception(){

			return	0;
		}

		int8	Node::endReception(){

			return	0;
		}

		int8	Node::send(uint8	*b,size_t	s){

			return	0;
		}

		int8	Node::receive(uint8	*b,size_t	s){

			return	0;
		}
	}
}