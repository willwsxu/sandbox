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

#include <functional>

#include <ttstl/waitable_queue.h>

namespace tt {

using Task = std::function<void ()>;

// A specialized queue which holds callable objects.
template <typename Queue = WaitableQueue<Task>>
class TaskQueue
{
public:
    TaskQueue() = default;
    virtual ~TaskQueue() = default;

    // The Run method waits on the queue and dispatches tasks as they arrive.
    // This call blocks until Unblock is called.
    //
    // Returns number of tasks dispatched.
    size_t Run()
    {
        size_t count = 0;
        try {
            for (;;) {
                auto task = m_queue.Pop();
                task();
                ++count;
            }
        } catch (const ExitLoop&) {
            // This block intentionally left blank.
        }
        return count;
    }

    // Dispatch at most one task.
    //
    // Returns number of tasks dispatched.
    size_t Poll()
    {
        try {
            auto task = m_queue.Pop();
            task();
        } catch (const ExitLoop&) {
            return 0;
        }
        return 1;
    }

    // Unblock a thread blocked on the Run method. This is a non-blocking call
    // which means it will *not* wait for the Run method to exit before
    // returning. All currently enqueued tasks will be run before unblocking.
    void Unblock()
    {
        m_queue.Push([]() { throw ExitLoop(); });
    }

    // Post a task to the queue.
    template <typename T>
    void Post(T&& task)
    {
        m_queue.Push(std::forward<T>(task));
    }

private:
    TaskQueue(const TaskQueue&) = delete;
    void operator=(const TaskQueue&) = delete;
    TaskQueue(TaskQueue&&) = delete;
    void operator=(TaskQueue&&) = delete;

    struct ExitLoop {};

    Queue m_queue;
};

} // namespace tt
