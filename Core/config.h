// config.h
//
// Authir: Eric Nivel
//
//

#ifndef mBrane_config_h
#define	mBrane_config_h


#define	WINDOWS
//#define	LINUX
//#define	MAC_OSX

#define	ARCH_32
//#define	ARCH_64


#if defined WINDOWS
	#define DllExport   __declspec(dllexport)
#else
	#define DllExport
#endif


#endif