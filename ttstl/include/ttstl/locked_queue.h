/***************************************************************************
*
*                    Unpublished Work Copyright (c) 2014
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
***************************************************************************
* Author: John Shaw (jshaw)
***************************************************************************/
#pragma once

#include <deque>
#include <mutex>

namespace tt {

// Simple thread-safe queue using a mutex. Both the container type and the
// mutex type are configurable.
//
// Mutex must conform to the BasicLockable concept which requires that it
// support the methods lock() and unlock().
//
// Container requires the methods push_back(), emplace_back(), pop_front(),
// front(), size(), and empty().
template <
    typename T,
    typename Container = std::deque<T>,
    typename Mutex = std::mutex
>
class LockedQueue
{
public:
    using mutex_type = Mutex;
    using container_type = Container;
    using value_type = typename Container::value_type;
    using size_type = typename Container::size_type;
    using reference = typename Container::reference;
    using const_reference = typename Container::const_reference;

    void Push(const T& v)
    {
        std::lock_guard<Mutex> lock(m_mtx);
        m_queue.push_back(v);
    }

    void Push(T&& v)
    {
        std::lock_guard<Mutex> lock(m_mtx);
        m_queue.push_back(std::move(v));
    }

    template <typename... Args>
    void Emplace(Args&&... args)
    {
        std::lock_guard<Mutex> lock(m_mtx);
        m_queue.emplace_back(std::forward<Args>(args)...);
    }

    bool Pop(T& v)
    {
        std::lock_guard<Mutex> lock(m_mtx);
        if (m_queue.empty()) {
            return false;
        }
        v = std::move(m_queue.front());
        m_queue.pop_front();
        return true;
    }

    // value_storage is a type exactly large enough to hold one instance of T.
    using value_storage = std::aligned_storage<sizeof(T), alignof(T)>;

    // Optimize the Pop if type T has expensive or no default constructor.
    // This version of Pop will use placement new to move-construct v.
    bool Pop(value_storage& v)
    {
        std::lock_guard<Mutex> lock(m_mtx);
        if (m_queue.empty()) {
            return false;
        }
        new (&v) T(std::move(m_queue.front()));
        m_queue.pop_front();
        return true;
    }

    // Thrown by the Pop() function below.
    class QueueEmpty : public std::exception {};

    // This version of Pop returns a move-constructed T object or throws if
    // the container is empty.
    T Pop()
    {
        std::lock_guard<Mutex> lock(m_mtx);
        if (m_queue.empty()) {
            throw QueueEmpty();
        }
        T v(std::move(m_queue.front()));
        m_queue.pop_front();
        return v;
    }

    size_type Size() const
    {
        std::lock_guard<Mutex> lock(m_mtx);
        return m_queue.size();
    }

    bool Empty() const
    {
        std::lock_guard<Mutex> lock(m_mtx);
        return m_queue.empty();
    }

private:
    mutable Mutex m_mtx;
    Container m_queue;
};

} // namespace tt
