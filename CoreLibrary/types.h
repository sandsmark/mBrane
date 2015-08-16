/*
 * HUMANOBS - mBrane
 *
 * Eric Nivel
 * Center for Analysis and Design of Intelligent Agents
 *   Reykjavik University, Menntavegur 1, 101 Reykjavik, Iceland
 *   http://cadia.ru.is
 * Copyright(c)2012
 *
 * This software was developed by the above copyright holder as part of
 * the HUMANOBS EU research project, in collaboration with the
 * following parties:
 *
 * Autonomous Systems Laboratory
 *   Technical University of Madrid, Spain
 *   http://www.aslab.org/
 *
 * Communicative Machines
 *   Edinburgh, United Kingdom
 *   http://www.cmlabs.com/
 *
 * Istituto Dalle Molle di Studi sull'Intelligenza Artificiale
 *   University of Lugano and SUPSI, Switzerland
 *   http://www.idsia.ch/
 *
 * Institute of Cognitive Sciences and Technologies
 *   Consiglio Nazionale delle Ricerche, Italy
 *   http://www.istc.cnr.it/
 *
 * Dipartimento di Ingegneria Informatica
 *   University of Palermo, Italy
 *   http://roboticslab.dinfo.unipa.it/index.php/Main/HomePage
 *
 *
 * --- HUMANOBS Open-Source BSD License, with CADIA Clause v 1.0 ---
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, is permitted provided that the following conditions
 * are met:
 *
 * - Redistributions of source code must retain the above copyright
 * and collaboration notice, this list of conditions and the
 * following disclaimer.
 *
 * - Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following
 * disclaimer in the documentation and/or other materials provided
 * with the distribution.
 *
 * - Neither the name of its copyright holders nor the names of its
 * contributors may be used to endorse or promote products
 * derived from this software without specific prior written permission.
 *
 * - CADIA Clause: The license granted in and to the software under this
 * agreement is a limited-use license. The software may not be used in
 * furtherance of:
 * (i) intentionally causing bodily injury or severe emotional distress
 * to any person;
 * (ii) invading the personal privacy or violating the human rights of
 * any person; or
 * (iii) committing or preparing for any act of war.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef core_types_h
#define core_types_h

#include	<cstddef>

#if defined(WIN32)
#define	WINDOWS
#define	ARCH_32
#elif defined(WIN64)
#define	WINDOWS
#define	ARCH_64
#elif defined(__GNUC__)

//#if __GNUC__ == 4
//#if __GNUC_MINOR__ < 3
//#error "GNU C++ 4.3 or later is required to compile this program"
//#endif /* __GNUC_MINOR__ */
//#endif /* __GNUC__ */

#if defined(__x86_64)
#define	ARCH_64
#elif defined(__i386)
#define	ARCH_32
#endif
#if defined(__linux)
#define	LINUX
#endif
#endif

#if defined WINDOWS

#define	WIN32_LEAN_AND_MEAN
#define	_WIN32_WINNT	0x0501	//	i.e. win xp sp2
#include	<windows.h>
#ifndef AF_MAX
#include	<winsock2.h>
#endif

#include	<vector>
#include	<unordered_set>
#include	<unordered_map>
#define		UNORDERED_MAP		std::tr1::unordered_map
#define		UNORDERED_SET		std::tr1::unordered_set
#define		UNORDERED_MULTIMAP	std::tr1::unordered_multimap
#define		UNORDERED_MULTISET	std::tr1::unordered_multiset

#if defined	CORELIBRARY_EXPORTS
#define core_dll	__declspec(dllexport)
#else
#define core_dll	__declspec(dllimport)
#endif
#define	dll_export	__declspec(dllexport)
#define	dll_import	__declspec(dllimport)

#pragma	warning(disable:	4530)	//	warning: exception disabled
#pragma	warning(disable:	4996)	//	warning: this function may be unsafe
#pragma	warning(disable:	4800)	//	warning: forcing value to bool
#pragma	warning(disable:	4251)	//	warning: class xxx needs to have dll-interface to be used by clients of class yyy

#if( _SECURE_SCL != 0 )
#pragma message( "Warning: _SECURE_SCL != 0. You _will_ get either slowness or runtime errors." )
#endif

#if( _HAS_ITERATOR_DEBUGGING != 0 )
#pragma message( "Warning: _HAS_ITERATOR_DEBUGGING != 0. You _will_ get either slowness or runtime errors." )
#endif

#elif defined LINUX
#define core_dll
#include <iostream>
#include <string>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include <limits.h>

#include	<vector>
#include	<unordered_map>
#include	<unordered_set>
#define		UNORDERED_MAP	std::unordered_map
#define		UNORDERED_SET	std::unordered_set
#define		UNORDERED_MULTIMAP	std::unordered_multimap
#define		UNORDERED_MULTISET	std::unordered_multiset

#define dll_export __attribute((visibility("default")))
#define dll_import __attribute((visibility("default")))
#define cdecl

#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#define SOCKET_ERROR	-1
#define INVALID_SOCKET	-1
#define closesocket(X)	close(X)
#else
#error "This is a new platform"
#endif

#define	NEWLINE	'\n'

namespace	core
{

#define	WORD32_MASK					0xFFFFFFFF

#if defined	ARCH_32

#define	HALF_WORD_SHIFT				16
#define	HALF_WORD_HIGH_MASK			0xFFFF0000
#define	HALF_WORD_LOW_MASK			0x0000FFFF
#define	WORD_MASK					0xFFFFFFFF

#elif defined	ARCH_64
// UPS!
// typedef	unsigned	long			uint64;

#define	HALF_WORD_SHIFT				32
#define	HALF_WORD_HIGH_MASK			0xFFFFFFFF00000000
#define	HALF_WORD_LOW_MASK			0x00000000FFFFFFFF
#define	WORD_MASK					0xFFFFFFFFFFFFFFFF

#endif

#if defined	WINDOWS
typedef	HINSTANCE						shared_object;
typedef	HANDLE							thread;
#define thread_ret						uint32
#define thread_ret_val(ret)					return ret;
typedef	LPTHREAD_START_ROUTINE			thread_function;
#define	thread_function_call			WINAPI
typedef	SOCKET							socket;
typedef	HANDLE							semaphore;
typedef	HANDLE							mutex;
typedef	CRITICAL_SECTION				critical_section;
typedef	HANDLE							timer;
typedef	HANDLE							event;
#define	signal_handler_function_call	WINAPI
typedef	PHANDLER_ROUTINE				signal_handler;
#elif defined	LINUX
typedef void 							*shared_object;
typedef pthread_t						thread;
#define thread_ret						void *
#define thread_ret_val(ret)				pthread_exit((thread_ret)ret);
typedef thread_ret(*thread_function)(void *);
#define thread_function_call
typedef int								socket;
typedef struct sockaddr					SOCKADDR;
typedef	sem_t							semaphore;
typedef pthread_mutex_t					mutex;
typedef pthread_mutex_t					critical_section;
typedef timer_t							timer;
#define signal_handler_function_call
typedef sighandler_t					signal_handler;
#define stricmp							strcasecmp
#endif

}


#endif
