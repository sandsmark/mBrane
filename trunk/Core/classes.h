/*
 * unusually this file does not perform any guarding against multiple inclusions
 * because that is intended!
 *
 * you should not include this header into anything apart from application.h or library.h
 */

/* the library version of what to include */
#ifdef _library_h_
#include	MBRANE_MESSAGE_CLASSES
#include	LIBRARY_CLASSES	
#endif /* _library_h_ */

/* and what applications want */
#ifdef _application_h_
#ifndef	LIBRARY_CLASSES	
	#include MBRANE_MESSAGE_CLASSES	
#else
	#include LIBRARY_CLASSES
#endif
#include APPLICATION_CLASSES
#endif /* _application_h_ */
