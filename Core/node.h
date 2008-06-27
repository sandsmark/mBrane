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

		class	Node{
		private:
			static	Node	*Singleton;
			uint16	_ID;
		public:
			static	Node	*Get();
			Node();
			~Node();
			uint16	ID()	const;
			void	run();
			int8	beginTransmission();
			int8	endTransmission();
			int8	beginReception();
			int8	endReception();
			int8	send(uint8	*b,size_t	s);
			int8	receive(uint8	*b,size_t	s);
		};
	}
}


#endif
