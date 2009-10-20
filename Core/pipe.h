//	pipe.h
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

#ifndef mBrane_sdk_pipe_h
#define mBrane_sdk_pipe_h

#include	"utils.h"


namespace	mBrane{
	namespace	sdk{

		//	Pipes are thread safe, depending on their control:
		//	11: 1 writer, 1 reader
		//	1N:	1 writer, N readers
		//	N1: N writers, 1 reader
		//	NN: N writers, N readers
		template<typename	T,uint32	_S>	class	Pipe11:
		public	Semaphore,
		public	CriticalSection{
		private:
			class	Block{
			public:
				T	buffer[_S*sizeof(T)];
				Block	*next;
				Block(Block	*prev):next(NULL){	if(prev)prev->next=this;	}
				~Block(){	if(next)	delete	next;	}
			};
			int32	head;
			int32	tail;
			Block	*first;
			Block	*last;
			Block	*spare;
		protected:
			void	_clear();
			T		*_pop();
		public:
			Pipe11();
			~Pipe11();
			void	clear();
			void	push(T	&t);	//	increases the size if necessary
			T		*pop();			//	decreases the size as necessary
		};

		template<typename	T,uint32	_S>	class	Pipe1N:
		public	Pipe11<T,_S>{
		private:
			CriticalSection	popCS;
		public:
			Pipe1N();
			~Pipe1N();
			void	clear();
			T		*pop();
		};

		template<typename	T,uint32	_S>	class	PipeN1:
		public	Pipe11<T,_S>{
		private:
			CriticalSection	pushCS;
		public:
			PipeN1();
			~PipeN1();
			void	clear();
			void	push(T	&t);
		};

		template<typename	T,uint32	_S>	class	PipeNN:
		public	Pipe11<T,_S>{
		private:
			CriticalSection	pushCS;
			CriticalSection	popCS;
		public:
			PipeNN();
			~PipeNN();
			void	clear();
			void	push(T	&t);
			T		*pop();
		};
	}
}


#include	"pipe.tpl.cpp"


#endif
