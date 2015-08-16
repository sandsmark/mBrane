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

#ifndef mBrane_sdk_memory_h
#define mBrane_sdk_memory_h

#include <cstdlib>

#include "../CoreLibrary/utils.h"
#include "config.h"
#include "array.h"


namespace mBrane
{
namespace sdk
{

// Memory allocator.
// One instance of Memory handles the allocation for instances of a given (fixed) size.
// Memory holds statically as many instances of Memory as there are instance sizes (in 64 times a power of 2) to provide allocation for.
// Retrieving the right Memory instance given a size requires a lock: for the sake of efficiency, threads are supposed to
// keep a reference on the Memory instance they use. For example, this is implemented in the Object class as a class
// variable (_Allocator) that is initialized once and kept throughout the process execution.
#ifdef MEMORY_1
// Memory allocation is provided as segments of a larger block. Blocks are allocated to a certain size (objectSize x objectCount), and linked together
// objectSize is the smallest power of two minus one greater or equal than the object's size; objectSize start at 64 bytes.
// Each Memory always holds at least one block, even if entirely free.
class mBrane_dll Memory
{
private:
    static Array<Memory, 16> *Memories;
    static std::mutex s_mutex;
    // Blocks hold contiguous data storage, i.e. an array of segments (byte arrays of size objectSize)
    // When not allocated, a segment contains a pointer to the next free segment: that's its first sizeof(uint8_t *) bytes
    // When deallocated, a segment is not initialized (except the pointer to the next free segment)
    // When allocated a segment contains the address of its block (first sizeof(Block *) bytes) - for speeding up deallocation:
    // this means that the allocated object starts at segment+sizeof(Block *)
    // Blocks are allocated by malloc(sizeof(Block)+totalSize) and deallocated by free()
    // Segments are allocated just after freeObjects, i.e. at ((uint8_t *)this)+sizeof(Block)
    // Synchronization between alloc() and dealloc() uses locks: each call to alloc() or dealloc() enters a critical section, regardless of the contention
    class Block
    {
        friend class Memory;
    private:
        const uint16_t objectCount;
        uint16_t freeObjects;
        const size_t objectSize; // power of 2; includes the pointer to the block
        const size_t totalSize; // objectCount x objectSize
        Block *_next;
        Block *_prev;
        uint8_t *firstFree; // first free cell
        void *operator new(size_t s, size_t objectSize, uint16_t objectCount);
        void operator delete(void *b);
        Block(size_t objectSize, uint16_t objectCount);
        ~Block();
        void *alloc(); // first sizeof(Block *) bytes initialized to this, the rest is initialized to 0
        void dealloc(void *s); // s is a segment, not an object; first sizeof(uint8_t *) bytes initialized to the next free segment, the rest is left untouched
    };
    const size_t objectSize;
    Block *firstBlock;
    Block *lastBlock;
    Block *emptyBlock; // address of the empty block in the block list, if any; NULL otherwise
    uint32_t freeObjects;
    std::mutex m_mutex;
    Memory(size_t objectSize);
    void *operator new(size_t s, uint16_t index);
    void operator delete(void *b);
    static size_t GetNormalizedSize(size_t s, uint8_t &pow2); // converts a size into the next power of 2
public:
    static Memory *GetStatic(size_t s); // called at class loading time; sequential calls
    static Memory *GetDynamic(size_t s); // called dynamically (rwa storage); concurrent calls
    Memory(); // do NOT use
    ~Memory();
    const uint32_t getObjectSize() const;
    void *alloc();
    void *alloc(uint32_t &normalizedSize); // same as alloc; returns the allocated size as a power of 2
    void dealloc(void *o);
};
#elif defined MEMORY_2
// Memory allocation is provided as segments of a larger block. Blocks are allocated to a certain size (segmentSize x segmentCount), and linked together
// segmentSize is the smallest power of two minus one greater or equal than the object's size; segmentSize start at 64 bytes.
// Each Memory always holds at least one block, even if entirely free.
class mBrane_dll Memory
{
private:
    static Array<Memory, 16> *Memories;
    static CriticalSection *CS;
    // Blocks hold contiguous data storage, i.e. an array of segments (byte arrays of size objectSize)
    // A segment is a pointer to the block (first sizeof(Block *) bytes) followed by allocation space (objectSize-sizeof(Block*)
    // This means that the allocated object starts at segment+sizeof(Block *)
    // Blocks are allocated by malloc(sizeof(Block)+totalSize) and deallocated by free(): this can be improved by allocating from aprivate heap instead
    // Segments are allocated just after freeObjects, i.e. at ((uint8_t *)this)+sizeof(Block)
    class Block
    {
        friend class Memory;
    private:
    private:
        const uint16_t segmentCount;
        const size_t segmentSize; // power of 2; includes the pointer to the block
        const size_t totalSize; // segmentCount x segmentSize
        Block *_next;
        Block *_prev;
        volatile int32_t freeSegments[2]; // stores the free segments per stack
        void *operator new(size_t s, size_t objectSize, uint16_t objectCount);
        void operator delete(void *b);
        Block(size_t segmentSize, uint16_t segmentCount);
        ~Block();
        uint8_t *segments;
    };
    Block *firstBlock;
    Block *lastBlock;

