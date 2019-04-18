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

#include <chrono>
#include <condition_variable>
#include <exception>
#include <mutex>
#include <queue>

namespace tt {

class QueueTimeout : public std::exception {};

template <typename T, typename Queue = std::queue<T>>
class WaitableQueue
{
public:
    WaitableQueue()
        : m_waiters(0)
    {}

    WaitableQueue(WaitableQueue&& other)
        : m_waiters(0)
    {
        std::lock_guard<std::mutex> lock(other.m_mtx);
        m_queue.swap(other.m_queue);
    }

    WaitableQueue& operator=(WaitableQueue&& other)
    {
        if (this != &other) {
            std::lock_guard<std::mutex> lock1(m_mtx);
            {
                std::lock_guard<std::mutex> lock2(other.m_mtx);
                m_queue.swap(other.m_queue);
            }
            if (m_waiters > 0) {
                m_cv.notify_all();
            }
        }
        return *this;
    }

    void Push(const T& v)
    {
        std::unique_lock<std::mutex> lock(m_mtx);
        m_queue.push(v);
        if (m_waiters > 0) {
            lock.unlock();
            m_cv.notify_one();
        }
    }

    void Push(T&& v)
    {
        std::unique_lock<std::mutex> lock(m_mtx);
        m_queue.push(std::move(v));
        if (m_waiters > 0) {
            lock.unlock();
            m_cv.notify_one();
        }
    }

    template <typename... Args>
    void Emplace(Args&&... args)
    {
        std::unique_lock<std::mutex> lock(m_mtx);
        m_queue.emplace(std::forward<Args>(args)...);
        if (m_waiters > 0) {
            lock.unlock();
            m_cv.notify_one();
        }
    }

    // Like Pop() but accepts a timeout. Throws QueueTimeout if timeout is
    // exceeded.
    template <class Rep, class Period>
    T Pop(const std::chrono::duration<Rep, Period>& timeout)
    {
        std::unique_lock<std::mutex> lock(m_mtx);

        ScopedIncrement<size_t> inc(m_waiters);

        while (m_queue.empty()) {
            if (m_cv.wait_for(lock, timeout) == std::cv_status::timeout) {
                throw QueueTimeout();
            }
        }

        T value(std::move(m_queue.front()));
        m_queue.pop();

        return value;
    }

    T Pop()
    {
        std::unique_lock<std::mutex> lock(m_mtx);

        ScopedIncrement<size_t> inc(m_waiters);

        while (m_queue.empty()) {
            m_cv.wait(lock);
        }

        T value(std::move(m_queue.front()));
        m_queue.pop();

        return value;
    }

private:
    WaitableQueue(const WaitableQueue&) = delete;
    void operator=(const WaitableQueue&) = delete;

    template <typename U>
    class ScopedIncrement
    {
    public:
        ScopedIncrement(U& v) : m_v(v) { ++m_v; }
        ~ScopedIncrement() { --m_v; }
    private:
        U& m_v;
    };

    Queue m_queue;
    std::mutex m_mtx;
    std::condition_variable m_cv;
    size_t m_waiters;
};

} // namespace tt
