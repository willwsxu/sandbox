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

// Helpers for use with the pimpl pattern. This allows the top-level observable
// class to forward calls to the implementation.
//
// Example:
//
//     class FooImpl;
//
//     class Foo : public Observable<Bar>
//     {
//     public:
//         TTSTL_OBSERVABLE_IFACE_OVERRIDE(Bar)
//
//     private:
//         shared_ptr<FooImpl> m_impl;
//     };
//
//     TTSTL_OBSERVABLE_FWD(Bar, Foo, m_impl)
//

// Use this macro in a class definition to declare an override
#define TTSTL_OBSERVABLE_IFACE_OVERRIDE(_observer) \
    virtual void RegisterShared(std::shared_ptr<_observer> observer) override; \
    virtual void RegisterWeak(std::weak_ptr<_observer> observer) override; \
    virtual void UnregisterShared(const std::shared_ptr<_observer>& observer) override; \
    virtual void UnregisterWeak(const std::weak_ptr<_observer>& observer) override; \
    virtual void UnregisterRaw(const _observer* observer) override;

// Use this macro to forward method calls to the impl
#define TTSTL_OBSERVABLE_FWD(_observer, _class, _impl) \
    void _class::RegisterShared(std::shared_ptr<_observer> observer) { \
        _impl->RegisterShared(std::move(observer)); } \
    void _class::RegisterWeak(std::weak_ptr<_observer> observer) { \
        _impl->RegisterWeak(std::move(observer)); } \
    void _class::UnregisterShared(const std::shared_ptr<_observer>& observer) { \
        _impl->UnregisterShared(observer); } \
    void _class::UnregisterWeak(const std::weak_ptr<_observer>& observer) { \
        _impl->UnregisterWeak(observer); } \
    void _class::UnregisterRaw(const _observer* observer) { \
        _impl->UnregisterRaw(observer); }
