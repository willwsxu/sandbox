/***************************************************************************
*
*                  Unpublished Work Copyright (c) 2012-2014
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
#include <array>
#include <mutex>
#include <type_traits>

#include <ttstl/platform.h>

namespace tt {

#if defined(ARCH_X86) || defined(ARCH_X86_64)
  #define cpu_relax() _mm_pause()
#else
  #define cpu_relax()
#endif

template <typename T, size_t N>
class alignas(TT_CACHE_LINE_SIZE) SPSCRingBuffer
{
public:
    SPSCRingBuffer()
        : m_head(0)
        , m_tail(0)
    {}

    ~SPSCRingBuffer() = default;

    void Push(const T& v)
    {
        // Wait until a spot becomes available
        const uint64_t head = m_head.load(std::memory_order_relaxed);
        while (head == m_tail.load(std::memory_order_acquire) + N) {
            cpu_relax();
        }

        // Assign value to node
        new (&m_ring[head % N].value) T(v);

        // Increment head pointer to make this node available to consumers
        m_head.store(head + 1, std::memory_order_release);
    }

    void Push(T&& v)
    {
        // Wait until a spot becomes available
        const uint64_t head = m_head.load(std::memory_order_relaxed);
        while (head == m_tail.load(std::memory_order_acquire) + N) {
            cpu_relax();
        }

        // Assign value to node
        new (&m_ring[head % N].value) T(std::move(v));

        // Increment head pointer to make this node available to consumers
        m_head.store(head + 1, std::memory_order_release);
    }

    template <typename... Args>
    void Emplace(Args&&... args)
    {
        // Wait until a spot becomes available
        const uint64_t head = m_head.load(std::memory_order_relaxed);
        while (head == m_tail.load(std::memory_order_acquire) + N) {
            cpu_relax();
        }

        // Assign value to node
        new (&m_ring[head % N].value) T(std::forward<Args>(args)...);

        // Increment head pointer to make this node available to consumers
        m_head.store(head + 1, std::memory_order_release);
    }

    T Pop()
    {
        // Wait until a spot becomes available
        const uint64_t tail = m_tail.load(std::memory_order_relaxed);
        while (tail == m_head.load(std::memory_order_acquire)) {
            cpu_relax();
        }

        T* value = reinterpret_cast<T*>(&m_ring[tail % N].value);
        T v(std::move(*value));
        value->~T();

        // Increment tail pointer to make this node available to producers
        m_tail.store(tail + 1, std::memory_order_release);

        return v;
    }

    bool CanPush() const
    {
        return m_head.load(std::memory_order_relaxed) <
               m_tail.load(std::memory_order_acquire) + N;
    }

    bool CanPop() const
    {
        return m_tail.load(std::memory_order_relaxed) <
               m_head.load(std::memory_order_acquire);
    }

private:
    SPSCRingBuffer(const SPSCRingBuffer&) = delete;
    void operator=(const SPSCRingBuffer&) = delete;
    SPSCRingBuffer(SPSCRingBuffer&&) = delete;
    void operator=(SPSCRingBuffer&&) = delete;

    struct Node
    {
        typename std::aligned_storage<
            sizeof(T),
            std::alignment_of<T>::value
        >::type value;
    };

    std::atomic<uint64_t> m_head;
    char m_pad1[TT_CACHE_LINE_SIZE - 8];
    std::atomic<uint64_t> m_tail;
    char m_pad2[TT_CACHE_LINE_SIZE - 8];
    std::array<Node, N> m_ring;
};

template <typename T, size_t N>
class alignas(TT_CACHE_LINE_SIZE) MPSCPodRingBuffer
{
public:
    MPSCPodRingBuffer()
        : m_head(0)
        , m_hres(0)
        , m_tail(0)
    {}

    ~MPSCPodRingBuffer() = default;

    void Push(const T& v)
    {
        // Reserve a spot and wait until it becomes available
        const uint64_t head = m_hres.fetch_add(1, std::memory_order_acq_rel);
        while (head >= m_tail.load(std::memory_order_acquire) + N) {
            cpu_relax();
        }

        // Assign value to node
        m_ring[head % N] = v;

        // Wait until it's our turn to increment the head pointer
        while (head > m_head.load(std::memory_order_acquire)) {
            cpu_relax();
        }

        // Increment head pointer to make this node available to consumers
        m_head.store(head + 1, std::memory_order_release);
    }

    void Push(T&& v)
    {
        // Reserve a spot and wait until it becomes available
        const uint64_t head = m_hres.fetch_add(1, std::memory_order_acq_rel);
        while (head >= m_tail.load(std::memory_order_acquire) + N) {
            cpu_relax();
        }

        // Assign value to node
        m_ring[head % N] = std::move(v);

        // Wait until it's our turn to increment the head pointer
        while (head > m_head.load(std::memory_order_acquire)) {
            cpu_relax();
        }

        // Increment head pointer to make this node available to consumers
        m_head.store(head + 1, std::memory_order_release);
    }

    T Pop()
    {
        // Wait until a spot becomes available
        const uint64_t tail = m_tail.load(std::memory_order_relaxed);
        while (tail == m_head.load(std::memory_order_acquire)) {
            cpu_relax();
        }

        T* value = reinterpret_cast<T*>(&m_ring[tail % N]);
        T v(std::move(*value));

        // Increment tail pointer to make this node available to producers
        m_tail.store(tail + 1, std::memory_order_release);

        return v;
    }

    const T& Front()
    {
        // Wait until a spot becomes available
        const uint64_t tail = m_tail.load(std::memory_order_relaxed);
        while (tail == m_head.load(std::memory_order_acquire)) {
            cpu_relax();
        }

        T* value = reinterpret_cast<T*>(&m_ring[tail % N].value);
        return *value;
    }

    bool CanPop() const
    {
        return m_tail.load(std::memory_order_relaxed) <
               m_head.load(std::memory_order_acquire);
    }

private:
    MPSCPodRingBuffer(const MPSCPodRingBuffer&) = delete;
    void operator=(const MPSCPodRingBuffer&) = delete;
    MPSCPodRingBuffer(MPSCPodRingBuffer&&) = delete;
    void operator=(MPSCPodRingBuffer&&) = delete;

    std::atomic<uint64_t> m_head;
    std::atomic<uint64_t> m_hres;
    char m_pad1[TT_CACHE_LINE_SIZE - 16];
    std::atomic<uint64_t> m_tail;
    char m_pad2[TT_CACHE_LINE_SIZE - 8];
    std::array<T, N> m_ring;
};



template <typename T, size_t N>
class alignas(TT_CACHE_LINE_SIZE) MPSCRingBuffer
{
public:
    MPSCRingBuffer()
        : m_head(0)
        , m_hres(0)
        , m_tail(0)
    {}

    ~MPSCRingBuffer() = default;

    void Push(const T& v)
    {
        // Reserve a spot and wait until it becomes available
        const uint64_t head = m_hres.fetch_add(1, std::memory_order_acq_rel);
        while (head >= m_tail.load(std::memory_order_acquire) + N) {
            cpu_relax();
        }

        // Assign value to node
        new (&m_ring[head % N].value) T(v);

        // Wait until it's our turn to increment the head pointer
        while (head > m_head.load(std::memory_order_acquire)) {
            cpu_relax();
        }

        // Increment head pointer to make this node available to consumers
        m_head.store(head + 1, std::memory_order_release);
    }

    void Push(T&& v)
    {
        // Reserve a spot and wait until it becomes available
        const uint64_t head = m_hres.fetch_add(1, std::memory_order_acq_rel);
        while (head >= m_tail.load(std::memory_order_acquire) + N) {
            cpu_relax();
        }

        // Assign value to node
        new (&m_ring[head % N].value) T(std::move(v));

        // Wait until it's our turn to increment the head pointer
        while (head > m_head.load(std::memory_order_acquire)) {
            cpu_relax();
        }

        // Increment head pointer to make this node available to consumers
        m_head.store(head + 1, std::memory_order_release);
    }

    template <typename... Args>
    void Emplace(Args&&... args)
    {
        // Reserve a spot and wait until it becomes available
        const uint64_t head = m_hres.fetch_add(1, std::memory_order_acq_rel);
        while (head >= m_tail.load(std::memory_order_acquire) + N) {
            cpu_relax();
        }

        // Assign value to node
        new (&m_ring[head % N].value) T(std::forward<Args>(args)...);

        // Wait until it's our turn to increment the head pointer
        while (head > m_head.load(std::memory_order_acquire)) {
            cpu_relax();
        }

        // Increment head pointer to make this node available to consumers
        m_head.store(head + 1, std::memory_order_release);
    }

    T Pop()
    {
        // Wait until a spot becomes available
        const uint64_t tail = m_tail.load(std::memory_order_relaxed);
        while (tail == m_head.load(std::memory_order_acquire)) {
            cpu_relax();
        }

        T* value = reinterpret_cast<T*>(&m_ring[tail % N].value);
        T v(std::move(*value));
        value->~T();

        // Increment tail pointer to make this node available to producers
        m_tail.store(tail + 1, std::memory_order_release);

        return v;
    }

    const T& Front()
    {
        // Wait until a spot becomes available
        const uint64_t tail = m_tail.load(std::memory_order_relaxed);
        while (tail == m_head.load(std::memory_order_acquire)) {
            cpu_relax();
        }

        T* value = reinterpret_cast<T*>(&m_ring[tail % N].value);
        return *value;
    }

    void Drop()
    {
        // Increment tail pointer to make this node available to producers
        const uint64_t tail = m_tail.load(std::memory_order_relaxed);
        T* value = reinterpret_cast<T*>(&m_ring[tail % N].value);
        value->~T();
        m_tail.store(tail + 1, std::memory_order_release);
    }

    bool CanPop() const
    {
        return m_tail.load(std::memory_order_relaxed) <
               m_head.load(std::memory_order_acquire);
    }

private:
    MPSCRingBuffer(const MPSCRingBuffer&) = delete;
    void operator=(const MPSCRingBuffer&) = delete;
    MPSCRingBuffer(MPSCRingBuffer&&) = delete;
    void operator=(MPSCRingBuffer&&) = delete;

    struct Node
    {
        typename std::aligned_storage<
            sizeof(T),
            std::alignment_of<T>::value
        >::type value;
    };

    std::atomic<uint64_t> m_head;
    std::atomic<uint64_t> m_hres;
    char m_pad1[TT_CACHE_LINE_SIZE - 16];
    std::atomic<uint64_t> m_tail;
    char m_pad2[TT_CACHE_LINE_SIZE - 8];
    std::array<Node, N> m_ring;
};

template <typename T, size_t N>
class alignas(TT_CACHE_LINE_SIZE) SPMCRingBuffer
{
public:
    SPMCRingBuffer()
        : m_head(0)
        , m_tail(0)
        , m_tres(0)
    {}

    ~SPMCRingBuffer() = default;

    void Push(const T& v)
    {
        // Wait until a spot becomes available
        const uint64_t head = m_head.load(std::memory_order_relaxed);
        while (head == m_tail.load(std::memory_order_acquire) + N) {
            cpu_relax();
        }

        // Assign value to node
        new (&m_ring[head % N].value) T(v);

        // Increment head pointer to make this node available to consumers
        m_head.store(head + 1, std::memory_order_release);
    }

    void Push(T&& v)
    {
        // Wait until a spot becomes available
        const uint64_t head = m_head.load(std::memory_order_relaxed);
        while (head == m_tail.load(std::memory_order_acquire) + N) {
            cpu_relax();
        }

        // Assign value to node
        new (&m_ring[head % N].value) T(std::move(v));

        // Increment head pointer to make this node available to consumers
        m_head.store(head + 1, std::memory_order_release);
    }

    template <typename... Args>
    void Emplace(Args&&... args)
    {
        // Wait until a spot becomes available
        const uint64_t head = m_head.load(std::memory_order_relaxed);
        while (head == m_tail.load(std::memory_order_acquire) + N) {
            cpu_relax();
        }

        // Assign value to node
        new (&m_ring[head % N].value) T(std::forward<Args>(args)...);

        // Increment head pointer to make this node available to consumers
        m_head.store(head + 1, std::memory_order_release);
    }

    T Pop()
    {
        // Reserve a spot and wait until it becomes available
        const uint64_t tail = m_tres.fetch_add(1, std::memory_order_acq_rel);
        while (tail >= m_head.load(std::memory_order_acquire)) {
            cpu_relax();
        }

        T* value = reinterpret_cast<T*>(&m_ring[tail % N].value);
        T v(std::move(*value));
        value->~T();

        // Wait until it's our turn to increment the tail pointer
        while (tail > m_tail.load(std::memory_order_acquire)) {
            cpu_relax();
        }

        // Increment tail pointer to make this node available to producers
        m_tail.store(tail + 1, std::memory_order_release);

        return v;
    }

    bool CanPush() const
    {
        return m_head.load(std::memory_order_relaxed) <
               m_tail.load(std::memory_order_acquire) + N;
    }

private:
    SPMCRingBuffer(const SPMCRingBuffer&) = delete;
    void operator=(const SPMCRingBuffer&) = delete;
    SPMCRingBuffer(SPMCRingBuffer&&) = delete;
    void operator=(SPMCRingBuffer&&) = delete;

    struct Node
    {
        typename std::aligned_storage<
            sizeof(T),
            std::alignment_of<T>::value
        >::type value;
    };

    std::atomic<uint64_t> m_head;
    char m_pad1[TT_CACHE_LINE_SIZE - 8];
    std::atomic<uint64_t> m_tail;
    std::atomic<uint64_t> m_tres;
    char m_pad2[TT_CACHE_LINE_SIZE - 16];
    std::array<Node, N> m_ring;
};

template <typename T, size_t N>
class alignas(TT_CACHE_LINE_SIZE) MPMCRingBuffer
{
public:
    MPMCRingBuffer()
        : m_head(0)
        , m_hres(0)
        , m_tail(0)
        , m_tres(0)
    {}

    ~MPMCRingBuffer() = default;

    void Push(const T& v)
    {
        // Reserve a spot and wait until it becomes available
        const uint64_t head = m_hres.fetch_add(1, std::memory_order_acq_rel);
        while (head >= m_tail.load(std::memory_order_acquire) + N) {
            cpu_relax();
        }

        // Assign value to node
        new (&m_ring[head % N].value) T(v);

        // Wait until it's our turn to increment the head pointer
        while (head > m_head.load(std::memory_order_acquire)) {
            cpu_relax();
        }

        // Increment head pointer to make this node available to consumers
        m_head.store(head + 1, std::memory_order_release);
    }

    void Push(T&& v)
    {
        // Reserve a spot and wait until it becomes available
        const uint64_t head = m_hres.fetch_add(1, std::memory_order_acq_rel);
        while (head >= m_tail.load(std::memory_order_acquire) + N) {
            cpu_relax();
        }

        // Assign value to node
        new (&m_ring[head % N].value) T(std::move(v));

        // Wait until it's our turn to increment the head pointer
        while (head > m_head.load(std::memory_order_acquire)) {
            cpu_relax();
        }

        // Increment head pointer to make this node available to consumers
        m_head.store(head + 1, std::memory_order_release);
    }

    template <typename... Args>
    void Emplace(Args&&... args)
    {
        // Reserve a spot and wait until it becomes available
        const uint64_t head = m_hres.fetch_add(1, std::memory_order_acq_rel);
        while (head >= m_tail.load(std::memory_order_acquire) + N) {
            cpu_relax();
        }

        // Assign value to node
        new (&m_ring[head % N].value) T(std::forward<Args>(args)...);

        // Wait until it's our turn to increment the head pointer
        while (head > m_head.load(std::memory_order_acquire)) {
            cpu_relax();
        }

        // Increment head pointer to make this node available to consumers
        m_head.store(head + 1, std::memory_order_release);
    }

    T Pop()
    {
        // Reserve a spot and wait until it becomes available
        const uint64_t tail = m_tres.fetch_add(1, std::memory_order_acq_rel);
        while (tail >= m_head.load(std::memory_order_acquire)) {
            cpu_relax();
        }

        T* value = reinterpret_cast<T*>(&m_ring[tail % N].value);
        T v(std::move(*value));
        value->~T();

        // Wait until it's our turn to increment the tail pointer
        while (tail > m_tail.load(std::memory_order_acquire)) {
            cpu_relax();
        }

        // Increment tail pointer to make this node available to producers
        m_tail.store(tail + 1, std::memory_order_release);

        return v;
    }

private:
    MPMCRingBuffer(const MPMCRingBuffer&) = delete;
    void operator=(const MPMCRingBuffer&) = delete;
    MPMCRingBuffer(MPMCRingBuffer&&) = delete;
    void operator=(MPMCRingBuffer&&) = delete;

    struct Node
    {
        typename std::aligned_storage<
            sizeof(T),
            std::alignment_of<T>::value
        >::type value;
    };

    std::atomic<uint64_t> m_head;
    std::atomic<uint64_t> m_hres;
    char m_pad1[TT_CACHE_LINE_SIZE - 16];
    std::atomic<uint64_t> m_tail;
    std::atomic<uint64_t> m_tres;
    char m_pad2[TT_CACHE_LINE_SIZE - 16];
    std::array<Node, N> m_ring;
};

// This locked version wraps the SPSCRingBuffer with mutex to make it thread
// safe. Two mutexes are used, one for producer and one for consumer to avoid
// contention between producers and consumers.
template <typename T, size_t N, typename Mutex>
class alignas(TT_CACHE_LINE_SIZE) LockedRingBuffer
{
public:
    LockedRingBuffer() = default;
    ~LockedRingBuffer() = default;

    void Push(const T& v)
    {
        std::lock_guard<Mutex> lock(m_pmtx);
        m_ring.Push(v);
    }

    void Push(T&& v)
    {
        std::lock_guard<Mutex> lock(m_pmtx);
        m_ring.Push(std::move(v));
    }

    template <typename... Args>
    void Emplace(Args&&... args)
    {
        std::lock_guard<Mutex> lock(m_pmtx);
        m_ring.Push(std::forward<Args>(args)...);
    }

    T Pop()
    {
        std::lock_guard<Mutex> lock(m_cmtx);
        return m_ring.Pop();
    }

private:
    LockedRingBuffer(const LockedRingBuffer&) = delete;
    void operator=(const LockedRingBuffer&) = delete;
    LockedRingBuffer(LockedRingBuffer&&) = delete;
    void operator=(LockedRingBuffer&&) = delete;

    Mutex m_pmtx;
    char m_pad1[TT_CACHE_LINE_SIZE - (sizeof(Mutex) % TT_CACHE_LINE_SIZE)];
    Mutex m_cmtx;
    char m_pad2[TT_CACHE_LINE_SIZE - (sizeof(Mutex) % TT_CACHE_LINE_SIZE)];
    SPSCRingBuffer<T, N> m_ring;
};

} // namespace tt
