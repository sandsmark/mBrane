//	types.h
//
//	Author: Eric Nivel
//
//	BSD license:
//	Copyright (c) 2008, Eric Nivel
//	All rights reserved.
//	Redistribution and use in source and binary forms, with or without
//	modification, are permitted provided that the following conditions are met:
//
//   - Redistributions of source code must retain the above copyright
//     notice, this list of conditions and the following disclaimer.
//   - Redistributions in binary form must reproduce the above copyright
//     notice, this list of conditions and the following disclaimer in the
//     documentation and/or other materials provided with the distribution.
//   - Neither the name of Eric Nivel nor the
//     names of their contributors may be used to endorse or promote products
//     derived from this software without specific prior written permission.
//
//	THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND ANY
//	EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
//	WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
//	DISCLAIMED. IN NO EVENT SHALL THE REGENTS AND CONTRIBUTORS BE LIABLE FOR ANY
//	DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
//	(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
//	LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
//	ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//	(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
//	SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#ifndef mBrane_types_h
#define mBrane_types_h

#include	<cstddef>

#include	"config.h"

#if defined WINDOWS
	#define	WIN32_LEAN_AND_MEAN
	#define	_WIN32_WINNT	0x0501	//	i.e. win xp sp2
	#include	<windows.h>
	#include	<winsock2.h>

	#if defined	BUILD_DLL
		#define dll	__declspec(dllexport)
	#else
		#define dll	__declspec(dllimport)
	#endif
	#define	dll_export	__declspec(dllexport)
	#define	dll_import	__declspec(dllimport)
	//#define cdecl		__cdecl		// no need to define this as the Windows do it anyway

	#pragma	warning(disable:	4530)	//	warning: exception disabled
	#pragma	warning(disable:	4996)	//	warning: this function may be unsafe
	#pragma	warning(disable:	4800)	//	warning: forcing value to bool
#elif defined LINUX
	#define dll
	#include <iostream>
	#include <string>
	#include <pthread.h>
	#include <semaphore.h>
	#include <signal.h>
	#include <limits.h>

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

namespace	mBrane{

#define	WORD32_MASK					0xFFFFFFFF

#if defined	ARCH_32

	typedef	unsigned	int				word32;
	typedef	unsigned	short			word16;

	typedef	char						int8;
	typedef	unsigned	char			uint8;
	typedef	short						int16;
	typedef	unsigned	short			uint16;
	typedef	long						int32;
	typedef	unsigned	long			uint32;
	typedef	long		long			int64;
	typedef	unsigned	long	long	uint64;
	typedef	float						float32;
	typedef	double						float64;

	typedef	word32						word;
	typedef	word16						half_word;

	#define	HALF_WORD_SHIFT				16
	#define	HALF_WORD_HIGH_MASK			0xFFFF0000
	#define	HALF_WORD_LOW_MASK			0x0000FFFF
	#define	WORD_MASK					0xFFFFFFFF

#elif defined	ARCH_64

	typedef	unsigned	int				word32;
	typedef	unsigned	long			word64;

	typedef	char						int8;
	typedef	unsigned	char			uint8;
	typedef	short						int16;
	typedef	unsigned	short			uint16;
	typedef	int							int32;
	typedef	unsigned	int				uint32;
	typedef	long						int64;
	typedef	unsigned	long			uint64;
	typedef	float						float32;
	typedef	double						float64;

	typedef	word64						word;
	typedef	word32						half_word;

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
	#define	signal_handler_function_call	WINAPI
	typedef	PHANDLER_ROUTINE				signal_handler;
#elif defined	LINUX
	typedef void *							shared_object;
	typedef pthread_t						thread;
	#define thread_ret						void *
	#define thread_ret_val(ret)					pthread_exit((thread_ret)ret);
	typedef thread_ret (*thread_function)(void *);
	#define thread_function_call
	typedef int							socket;
	typedef struct sockaddr						SOCKADDR;
	typedef	sem_t							semaphore;
	typedef pthread_mutex_t						mutex;
	typedef pthread_mutex_t						critical_section;
	typedef timer_t							timer;
	#define signal_handler_function_call
	typedef sighandler_t						signal_handler;
	#define stricmp strcasecmp
#elif defined	OSX
	#define stricmp strcasecmp
#endif

}


#endif
