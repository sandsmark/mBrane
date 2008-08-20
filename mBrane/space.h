//	space.h
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

#ifndef	mBrane_space_h
#define	mBrane_space_h

#include	"module_descriptor.h"


using	namespace	mBrane::sdk;

namespace	mBrane{

	class	Space:
	public	Projectable<Space>{
	private:
		uint16	ID;
		float32	_activationThreshold;	//	in [0,1]
	public:
		static	Array<Space>	Spaces;	//	0 is the root space
		uint32	activationCount;
		List<P<Projection<ModuleDescriptor> > >	moduleDescriptors;
		List<P<Projection<Space> > >			spaces;
		Space(uint16	ID);
		~Space();
		uint16	id();
		void	setActivationThreshold(float32	thr);
		float32	getActivationThreshold();
		uint32	project(ModuleDescriptor	*m,float32	activationLevel);
		uint32	project(Space	*s,float32	activationLevel);
		void	removeProjection(ModuleDescriptor	*dummy,uint32	p);
		void	removeProjection(Space	*dummy,uint32	p);
	};
}


#endif