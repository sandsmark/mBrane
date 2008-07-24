// node.cpp
//
// Author: Eric Nivel
//
//

#include	"node.h"
#include	"..\Core\crank.h"
#include	"..\Core\class_register.h"
#include	"xml_parser.h"

#include	<iostream>

using	namespace	mBrane::sdk;

namespace	mBrane{
	namespace	node{

		Node::Node(const	char	*configFileName):sdk::Node(),_shutdown(false){
			
			Singleton=this;
			XMLNode	mainNode=XMLNode::openFileHelper(configFileName,"NodeConfiguration");
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
			_ID=atoi(mainNode.getAttribute("id"));
			uint8	time_base=atoi(mainNode.getAttribute("time_base"));
			if(time_base){

			}else{

			}
			/*
			configure node
			*/
			application_configuration_file=mainNode.getAttribute("application_configuration_file");
		}

		Node::~Node(){
		}

		void	Node::loadUserLibrary(const	char	*fileName){
#if defined	WINDOWS
			userLibrary=LoadLibrary(TEXT(fileName));
			if(!userLibrary){

				DWORD	error=GetLastError();
				std::cout<<"GetProcAddress Error: "<<error<<std::endl;
			}
#elif defined LINUX
#elif defined OSX
#endif
		}

		void	Node::unloadUserLibrary(){
#if defined	WINDOWS
			if(userLibrary)
				FreeLibrary(userLibrary);
#elif defined LINUX
#elif defined OSX
#endif
		}

		typedef	 sdk::_Crank *(__cdecl	*CrankInstantiator)(uint16);

		void	Node::loadApplication(const	char	*fileName){

			if(fileName)
				application_configuration_file=fileName;
			XMLNode	mainNode=XMLNode::openFileHelper(application_configuration_file,"ApplicationConfiguration");
			//	read application config file and load cranks etc
			//	to load:
			//		user library
			//		entities, modules, groups, cranks
			//		initial subscriptions (per crank, group)
			//		schedulers (per crank)
			//		migrable or not (per crank)
			//		reception policy: time first or priority first (for all cranks)
			//		thread count
			//		target thread (per crank)
			loadUserLibrary(mainNode.getAttribute("user_library"));
#if defined	WINDOWS
			if(userLibrary){ 

				CrankInstantiator	instantiator=(CrankInstantiator)GetProcAddress(userLibrary,"NewCR1"); //	test
				if(instantiator){

					sdk::_Crank	*c=(instantiator)(0);
					delete	c;
				}else{

					DWORD	error=GetLastError();
					std::cout<<"GetProcAddress Error: "<<error<<std::endl;
				}
			}
#elif defined LINUX
#elif defined OSX
#endif
		}

		void	Node::unloadApplication(){
			//	to unload
			//		entities, modules, groups, cranks
			//		user library
			unloadUserLibrary();
		}

		void	Node::run(){

			unloadApplication();
		}

		void	Node::shutdown(){

			_shutdown=true;
		}

		int16	Node::send(uint8	*b,size_t	s){

			return	0;
		}

		int16	Node::recv(uint8	*b,size_t	s,bool	peek){

			return	0;
		}

		int16	Node::send(_Payload	*p){

			ClassRegister	*CR=ClassRegister::Get(p->cid());
			int16	r;
			if(r=send(((uint8	*)p)+CR->offset(),CR->size()))
				return	r;
			for(uint8	i=0;i<p->ptrCount();i++){

				if(r=send(*p->ptr(i)))
					return	r;
			}
			return	0;
		}

		int16	Node::recv(_Payload	**p){

			uint16	cid;
			int16	r;
			if(r=recv((uint8	*)&cid,sizeof(uint16),true))
				return	r;
			ClassRegister	*CR=ClassRegister::Get(cid);
			*p=(_Payload	*)CR->allocator()->alloc();
			if(r=recv((uint8	*)*p,CR->size()))
				return	r;
			_Payload	*ptr;
			_Payload	**_ptr;
			for(uint8	i=0;i<(*p)->ptrCount();i++){

				if(r=recv(&ptr)){

					delete	*p;
					return	r;
				}
				_ptr=(*p)->ptr(i);
				*_ptr=ptr;
				(*_ptr)->refCount=1;
			}
			return	0;
		}

		void	Node::dump(const	char	*fileName){
		}
			
		void	Node::load(const	char	*fileName){
		}

		void	Node::send(_Message	*m){
		}

		int64	Node::time(){

			return	0;
		}
	}
}