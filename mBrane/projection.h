//	projection.h
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

#ifndef	mBrane_projection_h
#define	mBrane_projection_h

#include	"..\Core\object.h"


using	namespace	mBrane::sdk;

namespace	mBrane{

	class	Space;
	class	ModuleDescriptor;

	template<class	C,class	U>	class	_Projection:
	public	Object<Memory,_Object,U>{
	protected:
		C		*projected;
		Space	*space;
		float32	activationLevel;
		_Projection(C	*projected,Space	*space);
		~_Projection();
	};

	template<class	C>	class	Projection:
	public	_Projection<C,Projection<C> >{
	public:
		Projection(C	*projected,Space	*space);
		~Projection();
		void	activate();
		void	deactivate();
		void	setActivationLevel(float32	a);
		void	updateActivationCount(float32	t);
	};

	template<class	C>	class	Projectable:
	public	Object<Memory,_Object,C>{
	protected:
		Array<typename	List<P<Projection<C> > >::Iterator>	projections;	//	indexed by space ID; to speed up space updating when deleting projections
		void	activate();
		void	deactivate();
	public:
		uint16	ID;
		uint32	activationCount;
		Projectable(uint16	ID);
		~Projectable();
		void	project(uint16	spaceID);
		void	unproject(uint16	spaceID);
		void	setActivationLevel(uint16	spaceID,float32	a);	//	projects on space if necessary
	};
}


#include	"projection.tpl.cpp"


#endif