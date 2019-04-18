/***************************************************************************
*
*                Unpublished Work Copyright (c) 2012-2013, 2015
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

#include <chrono>
#include <condition_variable>
#include <mutex>

namespace tt {

// Just a simple event implemented using a condition variable.
class Event
{
public:
    Event() : m_set(false) {}

    void Set()
    {
        {
            std::lock_guard<std::mutex> lock(m_mtx);
            m_set = true;
        }
        m_cv.notify_all();
    }

    void Wait()
    {
        std::unique_lock<std::mutex> lock(m_mtx);
        while (!m_set) {
            m_cv.wait(lock);
        }
    }

    // Returns true if event was set, false if timeout
    template <typename Clock, typename Duration>
    bool WaitUntil(const std::chrono::time_point<Clock, Duration>& abs_time)
    {
        std::unique_lock<std::mutex> lock(m_mtx);
        while (!m_set) {
            if (m_cv.wait_until(lock, abs_time) == std::cv_status::timeout) {
                return false;
            }
        }
        return true;
    }

    // Returns true if event was set, false if timeout
    template <typename Rep, typename Period>
    bool WaitFor(const std::chrono::duration<Rep, Period>& rel_time)
    {
        return WaitUntil(std::chrono::system_clock::now() + rel_time);
    }

    void Reset()
    {
        std::lock_guard<std::mutex> lock(m_mtx);
        m_set = false;
    }

    bool IsSet()
    {
        std::lock_guard<std::mutex> lock(m_mtx);
        return m_set;
    }

private:
    bool m_set;
    std::mutex m_mtx;
    std::condition_variable m_cv;
};

} // namespace tt
