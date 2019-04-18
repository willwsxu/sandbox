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

#include <memory>

namespace tt {

// Simple observer pattern implementation helper
template <typename Observer>
class Observable
{
public:
    virtual ~Observable() {}

    // Register an observer
    //
    // Thread-safe
    // Does not throw
    virtual void RegisterShared(std::shared_ptr<Observer> observer) = 0;
    virtual void RegisterWeak(std::weak_ptr<Observer> observer) = 0;

    // Unregister an observer
    //
    // Thread-safe
    // Does not throw
    virtual void UnregisterShared(const std::shared_ptr<Observer>& observer) = 0;
    virtual void UnregisterWeak(const std::weak_ptr<Observer>& observer) = 0;
    virtual void UnregisterRaw(const Observer* observer) = 0;
};

} // namespace tt
