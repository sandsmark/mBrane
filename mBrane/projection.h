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

#ifndef	mBrane_projection_h
#define	mBrane_projection_h

#include	"../Core/object.h"


using	namespace	mBrane::sdk;

namespace	mBrane
{

//	The following classes are possible values for instatntiating the C parameter of the template classes below.
class	Space;
class	ModuleDescriptor;

template<class	C, class	U>	class	_Projection:
    public	Object<Memory, _Object, U>
{
protected:
    C		*projected;
    Space	*space;
    float	activationLevel;
    _Projection(C	*projected, Space	*space);
    ~_Projection();
};

template<class	C>	class	Projection:
    public	_Projection<C, Projection<C>>
{
public:
    Projection(C	*projected, Space	*space);
    ~Projection();
    void	activate();
    void	deactivate();
    void	setActivationLevel(float	a);
    void	updateActivationCount(float	t);
};

template<class	C>	class	Projectable:
    public	Object<Memory, _Object, C>
{
protected:
    Array<Array<typename	List<P<Projection<C>>, 16>::Iterator, 32>, 8>	projections;	//	indexed by hostID | space ID; to speed up space updating when deleting projections
public:
    uint8_t hostID;	//	dynamically assigned; initially set to NoID, then resolved
    uint16_t ID;
    uint32_t activationCount;
    bool	reactivated;
    Projectable(uint8_t hostID, uint16_t ID);
    ~Projectable();
    void	project(uint8_t hostID, uint16_t spaceID);
    void	unproject(uint8_t hostID, uint16_t spaceID);
    void	activate();
    void	deactivate();
    void	setActivationLevel(uint8_t hostID, uint16_t spaceID, float	a);	//	projects on space if necessary
};
}


//#include	"projection.tpl.cpp"


#endif
