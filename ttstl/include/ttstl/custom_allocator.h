/***************************************************************************
 *
 *                  Unpublished Work Copyright (c) 2012-2014
 *                  Trading Technologies International, Inc.
 *                       All Rights Reserved Worldwide
 *
 *          * * *   S T R I C T L Y   P R O P R I E T A R Y   * * *
 *
 * WARNING:  This program (or document) is unpublished, proprietary property
 * of Trading Technologies International, Inc. and is to be maintained in
 * strict confidence. Unauthorized reproduction, distribution or disclosure
 * of this program (or document), or any program (or document) derived from
 * it is prohibited by State and Federal law, and by local law outside of
 * the U.S.
 *
 ***************************************************************************/
// Author: Rahul Singh
#ifndef CUSTOM_ALLOCATOR_H_
#define CUSTOM_ALLOCATOR_H_

#include <memory>
#include <type_traits>
#include <iterator>
#include <stdexcept>
#include <ostream>
#include <istream>
#include <cstdint>
#include <cassert>

namespace tt {

/* Custom allocator which aims at re-using a pre-allocated dynamically allocated memory via malloc 
 * and also retains the ownership of the memory.
 * This avoids a second memory allocation in, for example, string c'tor when trying to copy from such pre-allocated malloc memory.
 */
template<typename T>
class InPlaceAllocator : public std::allocator<T>
{
public:
    using size_type = size_t;
    using pointer = T*;
    using const_pointer = const T*;
    
    InPlaceAllocator(){}
    InPlaceAllocator(T* memory):m_allocatedMemory(memory){}
    
    template<typename U>
    struct rebind
    {
        typedef InPlaceAllocator<U> other;
    };

    pointer allocate(size_type n, const void *hint=0)
    {
        if(!m_allocatedMemory)
            return std::allocator<T>::allocate(n, hint);
        else
            return m_allocatedMemory;
    }

    void deallocate(pointer p, size_type n)
    {
        if(!m_allocatedMemory)
            std::allocator<T>::deallocate(p, n);
        else
        {
            assert(p == m_allocatedMemory);
            free(m_allocatedMemory);
            m_allocatedMemory = nullptr;
        }    
    }
    
    bool isMemoryPreAllocated() const {return m_allocatedMemory != nullptr;}
private:
    T* m_allocatedMemory=nullptr;
};

/* Custom allocator which aims at re-using a pre-allocated dynamically allocated memory 
 * and yet does not own the memory.
 * This avoids a second memory allocation in, for example, string c'tor when trying to copy from such pre-allocated malloc memory.
 */
template<typename T>
class InPlaceRefAllocator : public std::allocator<T>
{
public:
    using size_type = size_t;
    using pointer = T*;
    using const_pointer = const T*;
    
    InPlaceRefAllocator(){}
    InPlaceRefAllocator(T* memory):m_allocatedMemory(memory){}
    
    template<typename U>
    struct rebind
    {
        typedef InPlaceRefAllocator<U> other;
    };

    pointer allocate(size_type n, const void *hint=0)
    {
        if(!m_allocatedMemory)
            return std::allocator<T>::allocate(n, hint);
        else
            return m_allocatedMemory;
    }

    void deallocate(pointer p, size_type n)
    {
        if(!m_allocatedMemory)
            std::allocator<T>::deallocate(p, n);
        else
        {
            assert(p == m_allocatedMemory);
            m_allocatedMemory = nullptr;
        }    
    }
    
    bool isMemoryPreAllocated() const {return m_allocatedMemory != nullptr;}
private:
    T* m_allocatedMemory=nullptr;
};

} // namespace tt

#endif // CUSTOM_ALLOCATOR_H_
