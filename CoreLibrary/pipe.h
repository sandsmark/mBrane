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

#ifndef core_pipe_h
#define core_pipe_h

#include	"utils.h"


#define	PIPE_1

namespace	core{

		//	Pipes are thread safe, depending on their type:
		//	11: 1 writer, 1 reader
		//	1N:	1 writer, N readers
		//	N1: N writers, 1 reader
		//	NN: N writers, N readers
#ifdef	PIPE_1
		template<typename	T,uint32_t _S>	class	Pipe11:
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
			int32_t head;
			int32_t tail;
			Block	*first;
			Block	*last;
			Block	*spare;
		protected:
			void	_clear();
			T		_pop();
		public:
			Pipe11();
			~Pipe11();
			void	clear();
			void	push(T	&t);	//	increases the size as necessary
			T		pop();			//	decreases the size as necessary
		};

		template<typename	T,uint32_t _S>	class	Pipe1N:
		public	Pipe11<T,_S>{
		private:
			CriticalSection	popCS;
		public:
			Pipe1N();
			~Pipe1N();
			void	clear();
			T		pop();
		};

		template<typename	T,uint32_t _S>	class	PipeN1:
		public	Pipe11<T,_S>{
		private:
			CriticalSection	pushCS;
		public:
			PipeN1();
			~PipeN1();
			void	clear();
			void	push(T	&t);
		};

		template<typename	T,uint32_t _S>	class	PipeNN:
		public	Pipe11<T,_S>{
		private:
			CriticalSection	pushCS;
			CriticalSection	popCS;
		public:
			PipeNN();
			~PipeNN();
			void	clear();
			void	push(T	&t);
			T		pop();
		};
#elif	defined	PIPE_2
		template<typename	T,uint32_t _S,class	Pipe>	class	Push1;
		template<typename	T,uint32_t _S,class	Pipe>	class	PushN;
		template<typename	T,uint32_t _S,class	Pipe>	class	Pop1;
		template<typename	T,uint32_t _S,class	Pipe>	class	PopN;

		//	a Pipe<T,_S> is a linked list of blocks containing _S objects of type T
		//	push() adds an object at the tail of the last block and moves the tail forward; when the tail reaches the end of the last block, a new block is appended to the list
		//	pop() moves the head forward and returns the object at this location; when the head reaches the end of the first block, this block is deallocated
		//	Synchronization between readers and writers is lock-free under no contention (reqCount), and uses a lock (Semaphore::) under contention
		//	single writer pipes can use an int32_t tail, whereas multiple writer versions require int32_t volatile tail; idem for readers
		//	The Head and Tail argument is meant to allow the parameterizing of heads and tails
		//	Push and Pop are functors tailored to the multiplicity of resp. the read and write threads
		//	P is the actual pipe class (e.g. Pipe11, etc.)
		template<typename	T,uint32_t _S,typename	Head,typename	Tail,class	P,template<typename,uint32_t,class>	class	Push,template<typename,uint32_t,class>	class	Pop>	class	Pipe:
		public	Semaphore{
		template<typename	T,uint32_t _S,class	Pipe>	friend	class	Push1;
		template<typename	T,uint32_t _S,class	Pipe>	friend	class	PushN;
		template<typename	T,uint32_t _S,class	Pipe>	friend	class	Pop1;
		template<typename	T,uint32_t _S,class	Pipe>	friend	class	PopN;
		protected:
			class	Block{
			public:
				T	buffer[_S];
				Block	*next;
				Block(Block	*prev):next(NULL){	if(prev)prev->next=this;	}
				~Block(){	if(next)	delete	next;	}
			};
			Block	*first;
			Block	*last;
			Block	*spare;	//	pipes always retain a spare block: if a block is to be deallocated and there is no spare, it becomes the spare

			Head	head;	//	starts at -1
			Tail	tail;	//	starts at 0
			int32_t volatile	waitingList;	//	amount of readers that have to wait, negative value indicate free lunch

			Push<T,_S,P>	*_push;
			Pop<T,_S,P>		*_pop;

			void	shrink();	//	deallocates the first block when head reaches its end
			void	grow();		//	allocate a new last block when tail reaches its end

			Pipe();
		public:
			~Pipe();
			void	push(T	&t);
			T		pop();
		};

		template<class	Pipe>	class	PipeFunctor{
		protected:
			Pipe	&const	pipe;
			PipeFunctor(Pipe	&p);
		};

		template<typename	T,uint32_t _S,class	Pipe>	class	Push1:
		public	PipeFunctor<Pipe>{
		public:
			Push1(Pipe	&p);
			void	operator	()(T	&t);
		};

		template<typename	T,uint32_t _S,class	Pipe>	class	PushN:
		public	PipeFunctor<Pipe>,
		public	Semaphore{
		public:
			PushN(Pipe	&p);
			void	operator	()(T	&t);
		};

		template<typename	T,uint32_t _S,class	Pipe>	class	Pop1:
		public	PipeFunctor<Pipe>{
		public:
			Pop1(Pipe	&p);
			T	operator	()();
		};

		template<typename	T,uint32_t _S,class	Pipe>	class	PopN:
		public	PipeFunctor<Pipe>,
		public	Semaphore{
		public:
			PopN(Pipe	&p);
			T	operator	()();
		};

		template<typename	T,uint32_t S>	class	Pipe11:
		public	Pipe<T,S,int32_t,int32_t,Pipe11<T,S>,Push1,Pop1>{
		public:
			Pipe11();
			~Pipe11();
		};

		template<typename	T,uint32_t S>	class	Pipe1N:
		public	Pipe<T,S,int32_t,int32_t volatile,Pipe1N<T,S>,Push1,PopN>{
		public:
			Pipe1N();
			~Pipe1N();
		};

		template<typename	T,uint32_t S>	class	PipeN1:
		public	Pipe<T,S,int32_t volatile,int32_t,PipeN1<T,S>,PushN,Pop1>{
		public:
			PipeN1();
			~PipeN1();
		};

		template<typename	T,uint32_t S>	class	PipeNN:
		public	Pipe<T,S,int32_t volatile,int32_t volatile,PipeNN<T,S>,PushN,PopN>{
		public:
			PipeNN();
			~PipeNN();
		};
#endif
}


#include	"pipe.tpl.cpp"


#endif
