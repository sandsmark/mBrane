/*
* HUMANOBS - mBrane
*
* Eric Nivel
* Center for Analysis and Design of Intelligent Agents
*   Reykjavik University, Menntavegur 1, 101 Reykjavik, Iceland
*   http://cadia.ru.is
* Copyright(c)2012
*
* This software was developed by the above copyright holder as part of
* the HUMANOBS EU research project, in collaboration with the
* following parties:
*
* Autonomous Systems Laboratory
*   Technical University of Madrid, Spain
*   http://www.aslab.org/
*
* Communicative Machines
*   Edinburgh, United Kingdom
*   http://www.cmlabs.com/
*
* Istituto Dalle Molle di Studi sull'Intelligenza Artificiale
*   University of Lugano and SUPSI, Switzerland
*   http://www.idsia.ch/
*
* Institute of Cognitive Sciences and Technologies
*   Consiglio Nazionale delle Ricerche, Italy
*   http://www.istc.cnr.it/
*
* Dipartimento di Ingegneria Informatica
*   University of Palermo, Italy
*   http://roboticslab.dinfo.unipa.it/index.php/Main/HomePage
*
*
* --- HUMANOBS Open-Source BSD License, with CADIA Clause v 1.0 ---
*
* Redistribution and use in source and binary forms, with or without
* modification, is permitted provided that the following conditions
* are met:
*
* - Redistributions of source code must retain the above copyright
* and collaboration notice, this list of conditions and the
* following disclaimer.
*
* - Redistributions in binary form must reproduce the above copyright
* notice, this list of conditions and the following
* disclaimer in the documentation and/or other materials provided
* with the distribution.
*
* - Neither the name of its copyright holders nor the names of its
* contributors may be used to endorse or promote products
* derived from this software without specific prior written permission.
*
* - CADIA Clause: The license granted in and to the software under this
* agreement is a limited-use license. The software may not be used in
* furtherance of:
* (i) intentionally causing bodily injury or severe emotional distress
* to any person;
* (ii) invading the personal privacy or violating the human rights of
* any person; or
* (iii) committing or preparing for any act of war.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
* "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
* LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
* A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
* OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
* SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
* LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
* DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
* THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
* OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef	mBrane_space_h
#define	mBrane_space_h

#include	"xml_parser.h"

#include	"module_descriptor.h"


using	namespace	mBrane::sdk;

namespace	mBrane
{

class	Space:
    public	Projectable<Space>
{
private:
    class	_Projection
    {
    public:
        uint16_t spaceID;
        float	activationLevel;
    };
    float	initialActivationThreshold;
    const	char	*name;
    float	_activationThreshold;	//	in [0,1]
    Array<_Projection, 32>	initialProjections;
    void					applyInitialProjections(uint8_t hostID);
public:
    static	Space						*Get(const	char	*name);	//	in Config.
    static	Array<P<Space>, 16>			Config;	//	indexed by space ID; 0 is the root space; see ModuleDescriptor.h
    static	Array<Array<P<Space>, 16>, 8>	Main;	//	indexed by space ID; 0 is the root space
    static	Space						*New(XMLNode	&n);
    static	void						Init(uint8_t hostID);	//	resolves hostID for each space, copies Config in Main, apply initial projections.
    static	void						InitRoot(); // creates root space.
    static	uint16_t 					GetID(uint8_t hostID);	//	returns the first available slot in Main[hostID].
    List<P<Projection<ModuleDescriptor>>, 16>	moduleDescriptors;
    List<P<Projection<Space>>, 16>				spaces;
    Space(uint8_t hostID, const	char	*name = NULL);
    ~Space();
    const	char	*getName();
    void	setActivationThreshold(float	thr);
    float	getActivationThreshold();
    void	_activate();	//	update children activation; called upon changing the space threshold or the space activation level
    void	_deactivate();	//	deactivate children
    List<P<Projection<ModuleDescriptor>>, 16>::Iterator	project(Projection<ModuleDescriptor>	*p);
    List<P<Projection<Space>>, 16>::Iterator			project(Projection<Space>	*p);
};
}


#endif
