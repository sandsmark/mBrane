// node.cpp
//
// Author: Eric Nivel
//
//

#include	"node.h"


namespace	mBrane{
	namespace	sdk{

		Node	*Node::Singleton=NULL;

		inline	Node	*Node::Get(){

			return	Singleton;
		}

		Node::Node(){
			
			Singleton=this;
		}

		Node::~Node(){
		}
	}
}