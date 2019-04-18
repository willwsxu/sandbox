/***************************************************************************
*
*                  Unpublished Work Copyright (c) 2012, 2014
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

#include <utility>

namespace tt {

// Defer a function call until this object is destroyed. This can be used to
// register cleanup routines that need to be called if an exception is thrown
// or a function returns early. Call the Disarm() method to stop the function
// call from taking place when the object goes out of scope.
template <typename F>
class Deferred
{
public:
    Deferred(F&& func)
        : m_f(std::move(func))
        , m_armed(true)
    {}

    ~Deferred()
    {
        if (m_armed) {
            m_f();
        }
    }

    void Disarm() noexcept
    {
        m_armed = false;
    }

private:
    F m_f;
    bool m_armed;
};

template <typename F>
Deferred<F> MakeDeferred(F&& func)
{
    return Deferred<F>(std::move(func));
}

} // namespace tt
