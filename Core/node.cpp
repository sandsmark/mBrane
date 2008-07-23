// node.cpp
//
// Author: Eric Nivel
//
//

#include	"node.h"
#include	"crank.h"
#include	"class_register.h"

#ifdef	WINDOWS
	#include	<windows.h>
#endif

#include	<iostream>



namespace	mBrane{
	namespace	sdk{

		Node	*Node::Singleton=NULL;

		inline	Node	*Node::Get(){

			return	Singleton;
		}

		Node::Node(const	char	*configFileName){
			
			Singleton=this;
			//	read node config file and load plugins etc
			//	plugins:
			//		network: TCP/IP, IB
			//	configuration:
			//		network: 1 (TCP) or 2 (TCP // UDP) Gb eth
			//		network: interface(s) and port(s)
			//		network: time reference or not
			//		application config file name
			//		node ID
			//		network plugins
		}

		Node::~Node(){
		}

		inline	uint16	Node::ID()	const{

			return	_ID;
		}

		typedef	 _Crank *(__cdecl	*CrankInstantiator)(uint16);
		void	Node::run(){
			//	read application config file and load cranks etc
			//	to load:
			//		user defined shared object
			//		entities, modules, groups, cranks
			//		initial subscriptions (per crank, group)
			//		schedulers (per crank)
			//		migrable or not (per crank)
			//		reception policy: time first or priority first (for all cranks)
		    HINSTANCE	lib=LoadLibrary(TEXT("TestCranks"));
			if(lib){ 
			
				CrankInstantiator	instantiator=(CrankInstantiator)GetProcAddress(lib,"NewCR1"); //	test
				if(instantiator){

					_Crank	*c=(instantiator)(0);
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

		int16	Node::_send(uint8	*b,size_t	s){

			return	0;
		}

		int16	Node::_recv(uint8	*b,size_t	s,bool	peek){

			return	0;
		}

		int16	Node::_send(_Payload	*p){

			ClassRegister	*CR=ClassRegister::Get(p->cid());
			int16	r;
			if(r=_send(((uint8	*)p)+CR->offset(),CR->size()))
				return	r;
			for(uint8	i=0;i<p->ptrCount();i++){

				if(r=_send(*p->ptr(i)))
					return	r;
			}
			return	0;
		}

		int16	Node::_recv(_Payload	**p){

			uint16	cid;
			int16	r;
			if(r=_recv((uint8	*)&cid,sizeof(uint16),true))
				return	r;
			ClassRegister	*CR=ClassRegister::Get(cid);
			*p=(_Payload	*)CR->allocator()->alloc();
			if(r=_recv((uint8	*)*p,CR->size()))
				return	r;
			_Payload	*ptr;
			_Payload	**_ptr;
			for(uint8	i=0;i<(*p)->ptrCount();i++){

				if(r=_recv(&ptr)){

					delete	*p;
					return	r;
				}
				_ptr=(*p)->ptr(i);
				*_ptr=ptr;
				(*_ptr)->refCount=1;
			}
			return	0;
		}
	}
}