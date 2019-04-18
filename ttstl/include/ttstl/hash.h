#pragma once

#include <cstdint>

namespace tt {

inline unsigned long string_hash(const char* str)
{
    unsigned long hash = 5381;
    int c;

    while ((c = *str++))
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash;
}

// Implementation of FNV hash (see GCC's libstdc++) for 32bit/64bit size_t.
inline size_t fnv_hash(const void* ptr, size_t len, size_t hash = static_cast<size_t>(2166136261UL))
{
    static_assert((sizeof(size_t) == 4) || (sizeof(size_t) == 8), "size_t has non-standard size");
    static constexpr uint64_t fnv_prime = (sizeof(size_t) == 4) 
        ? static_cast<uint64_t>(16777619UL)
        : static_cast<uint64_t>(1099511628211ULL);

    const char* cptr = static_cast<const char*>(ptr);
    for (; len; --len)
    {
        hash ^= static_cast<size_t>(*cptr++);
        hash *= static_cast<size_t>(fnv_prime);
    }
    return hash;
}

} // namespace tt
