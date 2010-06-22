//	module_node.h
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

#ifndef	mBrane_sdk_module_node_h
#define	mBrane_sdk_module_node_h

#include	"payload.h"
#include	<iostream>

namespace	mBrane{
	namespace	sdk{

		namespace	module{

			class	_Module;

			//	Node API, as seen from the module
			class	mBrane_dll	Node{
			private:
				static	Node	*Singleton;
			protected:
				static	std::ostream	*Streams[3];
				uint8	_ID;	//	max: 0x7F
				Node(uint8	ID);
				~Node();
			public:
				static	const	uint8	NoID=0xFF;
				typedef	enum{
					PRIMARY=0,
					SECONDARY=1,
					BOTH=2,
					LOCAL=3
				}Network;
				typedef	enum{
					EXECUTION=0,
					NETWORK=1,
					APPLICATION=2
				}TraceLevel;
				static	Node	*Get();
				static	std::ostream&	trace(TraceLevel	l);
				uint8	id()	const;
				virtual	const	char	*name()=0;
				virtual	void	send(const	_Module	*sender,_Payload	*p,Network	network=PRIMARY)=0;
				virtual	void	send(const	_Module	*sender,_Payload	*message, uint8	nodeID,Network	network=PRIMARY)=0;
				virtual	uint64	time()	const=0;	//	in us since 01/01/70
				virtual	void	newSpace(const	_Module	*sender,Network	network=PRIMARY)=0;	//	names are meaningless for dynamic instances
				virtual	void	newModule(const	_Module	*sender,uint16	CID,Network	network=PRIMARY,const	char	*hostName="local")=0;
				virtual	void	deleteSpace(uint16	ID,Network	network=PRIMARY)=0;
				virtual	void	deleteModule(uint16	CID,uint16	ID,Network	network=PRIMARY)=0;
				virtual	void	activateModule(const	_Module	*sender,uint16	module_cid,uint16	module_id,uint16	space_id,float32	activationLevel,Network	network=PRIMARY)=0;
				virtual	void	activateSpace(const	_Module	*sender,uint16	space_id,uint16	target_sid,float32	activationLevel,Network	network=PRIMARY)=0;
				virtual	void	setSpaceThreshold(const	_Module	*sender,uint16	space_id,float32	threshold,Network	network=PRIMARY)=0;
				virtual	void	subscribeMessage(const	_Module	*sender,uint16	module_cid,uint16	module_id,uint16	space_id,uint16	message_cid,Network	network=PRIMARY)=0;
				virtual	void	unsubscribeMessage(const	_Module	*sender,uint16	module_cid,uint16	module_id,uint16	space_id,uint16	message_cid,Network	network=PRIMARY)=0;
				virtual	void	subscribeStream(const	_Module	*sender,uint16	module_cid,uint16	module_id,uint16	space_id,uint16	stream_id,Network	network=PRIMARY)=0;
				virtual	void	unsubscribeStream(const	_Module	*sender,uint16	module_cid,uint16	module_id,uint16	space_id,uint16	stream_id,Network	network=PRIMARY)=0;
				virtual	const	char	*getSpaceName(uint16	hostID,uint16	ID)=0;
				virtual	const	char	*getModuleName(uint16	CID)=0;
				//	Interface for caching.
				virtual	void		markUnused(_Payload	*p)=0;
				virtual	void		addConstantObject(_Payload	*c,const	std::string	&name)=0;
				virtual	_Payload	*getConstantObject(uint32	OID)=0;
				virtual	_Payload	*getConstantObject(const	std::string	&name)=0;
				virtual	void		addLookup(uint8	sourceNID,uint32	OID)=0;
				virtual	bool		hasLookup(uint8	destinationNID,uint32	OID)=0;
				virtual	void		addSharedObject(_Payload	*p)=0;
				virtual	_Payload	*getSharedObject(uint32	OID)=0;
				virtual	void		consolidate(_Payload	*p)=0;
			};
		}
	}
}


#endif
