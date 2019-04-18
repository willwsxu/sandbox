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
#include "platform.h"

namespace tt {

// Conforms to C++11 Lockable and Mutex concepts so it can be used with
// std::lock_guard or std::unique_lock.
class TicketLock
{
public:
    TicketLock() noexcept
        : m_nextTicket(0)
        , m_nowServing(0)
    {}

    ~TicketLock() = default;

    void lock() noexcept
    {
        uint64_t my_ticket = m_nextTicket.fetch_add(1);
        while (my_ticket != m_nowServing.load(std::memory_order_acquire)) {
            asm volatile("pause" ::: "memory");
        }
    }

    void unlock() noexcept
    {
        m_nowServing.fetch_add(1, std::memory_order_release);
    }

private:
    TicketLock(const TicketLock&) = delete;
    TicketLock& operator=(const TicketLock&) = delete;
    TicketLock(TicketLock&&) = delete;
    TicketLock& operator=(TicketLock&&) = delete;

    std::atomic<uint64_t> m_nextTicket __attribute__ ((aligned (TT_CACHE_LINE_SIZE)));
    char pad0[TT_CACHE_LINE_SIZE - sizeof(std::atomic<uint64_t>)];
    std::atomic<uint64_t> m_nowServing;
    char pad1[TT_CACHE_LINE_SIZE - sizeof(std::atomic<uint64_t>)];
};

} // namespace tt
