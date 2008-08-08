//	message.cpp
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

#include	"message.h"


namespace	mBrane{
	namespace	sdk{
		namespace	payloads{

			inline	_ControlMessage::_ControlMessage(uint32	mid,uint8	priority):_mid(mid),_priority(_priority),_senderNodeID(0){
			}

			inline	_ControlMessage::~_ControlMessage(){
			}

			inline	_ControlMessage::operator	_Payload	*()	const{

				return	(_Payload	*)(((uint8	*)this)-sizeof(_Payload));
			}

			inline	uint32	&_ControlMessage::mid(){

				return	_mid;
			}

			inline	uint8	&_ControlMessage::priority(){

				return	_priority;
			}

			inline	uint16	&_ControlMessage::senderNode_id(){

				return	_senderNodeID;
			}

			////////////////////////////////////////////////////////////////////////////////////////////////

			inline	_StreamData::_StreamData(){
			}

			inline	_StreamData::~_StreamData(){
			}

			inline	_StreamData::operator	_ControlMessage	*()	const{

				return	(_ControlMessage	*)(((uint8	*)this)-sizeof(_ControlMessage));
			}

			inline	_StreamData::operator	_Payload	*()	const{

				return	(_Payload	*)(((uint8	*)this)-sizeof(_ControlMessage)-sizeof(_Payload));
			}

			////////////////////////////////////////////////////////////////////////////////////////////////

			inline	_DynamicData::_DynamicData(){
			}

			inline	_DynamicData::~_DynamicData(){
			}

			inline	bool	_DynamicData::isDynamicData()	const{

				return	true;
			}

			////////////////////////////////////////////////////////////////////////////////////////////////

			inline	_CompressedPayload::_CompressedPayload():_DynamicData(),isCompressed(false){
			}

			inline	_CompressedPayload::~_CompressedPayload(){
			}

			inline	bool	_CompressedPayload::isCompressedPayload()	const{

				return	true;
			}

			inline	void	_CompressedPayload::compress(){
			}

			inline	void	_CompressedPayload::decompress(){
			}

			////////////////////////////////////////////////////////////////////////////////////////////////

			inline	_Message::_Message():_senderModuleCID(0),_senderModuleIID(0),_senderCrankCID(0),_senderCrankIID(0){
			}

			inline	_Message::~_Message(){
			}

			inline	_Message::operator	_ControlMessage	*()	const{

				return	(_ControlMessage	*)(((uint8	*)this)-sizeof(_ControlMessage));
			}

			inline	_Message::operator	_Payload	*()	const{

				return	(_Payload	*)(((uint8	*)this)-sizeof(_ControlMessage)-sizeof(_Payload));
			}

			inline	uint16	&_Message::senderModule_cid(){

				return	_senderModuleCID;
			}

			inline	uint16	&_Message::senderModule_iid(){

				return	_senderModuleIID;
			}

			inline	uint16	&_Message::senderCrank_cid(){

				return	_senderCrankCID;
			}

			inline	uint16	&_Message::senderCrank_iid(){

				return	_senderCrankIID;
			}
		}
	}
}