//	config.h
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

#ifndef	mBrane_config_h
#define	mBrane_config_h

#if defined(WIN32)
	#define	WINDOWS
	#define	ARCH_32
#elif defined(WIN64)
	#define	WINDOWS
	#define	ARCH_64
#elif defined(__GNUC__)

#if __GNUC__ == 4
#if __GNUC_MINOR__ < 3
#error "GNU C++ 4.3 or later is required to compile this program"
#endif /* __GNUC_MINOR__ */
#endif /* __GNUC__ */

	#if defined(__x86_64)
		#define	ARCH_64
	#elif defined(__i386)
		#define	ARCH_32
	#endif
	#if defined(__linux)
		#define	LINUX
	#elif defined(__APPLE__)
		#define	OSX
	#endif
#endif

#define	MBRANE_VERSION	0.0

//	class version selectors

//#define	MEMORY_MALLOC
#define	MEMORY_1
//#define	MEMORY_2

#define	PIPE_1
//#define	PIPE_2

#endif
