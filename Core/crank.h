// crank.h
//
// Author: Eric Nivel
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

#ifndef mBrane_sdk_crank_h
#define mBrane_sdk_crank_h

#include	"payload.h"
#include	"circular_buffer.h"


namespace	mBrane{
	namespace	sdk{

		class	dll	_Crank:
		public	CircularBuffer<P<_Payload> >{
		protected:
			uint16	_ID;
			bool	_canMigrate;
			bool	_canBeSwapped;
			_Crank(uint16	_ID,bool	canMigrate=true,bool	canBeSwapped=true);
			void	peek(int32	depth);	//	-1 means all messages
			int64	time();
			void	send(_Payload	*p);
		public:
			static	void	Build(uint16	CID);
			virtual	~_Crank();
			uint16	id()	const;
			bool	canMigrate();	//	on another node; dynamic
			bool	canBeSwapped();	//	on another thread within the same node; dynamic
			virtual	uint32	dumpSize();	//	dynamic
			virtual	uint8	*dump();	//	dumps the current state; can be called anytime
			virtual	void	load(uint8	*chunk);	//	initializes itself from a previously saved state
			virtual	void	start();	//	called when the crank is loaded in a thread for the first time, i.e. at node starting time
			virtual	void	stop();	//	called just before the crank is unloaded from the thread for the last time, i.e. at node shutdown time
			virtual	void	swapOut();	//	called when the crank is unloaded from its current thread for swapping
			virtual	void	swapIn();	//	called when the crank is loaded in a new thread after having been swapped out
			virtual	void	migrateOut();	//	callled when the crank is unloaded from its current thread for migration
			virtual	void	migrateIn();	//	called when the crank is loaded in a new thread after having migrated
			virtual	void	notify(_Payload	*p)=0;	//	called when the crank receives a message
			virtual	bool	preview(_Payload	*p)=0;	//	calls triggered by peek(): one per message peeked at
			//	TODO: get actual count
		};
	}
}


#endif
