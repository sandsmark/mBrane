// node.h
//
// Author: Eric Nivel
//
//

#ifndef	mBrane_sdk_node_h
#define	mBrane_sdk_node_h

#include	"message.h"


namespace	mBrane{
	namespace	sdk{

		class	dll	Node{
		protected:
			static	Node	*Singleton;
			uint16	_ID;
			Node();
			virtual	~Node();
		public:
			static	Node	*Get();
			uint16	ID()	const;
			virtual	void	send(uint16	crankID,_Message	*m)=0;
			virtual	void	send(uint16	crankID,_ControlMessage	*m)=0;
			virtual	int64	time()=0;	//	in ms since 01/01/70
		};
	}
}


#endif
