// node.h
//
// Author: Eric Nivel
//
//

#ifndef	mBrane_node_node_h
#define	mBrane_node_node_h

#include	"..\Core\node.h"


namespace	mBrane{
	namespace	node{

		class	Node:
		public	mBrane::sdk::Node{
		private:
			uint16	_ID;
		public:
			Node();
			~Node();
			uint16	ID();
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