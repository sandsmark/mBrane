#ifndef mbrane_h
#define mbrane_h

#include "types.h"

#if defined WINDOWS
	#if defined	MBRANELIB_EXPORTS
		#define mbrane_dll	__declspec(dllexport)
	#else
		#define mbrane_dll	__declspec(dllimport)
	#endif
#elif defined LINUX
	#define mbrane_dll
#endif

#endif // mbrane_h
