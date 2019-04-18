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

#include <atomic>

namespace tt {

// Allow a function to be executed in only one thread. Looping is necessary
// so that the function can respond to new data.
class ExclusiveWhile
{
public:
    ExclusiveWhile()
        : m_counter(0)
    {}

    // Run function exactly as many times as other threads have called Do.
    // The return value indicates if this thread was chosen to run the
    // function.
    template <typename Func>
    bool Do(Func f)
    {
        if (m_counter++ == 0) {
            // Possibly more effecient implementation when there is high
            // contention and m_counter is a high value because it avoids
            // doing atomic ops on each iteration. Instead it keeps the last
            // known count in a register. Commented out for the moment because
            // it hasn't been tested.
            //
            //uint64_t count = 1;
            //do {
            //    for (uint64_t i(0); i<count; ++i) {
            //        f();
            //    }
            //    count = m_counter.fetch_sub(count) - count;
            //} while (count > 0);
            //return true;

            do {
                f();
            } while (--m_counter > 0);
            return true;
        }
        return false;
    }

private:
    std::atomic<uint64_t> m_counter;
};

} // namespace tt
