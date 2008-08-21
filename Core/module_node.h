//	module_node.h
//
//	Author: Eric Nivel
//
//	BSD license:
//	Copyright (c) 2008, Eric Nivel, Thor List
//	All rights reserved.
//	Redistribution and use in source and binary forms, with or without
//	modification, are permitted provided that the following conditions are met:
//
//   - Redistributions of source code must retain the above copyright
//     notice, this list of conditions and the following disclaimer.
//   - Redistributions in binary form must reproduce the above copyright
//     notice, this list of conditions and the following disclaimer in the
//     documentation and/or other materials provided with the distribution.
//   - Neither the name of Eric Nivel, Thor List nor the
//     names of their contributors may be used to endorse or promote products
//     derived from this software without specific prior written permission.
//
//	THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND ANY
//	EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
//	WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
//	DISCLAIMED. IN NO EVENT SHALL THE REGENTS AND CONTRIBUTORS BE LIABLE FOR ANY
//	DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
//	(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
//	LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
//	ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//	(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
//	SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#ifndef	mBrane_sdk_module_node_h
#define	mBrane_sdk_module_node_h

#include	"payload.h"

//	Node API, as seen from the module
namespace	mBrane{
	namespace	sdk{
		namespace	module{

			class	_Module;
			class	dll	Node{
			private:
				static	Node	*Singleton;
			protected:
				static	const	uint16	NO_ID=0xFFFF;
				uint16	_ID;	//	max: 0xFFFE
				Node(uint16	ID);
				~Node();
			public:
				static	Node	*Get();
				uint16	ID()	const;
				virtual	_Module	*buildModule(uint16	CID,uint16	ID,uint16	clusterCID,uint16	clusterID)=0;
				virtual	void	start(_Module	*m)=0;
				virtual	void	stop(_Module	*m)=0;
				virtual	void	send(const	_Module	*sender,_Payload	*p)=0;
				virtual	int64	time()	const=0;	//	in ms since 01/01/70
			};
		}
	}
}


#endif
