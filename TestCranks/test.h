#ifndef	application_test_h
#define	application_test_h

//	LIMITATION: DO NOT USE __COUNTER__

#include	"3rd_party_library.h"	//	static linkage

#include	"c.h"	//	user classes declarations

#define	APPLICATION_CLASSES	"application_classes.h"
#include	"..\Core\application.h"	//	std include; mandatory


extern	"C"{
void	dll_export	Init();
}


#endif
