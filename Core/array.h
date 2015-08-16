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

#ifndef mBrane_sdk_array_h
#define mBrane_sdk_array_h

#include "../CoreLibrary/types.h"
#include <string.h>

using namespace core;

namespace mBrane
{
namespace sdk
{

template<typename T> class StaticArray
{
private:
    uint32_t _count;
    T *_array;
    bool _once;
public:
    StaticArray();
    ~StaticArray();
    void alloc(uint32_t count); // to be called only once
    uint32_t count() const;
    T &operator [](uint32_t i); // unprotected, i.e. do not let i>=count()
    T *data() const;
};

class ArrayManaged {};
class ArrayUnmanaged {};

template<typename T, uint16_t Size, class Managed> class Block
{
public:
    Block(Block *prev = NULL): next(NULL), prev(prev) {}
    ~Block()
    {
        if (next) {
            delete next;
        }
    }
    T data[Size];
    Block *next;
    Block *prev;
};

// Specialization of Block<T,Size> holding pointers and managing objects.
template<typename T, uint16_t Size> class Block<T *, Size, ArrayManaged>
{
public:
    Block(Block *prev = NULL): next(NULL), prev(prev)
    {
        memset(data, 0, Size * sizeof(T));
    }
    ~Block()
    {
        for (uint32_t i = 0; i < Size; i++)if (data[i]) {
                delete data[i];
            }

        if (next) {
            delete next;
        }
    }
    T *data[Size];
    Block *next;
    Block *prev;
};

template<typename T, uint16_t Size, class Managed> class _Array
{
protected:
    uint32_t _count;
    Block<T, Size, Managed> block;
    Block<T, Size, Managed> *last;
    // navigation optimization
    Block<T, Size, Managed> *current;
    uint32_t minIndex;
    uint32_t maxIndex;
    _Array();
public:
    virtual ~_Array();
    uint32_t count() const;
};

// Dynamic array, i.e. a linked list of Block<T,Size>.
template<typename T, uint16_t Size, class Managed = ArrayUnmanaged> class Array:
    public _Array<T, Size, Managed>
{
public:
    Array();
    ~Array();
    T *get(uint32_t i); // no allocation performed
    T &operator [](uint32_t i); // changes counts, i.e. makes sure that there's room. Doesn't mean any object has been assigned to the ith position
};

// Specialization of _Array<T,Size> holding pointers and managing objects.
// Array<T*,Size> instantiates pointed objects (calling T()) whenever needed upon calls to operator [].
// Array<T*,Size> deletes pointed objects.
template<typename T, uint16_t Size> class Array<T *, Size, ArrayManaged>:
    public _Array<T *, Size, ArrayManaged>
{
public:
    Array();
    ~Array();
    T **get(uint32_t i); // no allocation performed
    T *&operator [](uint32_t i); // changes counts, i.e. makes sure that there's room. Doesn't mean any object has been assigned to the ith position
};
}
}


#include "array.tpl.cpp"


#endif
