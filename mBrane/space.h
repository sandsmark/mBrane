//	space.h
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

#ifndef	mBrane_space_h
#define	mBrane_space_h

#include	"..\Core\xml_parser.h"

#include	"module_descriptor.h"


using	namespace	mBrane::sdk;

namespace	mBrane{

	class	Space:
	public	Projectable<Space>{
	private:
		class	_Projection{
		public:
			uint16	spaceID;
			float32	activationLevel;
		};
		float32	initialActivationThreshold;
		const	char	*name;
		float32	_activationThreshold;	//	in [0,1]
		Array<_Projection,32>	initialProjections;
		void					applyInitialProjections(uint16	hostID);
	public:
		static	Space						*Get(const	char	*name);	//	in Config.
		static	Array<P<Space>,16>			Config;	//	indexed by space ID; 0 is the root space; see ModuleDescriptor.h
		static	Array<Array<P<Space>,16>,8>	Main;	//	indexed by space ID; 0 is the root space
		static	Space						*New(XMLNode	&n);
		static	void						Init(uint16	hostID);	//	resolves hostID for each space, copies Config in Main, apply initial projections.
		static	void						InitRoot(); // creates root space.
		static	uint16						GetID(uint16	hostID);	//	returns the first available slot in Main[hostID].
		List<P<Projection<ModuleDescriptor> >,16>	moduleDescriptors;
		List<P<Projection<Space> >,16>				spaces;
		Space(uint16	hostID,const	char	*name=NULL);
		~Space();
		const	char	*getName();
		void	setActivationThreshold(float32	thr);
		float32	getActivationThreshold();
		void	_activate();	//	update children activation; called upon changing the space threshold or the space activation level
		void	_deactivate();	//	deactivate children
		List<P<Projection<ModuleDescriptor> >,16>::Iterator	project(Projection<ModuleDescriptor>	*p);
		List<P<Projection<Space> >,16>::Iterator			project(Projection<Space>	*p);

		void	trace();
	};
}


#endif