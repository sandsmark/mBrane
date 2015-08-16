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

#ifndef core_base_h
#define core_base_h

#include	<cstdlib>

#include	"types.h"


namespace	core
{

class	_Object;

//	Smart pointer (ref counting, deallocates when ref count<=0).
//	No circular refs (use std c++ ptrs).
//	No passing in functions (cast P<C> into C*).
//	Cannot be a value returned by a function (return C* instead).
template<class	C>	class	P
{
private:
    _Object	*object;
public:
    P();
    P(C	*o);
    P(const P<C>	&p);
    ~P();
    C	*operator	->()	const;
    template<class	D>	operator	D	*()	const
    {
        return	(D *)object;
    }
    bool	operator	==(C	*c)	const;
    bool	operator	!=(C	*c)	const;
    bool	operator	!()	const;
    template<class	D>	bool	operator	==(P<D>	&p)	const;
    template<class	D>	bool	operator	!=(P<D>	&p)	const;
    P<C>	&operator	=(C	*c);
    P<C>	&operator	=(const  P<C>	&p);
    template<class	D>	P<C>	&operator	=(const P<D>	&p);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//	Root smart-pointable object class.
class	core_dll	_Object
{
    template<class	C>	friend	class	P;
    friend	class	_P;
protected:
#ifdef	ARCH_32
    uint32_t __vfptr_padding_Object;
#endif
    int32_t volatile	refCount;
    _Object();
public:
    virtual	~_Object();
    void	incRef();
    virtual	void	decRef();
};

//	Template version of the well-known DP. Adapts C to _Object.
template<class	C>	class	_ObjectAdapter:
    public	C,
    public	_Object
{
protected:
    _ObjectAdapter();
};
}


#include	"base.tpl.cpp"


#endif
