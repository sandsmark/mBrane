#ifndef	_test_h
#define	_test_h

//	LIMITATION: DO NOT USE __COUNTER__

#include	"c.h"	//	user classes declarations

#define	APPLICATION_CLASSES	"application_classes.h"
#include	"..\Core\application.h"	//	std include; mandatory


extern	"C"{
mBrane::sdk::_Crank *	__cdecl	NewCR1(uint16);
}


#endif
