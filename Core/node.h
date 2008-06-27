// node.h
//
// Author: Eric Nivel
//
//

#ifndef	mBrane_sdk_node_h
#define	mBrane_sdk_node_h

#include	"payload.h"


namespace	mBrane{
	namespace	sdk{

		class	Node{	//	TODO: define the actual Node here instead of an interface: save the virtual calls => make core.lib a dll (with a .def file)
		private:
			static	Node	*Singleton;
		protected:
			Node();
			virtual	~Node();
		public:
			static	Node	*Get();
			virtual	int8	beginTransmission()=0;
			virtual	int8	endTransmission()=0;
			virtual	int8	beginReception()=0;
			virtual	int8	endReception()=0;
			virtual	int8	send(uint8	*b,size_t	s)=0;
			virtual	int8	receive(uint8	*b,size_t	s)=0;
		};
	}
}


#endif
