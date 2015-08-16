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

namespace mBrane
{
namespace sdk
{

template<typename T> StaticArray<T>::StaticArray(): _count(0), _array(NULL), _once(false)
{
}

template<typename T> StaticArray<T>::~StaticArray()
{
    if (_array) {
        delete[] _array;
    }
}

template<typename T> inline T *StaticArray<T>::data() const
{
    return _array;
}

template<typename T> inline uint32_t StaticArray<T>::count() const
{
    return _count;
}

template<typename T> void StaticArray<T>::alloc(uint32_t count)
{
    if (_once) {
        return;
    }

    _array = new T[(_count = count) * sizeof(T)];
    _once = true;
}

template<typename T> inline T &StaticArray<T>::operator [](uint32_t i)
{
    return _array[i];
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T, uint16_t Size, class Managed> _Array<T, Size, Managed>::_Array(): _count(0), current(&block), minIndex(0), maxIndex(Size - 1), last(&block)
{
}

template<typename T, uint16_t Size, class Managed> _Array<T, Size, Managed>::~_Array()
{
}

template<typename T, uint16_t Size, class Managed> inline uint32_t _Array<T, Size, Managed>::count() const
{
    return _count;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T, uint16_t Size, class Managed> Array<T, Size, Managed>::Array(): _Array<T, Size, Managed>()
{
}

template<typename T, uint16_t Size, class Managed> Array<T, Size, Managed>::~Array()
{
}

template<typename T, uint16_t Size, class Managed> inline T *Array<T, Size, Managed>::get(uint32_t i)
{
    if (i < this->minIndex) {
        if (this->current->prev) {
            this->minIndex -= Size;
            this->maxIndex -= Size;
            this->current = this->current->prev;
            return get(i);
        }

        return NULL;
    } else if (i > this->maxIndex) {
        if (this->current->next) {
            this->minIndex += Size;
            this->maxIndex += Size;
            this->current = this->current->next;
            return get(i);
        }

        return NULL;
    } else {
        return this->current->data + i - this->minIndex;
    }
}

template<typename T, uint16_t Size, class Managed> inline T &Array<T, Size, Managed>::operator [](uint32_t i)
{
    if (i < this->minIndex) {
        if (this->current->prev) {
            this->minIndex -= Size;
            this->maxIndex -= Size;
            this->current = this->current->prev;
            return (*this)[i];
        }
    } else if (i > this->maxIndex) {
        if (!this->current->next) {
            this->last = this->current->next = new Block<T, Size, Managed>(this->current);
        }

        this->minIndex += Size;
        this->maxIndex += Size;
        this->current = this->current->next;
        return (*this)[i];
    }

    if (i >= this->_count) {
        this->_count = i + 1;
    }

    return this->current->data[i - this->minIndex];
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T, uint16_t Size> Array<T *, Size, ArrayManaged>::Array(): _Array<T *, Size, ArrayManaged>()
{
}

template<typename T, uint16_t Size> Array<T *, Size, ArrayManaged>::~Array()
{
}

template<typename T, uint16_t Size> inline T **Array<T *, Size, ArrayManaged>::get(uint32_t i)
{
    if (i < this->minIndex) {
        if (this->current->prev) {
            this->minIndex -= Size;
            this->maxIndex -= Size;
            this->current = this->current->prev;
            return get(i);
        }

        return NULL;
    } else if (i > this->maxIndex) {
        if (this->current->next) {
            this->minIndex += Size;
            this->maxIndex += Size;
            this->current = this->current->next;
            return get(i);
        }

        return NULL;
    } else {
        return this->current->data + i - this->minIndex;
    }
}

template<typename T, uint16_t Size> inline T *&Array<T *, Size, ArrayManaged>::operator [](uint32_t i)
{
    if (i < this->minIndex) {
        if (this->current->prev) {
            this->minIndex -= Size;
            this->maxIndex -= Size;
            this->current = this->current->prev;
            return (*this)[i];
        }
    } else if (i > this->maxIndex) {
        if (!this->current->next) {
            this->last = this->current->next = new Block<T *, Size, ArrayManaged>(this->current);
        }

        this->minIndex += Size;
        this->maxIndex += Size;
        this->current = this->current->next;
        return (*this)[i];
    }

    if (i >= this->_count) {
        this->_count = i + 1;
    }

    if (!this->current->data[i - this->minIndex]) {
        return this->current->data[i - this->minIndex] = new T();
    } else {
        return this->current->data[i - this->minIndex];
    }
}
}
}
