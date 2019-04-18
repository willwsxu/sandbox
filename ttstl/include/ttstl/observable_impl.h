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

#include <mutex>
#include <vector>

#include <ttstl/observable.h>

namespace tt {

#define CORRUPTED_OBSERVER (void*)0xffffffffffffffff

template <typename Observer>
class ObservableImpl : virtual public Observable<Observer>
{
public:
    // Register an observer. Observers will be notified in the order in which
    // they were inserted.
    //
    // Thread-safe
    // Does not throw
    void RegisterShared(std::shared_ptr<Observer> observer) override
    {
        if (observer && observer.get() != CORRUPTED_OBSERVER) {
            std::lock_guard<std::mutex> lock(m_mtx);
            m_weakObservers.emplace_back(observer);
            m_strongObservers.emplace_back(std::move(observer));
        } else {
            char cmd[1024];
            snprintf(cmd, 1024, "logger ObservableImpl::RegiserShared observer corrupted. observer=%p", (void*)observer.get());
            system(cmd);
        }
    }

    void RegisterWeak(std::weak_ptr<Observer> observer) override
    {
        auto o = observer.lock();
        if (o && o.get() != CORRUPTED_OBSERVER) {
            std::lock_guard<std::mutex> lock(m_mtx);
            m_weakObservers.emplace_back(std::move(observer));
            m_strongObservers.emplace_back(nullptr);
        } else {
            char cmd[1024];
            snprintf(cmd, 1024, "logger ObservableImpl::RegisterWeak observer corrupted. observer=%p", (void*)o.get());
            system(cmd);
        }
    }

    // Unregister an observer.
    //
    // Thread-safe
    // Does not throw
    void UnregisterShared(const std::shared_ptr<Observer>& observer) override
    {
        std::lock_guard<std::mutex> lock(m_mtx);
        // O(N), but that's ok because I expect N to be small.
        for (size_t i(0); i < m_weakObservers.size(); ++i) {
            if (GetObserver(i) == observer) {
                m_weakObservers.erase(m_weakObservers.begin() + i);
                m_strongObservers.erase(m_strongObservers.begin() + i);
                break;
            }
        }
    }

    void UnregisterWeak(const std::weak_ptr<Observer>& observer) override
    {
        auto o = observer.lock();
        if (o) {
            UnregisterShared(o);
        }
    }

    void UnregisterRaw(const Observer* observer) override
    {
        std::lock_guard<std::mutex> lock(m_mtx);

        // O(N), but that's ok because I expect N to be small.
        for (size_t i(0); i < m_weakObservers.size(); ++i) {
            if (GetObserver(i).get() == observer) {
                m_weakObservers.erase(m_weakObservers.begin() + i);
                m_strongObservers.erase(m_strongObservers.begin() + i);
                break;
            }
        }
    }

protected:
    std::vector<std::weak_ptr<Observer>> GetObservers() const
    {
        std::lock_guard<std::mutex> lock(m_mtx);
        return m_weakObservers;
    }

    void ClearObservers()
    {
        std::lock_guard<std::mutex> lock(m_mtx);
        m_weakObservers.clear();
        m_strongObservers.clear();
    }

    // Apply function to all observers
    template <typename Func>
    void VisitObservers(Func func) const
    {
        for (size_t i(0); i < m_weakObservers.size(); ++i) {
            auto o = GetObserver(i);
            if (o) {
                func(*o);
            }
        }
    }

    // Call a member function of the observer
    //
    // Example:
    //   class MyObserver {
    //   public:
    //       void OnSomethingHappened(const char* a, int b);
    //   }
    //
    //   NotifyObservers(&MyObserver::OnSomethingHappened, "foo", 23);
    template <typename MemFunc, typename... Args>
    void NotifyObservers(MemFunc func, Args&&... args) const
    {
        for (size_t i(0); i < m_weakObservers.size(); ++i) {
            auto o = GetObserver(i);
            if (o) {
                // DEB-47623
                // A crash caused by a corrupted observer pointer happened in field multiple times
                // which we cannot pin-point where the culprit is. Check against the corrupted
                // observer for now to prevent the crash.
                if (o.get() != CORRUPTED_OBSERVER) {
                    (o.get()->*func)(args...);
                } else {
                    char cmd[1024];
                    snprintf(cmd, 1024, "logger ObservableImpl::NotifyObservers skip corrupted observer. observer=%p", (void*)o.get());
                    system(cmd);
                }
            }
        }
    }

private:
    std::shared_ptr<Observer> GetObserver(size_t i) const
    {
        if (m_strongObservers[i]) {
            return m_strongObservers[i];
        } else {
            return m_weakObservers[i].lock();
        }
    }

    mutable std::mutex m_mtx;
    std::vector<std::shared_ptr<Observer>> m_strongObservers;
    std::vector<std::weak_ptr<Observer>> m_weakObservers;
};

} // namespace tt
