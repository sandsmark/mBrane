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

		class	DllExport	Node{
		private:
			static	Node	*Singleton;
			uint16	_ID;
			int16	_send(uint8	*b,size_t	s);
			int16	_recv(uint8	*b,size_t	s,bool	peek=false);
		public:
			static	Node	*Get();
			Node(const	char	*configFileName);
			~Node();
			uint16	ID()	const;
			void	run();
			int16	_send(_Payload	*p);	//	return 0 if successfull, error code (>0) if not
			int16	_recv(_Payload	**p);
		};
	}
}


#endif
