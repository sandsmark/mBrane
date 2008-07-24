// node.h
//
// Author: Eric Nivel
//
//

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
			void	send(sdk::_Message	*m);
			int64	time();
		};
	}
}


#endif
