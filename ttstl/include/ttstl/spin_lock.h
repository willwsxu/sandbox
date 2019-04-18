/***************************************************************************
*
*                    Unpublished Work Copyright (c) 2013, 2015
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
#pragma once

#include <atomic>

namespace tt {

// Conforms to C++11 Lockable and Mutex concepts so it can be used with
// std::lock_guard or std::unique_lock.
class SpinLock
{
public:
    SpinLock() noexcept
        : m_locked(false)
    {}

    ~SpinLock() = default;

    void lock() noexcept
    {
        for (;;) {
            while (m_locked.load(std::memory_order_acquire)) {
                asm volatile("pause" ::: "memory");
            }
            bool expected = false;
            if (m_locked.compare_exchange_weak(
                    expected, true, std::memory_order_acq_rel))
            {
                break;
            }
            asm volatile("pause" ::: "memory");
        }
    }

    bool try_lock() noexcept
    {
        if (m_locked.load(std::memory_order_acquire)) {
            return false;
        }
        bool expected = false;
        return m_locked.compare_exchange_weak(
            expected, true, std::memory_order_acq_rel);
    }

    void unlock() noexcept
    {
        m_locked.store(false, std::memory_order_release);
    }

private:
    SpinLock(const SpinLock&) = delete;
    SpinLock& operator=(const SpinLock&) = delete;
    SpinLock(SpinLock&&) = delete;
    SpinLock& operator=(SpinLock&&) = delete;

    std::atomic<bool> m_locked;
};

} // namespace tt
