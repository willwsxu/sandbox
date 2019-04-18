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
***************************************************************************/
#pragma once

#include <chrono>
#include <random>

namespace tt {

// This class implements an exponential backoff algorithm. Every call to
// Next() will return a new duration. The precision of the returned values is
// equal to the precision of the duration type specified. The random offset
// is computed in ticks.
template <
    typename Duration = std::chrono::milliseconds,
    typename RandomEngine = std::minstd_rand
>
class ExponentialBackoff
{
public:
    using duration = Duration;
    using random_engine = RandomEngine;

    ExponentialBackoff(
        Duration interval,
        Duration maxDelay = Duration::max(),
        double factor = 0.5,
        typename RandomEngine::result_type seed = std::random_device()())
        : m_randEngine(seed)
        , m_interval(interval)
        , m_maxDelay(maxDelay)
        , m_factor(factor)
        , m_count(0)
        , m_maxed(false)
    {}

    Duration Next()
    {
        Duration delay;

        // Avoid computing a new delay if we have already reached the limit. If
        // m_count becomes large, computing 2^n may overflow.
        if (m_maxed) {
            delay = m_maxDelay;
        } else {
            // Compute a new delay using exponential backoff algorithm.
            delay = m_interval * (1ULL << m_count);
            if (m_count > 0) {
                // Check for overflow
                Duration prev = m_interval * (1ULL << (m_count-1));
                if (delay < prev) {
                    delay = m_maxDelay;
                }
            }

            // Set m_maxed and clamp delay to maxDelay if maxDelay is met or
            // exceeded.
            if (delay >= m_maxDelay) {
                delay = m_maxDelay;
                m_maxed = true;
            }
        }

        // Randomize the delay to avoid the thundering herd effect.
        if (m_factor != 0) {
            auto r = delay.count() * m_factor;
            std::uniform_int_distribution<typename Duration::rep> dist(-r, r);
            auto ticks = dist(m_randEngine);
            // Check for overflow
            if (Duration::max().count() - delay.count() < ticks) {
                delay = Duration::max();
            } else {
                delay += Duration(ticks);
            }
        }

        ++m_count;

        return delay;
    }

    Duration Interval() const
    {
        return m_interval;
    }

    unsigned long Count() const
    {
        return m_count;
    }

    double Factor() const
    {
        return m_factor;
    }

    void Reset()
    {
        m_count = 0;
        m_maxed = false;
    }

private:
    RandomEngine m_randEngine;
    Duration m_interval;
    Duration m_maxDelay;
    double m_factor;
    unsigned long m_count;
    bool m_maxed;
};

} // namespace tt
