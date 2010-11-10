#ifndef mbrane_h
#define mbrane_h

#include "types.h"

	#if defined	MBRANELIB_EXPORTS
		#define mbrane_dll	__declspec(dllexport)
	#else
		#define mbrane_dll	__declspec(dllimport)
	#endif
	#define	dll_export	__declspec(dllexport)
	#define	dll_import	__declspec(dllimport)

#endif // mbrane_h
