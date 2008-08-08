//	message.tpl.cpp
//
//	Author: Eric Nivel
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

namespace	mBrane{
	namespace	sdk{
		namespace	payloads{

			template<class	Data>	inline	CrankData<Data>::CrankData():Payload<Memory,CrankData<Data> >(),Data(){
			}
			
			template<class	Data>	inline	CrankData<Data>::~CrankData(){
			}

			template<class	Data>	inline	bool	CrankData<Data>::isCrankData()	const{

				return	true;
			}

			////////////////////////////////////////////////////////////////////////////////////////////////

			template<class	U>	inline	ControlMessage<U>::ControlMessage(uint32	mid,uint8	priority):Payload<Memory,U>(),_ControlMessage(mid,priority){
			}

			template<class	U>	inline	bool	ControlMessage<U>::isControlMessage(){

				return	true;
			}

			////////////////////////////////////////////////////////////////////////////////////////////////

			template<class	U>	inline	Message<U>::Message(uint32	mid,uint8	priority):ControlMessage<U>(mid,priority){
			}

			template<class	U>	inline	bool	Message<U>::isControlMessage(){

				return	false;
			}

			template<class	U>	inline	bool	Message<U>::isMessage(){

				return	true;
			}

			////////////////////////////////////////////////////////////////////////////////////////////////

			template<class	U>	inline	StreamData<U>::StreamData(uint32	mid,uint8	priority):ControlMessage<U>(mid,priority){
			}

			template<class	U>	inline	bool	StreamData<U>::isControlMessage(){

				return	false;
			}

			template<class	U>	inline	bool	StreamData<U>::isStreamData(){

				return	true;
			}

			////////////////////////////////////////////////////////////////////////////////////////////////

			template<class	S,class	D,class	F>	inline	const	size_t	CompressedPayload<S,D,F>::CoreSize(){

				return	Size()-2*sizeof(F);
			}

			template<class	S,class	D,class	F>	CompressedPayload<S,D,F>::CompressedPayload():S(),_CompressedPayload(),D(){
			}
		}
	}
}