    // A Memory instance maintains two stacks, the alloc stack and the dealloc stack, both holding free segments
    // alloc() pops a segment out of the alloc stack, while dealloc() pushes a segment on top of the dealloc stack
    // When the alloc stack is empty, alloc() tries to dealocate an empty block if any, and decreases the dealloc stack;
    // then the two stacks are swapped: the dealloc stack becomes the alloc stack and vice-versa
    // If the dealloc stack is not full enough, no swapping is performed, but a new block is allocated instead and the alloc stack refilled
    // When the dealloc stack is full, dealloc() swaps the stack pointers if the alloc stack is empty enough
    // If that's not the case, it tries to deallocate an empty block, if any, and if unsuccessful, the dealloc stack is increased (realloc)
    // Synchronization between multiple alloc() is lock-free under no contention (alloc_top) and uses a lock under contention (a_sem); idem for dealloc() (resp. dealloc_top and d_sem)
    // Synchronization between one alloc() swapping stacks and multiple dealloc() is lock-free under no contention (a_d_guard), and uses a lock (a_d_sem) under contention
    // Synchronization between one dealloc() swapping stacks and multiple alloc() is lock-free under no contention (d_a_guard), and uses a lock (d_a_sem) under contention
    class Stack
    {
    public:
        uint8_t **stack; // points to segments
        int32_t top;
        int32_t size; // in ints
        uint32_t id; // used to retrieve a block's free segments per stack
    };
    Stack stacks[2];
    Stack *volatile alloc_stack;
    Stack *volatile dealloc_stack;
    Semaphore *a_sem; // synchronizes alloc threads
    Semaphore *d_sem; // synchronizes dealloc threads
    Semaphore *a_d_sem; // synchronizes one alloc thread and multiple dealloc threads using the dealloc stack
    Semaphore *d_a_sem; // synchronizes one dealloc thread and multiple alloc threads using the alloc stack
    int32_t volatile a_d_guard;
    int32_t volatile d_a_guard;
    int32_t volatile alloc_top; // avoids contention on alloc_stack->top
    int32_t volatile dealloc_top; // avoids contention on dealloc_stack->top

    void *allocSegment(int32_t location) const;
    void deallocSegment(void *o, int32_t location) const;
    bool deallocBlock(); // attempt to find an empty bloc and deallocate it and shrink the dealloc stack; return true is succeeded
    void swapStackPointers();

    const size_t objectSize;
    Memory(size_t objectSize);
    void *operator new(size_t s, uint16_t index);
    void operator delete(void *b);
    static size_t GetNormalizedSize(size_t s, uint8_t &pow2); // converts a size into the next power of 2
public:
    static Memory *GetStatic(size_t s); // called at class loading time; sequential calls
    static Memory *GetDynamic(size_t s); // called dynamically (rwa storage); concurrent calls
    Memory(); // do NOT use
    ~Memory();
    const uint32_t getObjectSize() const;
    void *alloc();
    void *alloc(uint32_t &normalizedSize); // same as alloc; returns the allocated size as a power of 2
    void dealloc(void *o);
};
#elif defined MEMORY_MALLOC
// for testing only!
class mBrane_dll Memory
{
private:
    const size_t objectSize;
    Memory(size_t objectSize): objectSize(objectSize) {}
public:
    static Memory *GetStatic(size_t s)
    {
        return new Memory(s);    // memory leak: Memory(s) never deallocated
    }
    static Memory *GetDynamic(size_t s)
    {
        return new Memory(s);    // idem
    }
    ~Memory() {}
    const uint32_t getObjectSize() const
    {
        return objectSize;
    }
    void *alloc()
    {
        return malloc(objectSize);
    }
    void *alloc(uint32_t &normalizedSize)
    {
        normalizedSize = objectSize;
        return malloc(objectSize);
    }
    void dealloc(void *o)
    {
        free(o);
    }
};
#endif
}
}


#endif
