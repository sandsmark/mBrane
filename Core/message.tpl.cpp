//	message.tpl.cpp
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

namespace	mBrane{
	namespace	sdk{
		namespace	payloads{

			template<class	S,class	D,class	F>	inline	const	size_t	CompressedData<S,D,F>::CoreSize(){

				return	Size()-2*sizeof(F);
			}

			template<class	S,class	D,class	F>	CompressedData<S,D,F>::CompressedData():S(),_CompressedData(),D(){
			}

			////////////////////////////////////////////////////////////////////////////////////////////////

			template<class	Data>	inline	ModuleData<Data>::ModuleData():Payload<Memory,ModuleData<Data> >(),Data(){
			}
			
			template<class	Data>	inline	ModuleData<Data>::~ModuleData(){
			}

			////////////////////////////////////////////////////////////////////////////////////////////////

			template<class	U>	ControlMessage<U>::ControlMessage():Payload<Memory,U>(){

				_metaData|=(CONTROL<<2);
			}

			template<class	U>	ControlMessage<U>::~ControlMessage(){
			}

			////////////////////////////////////////////////////////////////////////////////////////////////

			template<class	U>	Message<U>::Message():Payload<Memory,U>(),_Message(){

				_metaData|=(DATA<<2);
			}

			template<class	U>	inline	Message<U>::operator	_Message	*()	const{

				return	(_Message	*)this;
			}

			////////////////////////////////////////////////////////////////////////////////////////////////

			template<class	U>	inline	StreamData<U>::StreamData(uint16	sid):Payload<Memory,U>(),_StreamData(sid){

				_metaData|=(STREAM<<2);
			}

			template<class	U>	inline	StreamData<U>::operator	_StreamData	*()	const{

				return	(_StreamData	*)this;
			}
		}
	}
}