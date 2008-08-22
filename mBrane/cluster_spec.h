//	cluster_spec.h
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

#ifndef	mBrane_cluster_spec_h
#define	mBrane_cluster_spec_h

#include	"module_spec.h"
#include	"space_spec.h"


using	namespace	mBrane::sdk;

namespace	mBrane{

	class	Cluster;
	class	ClusterSpec{	//	aka Entity, i.e. blueprint to instantiate spaces and modules
	protected:
		static	uint16	LastCID;
		uint16	CID;
	public:
		static	Array<ClusterSpec>	Main;
		static	ClusterSpec	*New(XMLNode	&n);
		class	ClusterRef{
		public:
			static	ClusterRef	*New(XMLNode	&n);
			uint16	clusterClass;
			uint16	clusterCount;
		};
		Array<ModuleSpec	*>	modules;
		Array<SpaceSpec		*>	spaces;
		Array<ClusterRef	*>	clusters;
		ClusterSpec();
		~ClusterSpec();
		void	instanciate();
	};
}


#endif