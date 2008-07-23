#ifndef	_test_h
#define	_test_h

#include	"c.h"
#include	"dll.h"


extern "C" {
	DllExport mBrane::sdk::_Crank *	__cdecl	NewCR1(uint16);
}

USER_CLASSES_BEGIN	//	use only once
CLASS(Final_C1);
CLASS(Final_C2);
CLASS(Final_C3);
USER_CLASSES_END


#endif
