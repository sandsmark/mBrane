#ifndef	_test_h
#define	_test_h

#include	"c.h"
#include	"dll.h"


extern "C" {
	DllExport mBrane::sdk::_Crank *	__cdecl	NewCR1();
}

USER_CLASSES_BEGIN	//	use only once
CLASS(_C1);
CLASS(__C2);
CLASS(_C3);
USER_CLASSES_END


#endif
