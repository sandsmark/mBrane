//	module_descriptor.cpp
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

#include	"module_descriptor.h"


namespace	mBrane{

	ModuleEntry::ModuleEntry(NodeEntry	*n,ModuleDescriptor	*m):Object<Memory,_Object,ModuleEntry>(),node(n),module(m){
	}

	ModuleEntry::~ModuleEntry(){

		if(module->activationCount)
			node->activationCount--;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////

	Array<Array<P<ModuleDescriptor> > >	ModuleDescriptor::Main;

	ModuleDescriptor::ModuleDescriptor(_Module	*m):module(m),activationCount(0){
	}

	ModuleDescriptor::~ModuleDescriptor(){
	}

	////////////////////////////////////////////////////////////////////////////////////////////////

	ModuleDescriptorProjection::ModuleDescriptorProjection(ModuleDescriptor	*projected,Space	*space,float32	activationLevel):Projection<ModuleDescriptor>(projected,space,activationLevel){
	}
	
	ModuleDescriptorProjection::~ModuleDescriptorProjection(){
	
		List<typename	List<P<ModuleEntry> >::Iterator>::Iterator	i;
		for(i=subscriptions.begin();i!=subscriptions.end();i++){

			((P<ModuleEntry>)((List<P<ModuleEntry> >::Iterator)i))=NULL;
			((List<P<ModuleEntry> >::Iterator)i).remove();
		}
	}
}