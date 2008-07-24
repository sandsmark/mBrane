// config.h
//
// Author: Eric Nivel
//
//

#ifndef	mBrane_config_h
#define	mBrane_config_h


#define	WINDOWS
//#define	LINUX
//#define	OSX

#define	ARCH_32
//#define	ARCH_64


#if defined WINDOWS
	#if defined	BUILD_DLL
		#define dll	__declspec(dllexport)
	#else
		#define dll	__declspec(dllimport)
	#endif
#else
	#define dll
#endif


#endif