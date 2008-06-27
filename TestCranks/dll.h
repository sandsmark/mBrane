#ifndef dll_h
#define dll_h

#include	"..\Core\mBrane.h"


#if defined WINDOWS
	#include	<windows.h>
	#define DllExport   __declspec(dllexport)
#else
	#define DllExport
#endif


#endif