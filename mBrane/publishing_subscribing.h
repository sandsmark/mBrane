//	publishing_subscribing.h
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

#ifndef	mBrane_publishing_subscribing_h
#define	mBrane_publishing_subscribing_h

#include	"..\Core\list.h"
#include	"..\Core\pipe.h"
#include	"..\Core\crank.h"


using	namespace	mBrane::sdk;
using	namespace	mBrane::sdk::crank;

namespace	mBrane{

	class	OrderedMessagingEngine;
	template<class	Engine>	class	Messaging;
	class	PublishingSubscribing{
	friend	class	OrderedMessagingEngine;
	template<class	Engine>	friend	class	Messaging;
	protected:
		typedef	struct{
			uint32	activationCount;
			List<_Crank	*>	*cranks;
		}NodeEntry;
		
		Array<Array<Array<NodeEntry>	*>	*>	routes;
		CriticalSection							routesCS;
		
		//	TODO:	groups,crank descriptors
		PublishingSubscribing();
		~PublishingSubscribing();
	};
}


#endif