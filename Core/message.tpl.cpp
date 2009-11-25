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

			template<class	U,class	D,class	F,class	P,class	M>	inline	size_t	CompressedStreamData<U,D,F,P,M>::CoreSize(){

				return	___Payload<CompressedData,P,U,M>::Size()-2*sizeof(F);
			}

			template<class	U,class	D,class	F,class	P,class	M>	inline	CompressedStreamData<U,D,F,P,M>::CompressedStreamData():___Payload<CompressedData,P,U,M>(){
			}

			template<class	U,class	D,class	F,class	P,class	M>	CompressedStreamData<U,D,F,P,M>::~CompressedStreamData(){
			}

			////////////////////////////////////////////////////////////////////////////////////////////////

			template<class	U>	ControlMessage<U>::ControlMessage()/*:Payload<U,StaticData,Memory>()*/{

				this->_metaData = ControlMessage<U>::_MetaData | ControlMessage<U>::CONTROL<<2;
				//ControlMessage<U>::_metaData|=(ControlMessage<U>::CONTROL<<2);
			}

			template<class	U>	ControlMessage<U>::~ControlMessage(){
			}
			
			////////////////////////////////////////////////////////////////////////////////////////////////

			template<class	U,template<class>	class	A,class	M>	inline	StreamData<U,A,M>::StreamData(uint16	sid):Payload<U,A,M>(),_StreamData(sid){

				this->_metaData = StreamData<U,A,M>::_MetaData | StreamData<U,A,M>::STREAM<<2;
				// StreamData<U>::_metaData|=(StreamData<U>::STREAM<<2);
			}

			template<class	U,template<class>	class	A,class	M>	inline	StreamData<U,A,M>::~StreamData(){
			}

			template<class	U,template<class>	class	A,class	M>	_StreamData	*StreamData<U,A,M>::as_StreamData(){

				return	(_StreamData	*)(((uint8	*)this)+sizeof(Payload<U,A,M>));
			}

			////////////////////////////////////////////////////////////////////////////////////////////////

			template<class	U,template<class>	class	A,class	M>	Message<U,A,M>::Message():Payload<U,A,M>(),_Message(){

				this->_metaData = Message<U,A,M>::_MetaData | Message<U,A,M>::DATA<<2;
				//Message<U>::_metaData|=(Message<U>::DATA<<2);
			}

			template<class	U,template<class>	class	A,class	M>	Message<U,A,M>::~Message(){
			}

			template<class	U,template<class>	class	A,class	M>	_Message	*Message<U,A,M>::as_Message(){

				return	(_Message	*)(((uint8	*)this)+sizeof(Payload<U,A,M>));
			}
		}
	}
}
