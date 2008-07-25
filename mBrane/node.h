// node.h
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

#ifndef	mBrane_node_node_h
#define	mBrane_node_node_h

#include	"..\Core\node.h"

#if defined WINDOWS
	#include	<windows.h>
#elif defined	LINUX
#elif defined	OSX
#endif


namespace	mBrane{
	namespace	node{

		class	Node:
		public	sdk::Node{
		private:
			const	char	*application_configuration_file;
			int16	send(uint8	*b,size_t	s);
			int16	recv(uint8	*b,size_t	s,bool	peek=false);
			int16	send(sdk::_Payload	*p);	//	return 0 if successfull, error code (>0) otherwise
			int16	recv(sdk::_Payload	**p);
#if defined	WINDOWS
			HINSTANCE	userLibrary;
#elif defined	LINUX
#elif defined	OSX
#endif
			void	loadUserLibrary(const	char	*fileName);
			void	unloadUserLibrary();
			bool	_shutdown;
		public:
			Node(const	char	*configFileName);
			~Node();
			void	run();
			void	shutdown();
			void	dump(const	char	*fileName);	//	dumps the current system state
			void	load(const	char	*fileName);	//	initializes itself from a previously saved system state
			void	loadApplication(const	char	*fileName=NULL);
			void	unloadApplication();
			void	send(uint16	crankID,sdk::_Message	*m);
			void	send(uint16	crankID,sdk::_ControlMessage	*m);
			int64	time();
		};
	}
}


#endif
