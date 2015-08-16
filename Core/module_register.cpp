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

#include	<memory>
#include	<cstring>
#include	"module_register.h"


namespace	mBrane{
	namespace	sdk{

		Array<ModuleRegister,1024>	*ModuleRegister::Modules=NULL;

		Array<ModuleRegister,1024>	*ModuleRegister::Get(){

			if(!Modules)
				Modules=new	Array<ModuleRegister,1024>();
			return	Modules;
		}

		ModuleRegister	*ModuleRegister::Get(uint16	CID){

			return	Get()->get(CID);
		}

		uint16	ModuleRegister::GetCID(const	char	*className){

			for(uint16	i=0;i<Modules->count();i++)
				if(strcmp(Modules->get(i)->class_name,className)==0)
					return	i;
			return	ClassRegister::NoClass;
		}

		inline	uint16	ModuleRegister::Count(){

			return	(uint16)Modules->count();
		}

		uint16	ModuleRegister::Load(ModuleBuilder	b,const	char	*className){

			ModuleRegister	*r=&Get()->operator	[](Get()->count());
			r->_builder=b;
			strcpy(r->class_name,className);
			return	(uint16)(Modules->count()-1);
		}

		ModuleRegister::ModuleRegister():_builder(NULL){
		}

		ModuleRegister::~ModuleRegister(){
		}

		module::_Module	*ModuleRegister::buildModule()	const{

			return	_builder();
		}

		const	char	*ModuleRegister::name()	const{

			return	class_name;
		}

		void	ModuleRegister::Cleanup(){

			delete	Modules;
		}
	}
}
