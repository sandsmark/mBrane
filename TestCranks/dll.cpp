#include "dll.h"


#if defined WINDOWS
	BOOL	APIENTRY	DllMain(HANDLE	hModule,DWORD	ul_reason_for_call,LPVOID	lpReserved){
		return TRUE;
	}
#else

#endif