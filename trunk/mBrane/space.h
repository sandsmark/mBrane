//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/ //_/_/ //_/_/  space.h //_/_/ //_/_/  Eric Nivel //_/_/  Center for Analysis and Design of Intelligent Agents //_/_/    Reykjavik University, Menntavegur 1, 101 Reykjavik, Iceland //_/_/    http://cadia.ru.is
//_/_/  Copyright©2012 //_/_/ //_/_/  This software was developed by the above copyright holder as part of  //_/_/  the HUMANOBS EU research project, in collaboration with the  //_/_/  following parties:
//_/_/   //_/_/  Autonomous Systems Laboratory //_/_/    Technical University of Madrid, Spain //_/_/    http://www.aslab.org/ //_/_/ //_/_/  Communicative Machines //_/_/    Edinburgh, United Kingdom //_/_/    http://www.cmlabs.com/ //_/_/ //_/_/  Istituto Dalle Molle di Studi sull'Intelligenza Artificiale //_/_/    University of Lugano and SUPSI, Switzerland //_/_/    http://www.idsia.ch/ //_/_/ //_/_/  Institute of Cognitive Sciences and Technologies //_/_/    Consiglio Nazionale delle Ricerche, Italy //_/_/    http://www.istc.cnr.it/
//_/_/
//_/_/  Dipartimento di Ingegneria Informatica
//_/_/    University of Palermo, Italy
//_/_/    http://roboticslab.dinfo.unipa.it/index.php/Main/HomePage
//_/_/ //_/_/
//_/_/  --- HUMANOBS Open-Source BSD License, with CADIA Clause v 1.0 ---
//_/_/
//_/_/  Redistribution and use in source and binary forms, with or without 
//_/_/  modification, is permitted provided that the following conditions 
//_/_/  are met:
//_/_/
//_/_/  - Redistributions of source code must retain the above copyright 
//_/_/    and collaboration notice, this list of conditions and the 
//_/_/    following disclaimer.
//_/_/
//_/_/  - Redistributions in binary form must reproduce the above copyright 
//_/_/    notice, this list of conditions and the following
//_/_/    disclaimer in the documentation and/or other materials provided 
//_/_/    with the distribution.
//_/_/
//_/_/  - Neither the name of its copyright holders nor the names of its 
//_/_/    contributors may be used to endorse or promote products 
//_/_/    derived from this software without specific prior written permission.
//_/_/
//_/_/  - CADIA Clause: The license granted in and to the software under this 
//_/_/     agreement is a limited-use license. The software may not be used in 
//_/_/     furtherance of: 
//_/_/     (i) intentionally causing bodily injury or severe emotional distress 
//_/_/         to any person; 
//_/_/     (ii) invading the personal privacy or violating the human rights of 
//_/_/         any person; or 
//_/_/     (iii) committing  or preparing for any act of war.
//_/_/
//_/_/  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
//_/_/  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
//_/_/  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR 
//_/_/  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
//_/_/  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
//_/_/  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
//_/_/  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, 
//_/_/  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY 
//_/_/  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
//_/_/  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
//_/_/  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//_/_/
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

#ifndef	mBrane_space_h
#define	mBrane_space_h

#include	"xml_parser.h"

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
		void					applyInitialProjections(uint8	hostID);
	public:
		static	Space						*Get(const	char	*name);	//	in Config.
		static	Array<P<Space>,16>			Config;	//	indexed by space ID; 0 is the root space; see ModuleDescriptor.h
		static	Array<Array<P<Space>,16>,8>	Main;	//	indexed by space ID; 0 is the root space
		static	Space						*New(XMLNode	&n);
		static	void						Init(uint8	hostID);	//	resolves hostID for each space, copies Config in Main, apply initial projections.
		static	void						InitRoot(); // creates root space.
		static	uint16						GetID(uint8	hostID);	//	returns the first available slot in Main[hostID].
		List<P<Projection<ModuleDescriptor> >,16>	moduleDescriptors;
		List<P<Projection<Space> >,16>				spaces;
		Space(uint8	hostID,const	char	*name=NULL);
		~Space();
		const	char	*getName();
		void	setActivationThreshold(float32	thr);
		float32	getActivationThreshold();
		void	_activate();	//	update children activation; called upon changing the space threshold or the space activation level
		void	_deactivate();	//	deactivate children
		List<P<Projection<ModuleDescriptor> >,16>::Iterator	project(Projection<ModuleDescriptor>	*p);
		List<P<Projection<Space> >,16>::Iterator			project(Projection<Space>	*p);
	};
}


#endif
