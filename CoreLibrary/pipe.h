//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
//_/_/
//_/_/ HUMANOBS - CoreLibrary
//_/_/
//_/_/ Eric Nivel, Thor List
//_/_/ Center for Analysis and Design of Intelligent Agents
//_/_/   Reykjavik University, Menntavegur 1, 101 Reykjavik, Iceland
//_/_/   http://cadia.ru.is
//_/_/ Copyright(c)2012
//_/_/
//_/_/ This software was developed by the above copyright holder as part of 
//_/_/ the HUMANOBS EU research project, in collaboration with the 
//_/_/ following parties:
//_/_/ 
//_/_/ Autonomous Systems Laboratory
//_/_/   Technical University of Madrid, Spain
//_/_/   http://www.aslab.org/
//_/_/
//_/_/ Communicative Machines
//_/_/   Edinburgh, United Kingdom
//_/_/   http://www.cmlabs.com/
//_/_/
//_/_/ Istituto Dalle Molle di Studi sull'Intelligenza Artificiale
//_/_/   University of Lugano and SUPSI, Switzerland
//_/_/   http://www.idsia.ch/
//_/_/
//_/_/ Institute of Cognitive Sciences and Technologies
//_/_/   Consiglio Nazionale delle Ricerche, Italy
//_/_/   http://www.istc.cnr.it/
//_/_/
//_/_/ Dipartimento di Ingegneria Informatica
//_/_/   University of Palermo, Italy
//_/_/   http://roboticslab.dinfo.unipa.it/index.php/Main/HomePage
//_/_/
//_/_/
//_/_/ --- HUMANOBS Open-Source BSD License, with CADIA Clause v 1.0 ---
//_/_/
//_/_/ Redistribution and use in source and binary forms, with or without 
//_/_/ modification, is permitted provided that the following conditions 
//_/_/ are met:
//_/_/
//_/_/ - Redistributions of source code must retain the above copyright 
//_/_/ and collaboration notice, this list of conditions and the 
//_/_/ following disclaimer.
//_/_/
//_/_/ - Redistributions in binary form must reproduce the above copyright 
//_/_/ notice, this list of conditions and the following
//_/_/ disclaimer in the documentation and/or other materials provided 
//_/_/ with the distribution.
//_/_/
//_/_/ - Neither the name of its copyright holders nor the names of its 
//_/_/ contributors may be used to endorse or promote products 
//_/_/ derived from this software without specific prior written permission.
//_/_/
//_/_/ - CADIA Clause: The license granted in and to the software under this 
//_/_/ agreement is a limited-use license. The software may not be used in 
//_/_/ furtherance of: 
//_/_/ (i) intentionally causing bodily injury or severe emotional distress 
//_/_/ to any person; 
//_/_/ (ii) invading the personal privacy or violating the human rights of 
//_/_/ any person; or 
//_/_/ (iii) committing or preparing for any act of war.
//_/_/
//_/_/ THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
//_/_/ "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
//_/_/ LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR 
//_/_/ A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
//_/_/ OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
//_/_/ SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
//_/_/ LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, 
//_/_/ DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY 
//_/_/ THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
//_/_/ (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
//_/_/ OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//_/_/
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

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
			T		_pop();
		public:
			Pipe11();
			~Pipe11();
			void	clear();
			void	push(T	&t);	//	increases the size as necessary
			T		pop();			//	decreases the size as necessary
		};

		template<typename	T,uint32	_S>	class	Pipe1N:
		public	Pipe11<T,_S>{
		private:
			CriticalSection	popCS;
		public:
			Pipe1N();
			~Pipe1N();
			void	clear();
			T		pop();
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
			T		pop();
		};
#elif	defined	PIPE_2
		template<typename	T,uint32	_S,class	Pipe>	class	Push1;
		template<typename	T,uint32	_S,class	Pipe>	class	PushN;
		template<typename	T,uint32	_S,class	Pipe>	class	Pop1;
		template<typename	T,uint32	_S,class	Pipe>	class	PopN;

		//	a Pipe<T,_S> is a linked list of blocks containing _S objects of type T
		//	push() adds an object at the tail of the last block and moves the tail forward; when the tail reaches the end of the last block, a new block is appended to the list
		//	pop() moves the head forward and returns the object at this location; when the head reaches the end of the first block, this block is deallocated
		//	Synchronization between readers and writers is lock-free under no contention (reqCount), and uses a lock (Semaphore::) under contention
		//	single writer pipes can use an int32 tail, whereas multiple writer versions require int32 volatile tail; idem for readers
		//	The Head and Tail argument is meant to allow the parameterizing of heads and tails
		//	Push and Pop are functors tailored to the multiplicity of resp. the read and write threads
		//	P is the actual pipe class (e.g. Pipe11, etc.)
		template<typename	T,uint32	_S,typename	Head,typename	Tail,class	P,template<typename,uint32,class>	class	Push,template<typename,uint32,class>	class	Pop>	class	Pipe:
		public	Semaphore{
		template<typename	T,uint32	_S,class	Pipe>	friend	class	Push1;
		template<typename	T,uint32	_S,class	Pipe>	friend	class	PushN;
		template<typename	T,uint32	_S,class	Pipe>	friend	class	Pop1;
		template<typename	T,uint32	_S,class	Pipe>	friend	class	PopN;
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
			int32	volatile	waitingList;	//	amount of readers that have to wait, negative value indicate free lunch

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

		template<typename	T,uint32	_S,class	Pipe>	class	Push1:
		public	PipeFunctor<Pipe>{
		public:
			Push1(Pipe	&p);
			void	operator	()(T	&t);
		};

		template<typename	T,uint32	_S,class	Pipe>	class	PushN:
		public	PipeFunctor<Pipe>,
		public	Semaphore{
		public:
			PushN(Pipe	&p);
			void	operator	()(T	&t);
		};

		template<typename	T,uint32	_S,class	Pipe>	class	Pop1:
		public	PipeFunctor<Pipe>{
		public:
			Pop1(Pipe	&p);
			T	operator	()();
		};

		template<typename	T,uint32	_S,class	Pipe>	class	PopN:
		public	PipeFunctor<Pipe>,
		public	Semaphore{
		public:
			PopN(Pipe	&p);
			T	operator	()();
		};

		template<typename	T,uint32	S>	class	Pipe11:
		public	Pipe<T,S,int32,int32,Pipe11<T,S>,Push1,Pop1>{
		public:
			Pipe11();
			~Pipe11();
		};

		template<typename	T,uint32	S>	class	Pipe1N:
		public	Pipe<T,S,int32,int32	volatile,Pipe1N<T,S>,Push1,PopN>{
		public:
			Pipe1N();
			~Pipe1N();
		};

		template<typename	T,uint32	S>	class	PipeN1:
		public	Pipe<T,S,int32	volatile,int32,PipeN1<T,S>,PushN,Pop1>{
		public:
			PipeN1();
			~PipeN1();
		};

		template<typename	T,uint32	S>	class	PipeNN:
		public	Pipe<T,S,int32	volatile,int32	volatile,PipeNN<T,S>,PushN,PopN>{
		public:
			PipeNN();
			~PipeNN();
		};
#endif
}


#include	"pipe.tpl.cpp"


#endif
