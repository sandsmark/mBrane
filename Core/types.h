// types.h
//
// Author: Eric Nivel
//
//	BSD license:
//	Copyright (c) 2008, Eric Nivel, Thor List
//	All rights reserved.
//	Redistribution and use in source and binary forms, with or without
//	modification, are permitted provided that the following conditions are met:
//
//   - Redistributions of source code must retain the above copyright
//     notice, this list of conditions and the following disclaimer.
//   - Redistributions in binary form must reproduce the above copyright
//     notice, this list of conditions and the following disclaimer in the
//     documentation and/or other materials provided with the distribution.
//   - Neither the name of Eric Nivel, Thor List nor the
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

#include	<stddef.h>

#include	"config.h"

#if defined WINDOWS
	#include	<windows.h>

	#if defined	BUILD_DLL
		#define dll	__declspec(dllexport)
	#else
		#define dll	__declspec(dllimport)
	#endif
#else
	#define dll
#endif

namespace	mBrane{

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
	typedef	HINSTANCE				shared_object;
	typedef	HANDLE					thread;
	typedef	LPTHREAD_START_ROUTINE	thread_function;
	#define	thread_function_call	WINAPI
#elif defined	LINUX
#elif defined	OSX
#endif

}


#endif