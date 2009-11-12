#ifndef	Perf_h
#define	Perf_h

/* __COUNTER__ is used in the mbrane header files to setup
 * various switch statements. This means that you can NOT
 * use it in any external DLLs header file
 *
 *	LIMITATION: DO NOT USE __COUNTER__
 */


/* if we are including any non-mbrane DLLs */
//#include	"3rd_party_library.h"

/* our class definitions
 * the 'APPLICATION_CLASSES' definition sets things up for mbrane
 */
#include	"Perf_class.h"
#define	APPLICATION_CLASSES	"../Perf/Perf_class_def.h"

/* mbrane includes. required. */
#include	"../Core/application.h"


extern	"C"{
void	dll_export	Init();
}


#endif /* Perf_h */
