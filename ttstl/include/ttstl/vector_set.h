/***************************************************************************
 *
 *                  Unpublished Work Copyright (c) 2014-2015
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

#include <algorithm>
#include <vector>

#include <boost/compressed_pair.hpp>

namespace tt {

template <
    typename Key,
    typename Compare = std::less<Key>,
    typename Allocator = std::allocator<Key>,
    typename Container = std::vector<Key, Allocator>
>
class VectorSet
{
public:
    using key_type = Key;
    using value_type = Key;
    using size_type = typename Container::size_type;
    using difference_type = typename Container::difference_type;
    using key_compare = Compare;
    using value_compare = Compare;
    using allocator_type = Allocator;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = typename std::allocator_traits<Allocator>::pointer;
    using const_pointer = typename std::allocator_traits<Allocator>::const_pointer;
    using iterator = typename Container::iterator;
    using const_iterator = typename Container::const_iterator;

    explicit VectorSet(
        const Compare& compare = Compare(),
        const Allocator& alloc = Allocator())
        : m_data(Container(alloc), compare)
    {
    }

    template <typename InputIt>
    VectorSet(
        InputIt first,
        InputIt last,
        const Compare& compare = Compare(),
        const Allocator& alloc = Allocator())
        : VectorSet(compare, alloc)
    {
        auto& data = Data();
        data.assign(first, last);
        std::sort(data.begin(), data.end(), Cmp());
    }

    VectorSet(
        std::initializer_list<value_type> init,
        const Compare& compare = Compare(),
        const Allocator& alloc = Allocator())
        : VectorSet(compare, alloc)
    {
        auto& data = Data();
        data.assign(std::begin(init), std::end(init));
        std::sort(data.begin(), data.end(), Cmp());
    }

    VectorSet(const VectorSet& other) = default;
    VectorSet(VectorSet&& other) = default;
    VectorSet& operator=(const VectorSet& other) = default;
    VectorSet& operator=(VectorSet&& other) = default;

    iterator begin() { return Data().begin(); }
    const_iterator begin() const { return Data().begin(); }
    const_iterator cbegin() const { return Data().cbegin(); }

    iterator end() { return Data().end(); }
    const_iterator end() const { return Data().end(); }
    const_iterator cend() const { return Data().cend(); }

    iterator rbegin() { return Data().rbegin(); }
    const_iterator rbegin() const { return Data().rbegin(); }
    const_iterator crbegin() const { return Data().crbegin(); }

    iterator rend() { return Data().rend(); }
    const_iterator rend() const { return Data().rend(); }
    const_iterator crend() const { return Data().crend(); }

    bool empty() const { return Data().empty(); }
    size_type size() const { return Data().size(); }
    size_type max_size() const { return Data().max_size(); }

    void clear() { Data().clear(); }

    std::pair<iterator,bool> insert(const value_type& value)
    {
        auto iter = lower_bound(value);
        if (IsEqual(iter, value)) {
            return std::make_pair(iter, false);
        } else {
            return std::make_pair(Data().insert(iter, value), true);
        }
    }

    std::pair<iterator,bool> insert(value_type&& value)
    {
        auto iter = lower_bound(value);
        if (IsEqual(iter, value)) {
            return std::make_pair(iter, false);
        } else {
            return std::make_pair(Data().insert(iter, std::move(value)), true);
        }
    }

    template <typename InputIt>
    void insert(InputIt first, InputIt last)
    {
        while (first != last) {
            insert(*first++);
        }
    }

    void insert(std::initializer_list<value_type> ilist)
    {
        for (const value_type& v : ilist) {
            insert(v);
        }
    }

    template <typename... Args>
    std::pair<iterator,bool> emplace(Args&&... args)
    {
        return insert(value_type(std::forward<Args>(args)...));
    }

    iterator erase(const_iterator pos) { return Data().erase(pos); }

    iterator erase(const_iterator first, const_iterator last)
    {
        return Data().erase(first, last);
    }

    size_type erase(const key_type& key)
    {
        auto iter = lower_bound(key);
        if (IsEqual(iter, key)) {
            Data().erase(iter);
            return 1;
        } else {
            return 0;
        }
    }

    template <typename K, typename C = Compare, typename C::is_transparent* = nullptr>
    size_type erase(const K& key)
    {
        auto iter = lower_bound(key);
        if (IsEqual(iter, key)) {
            Data().erase(iter);
            return 1;
        } else {
            return 0;
        }
    }

    void swap(VectorSet& other) { std::swap(m_data, other.m_data); }

    size_type count(const key_type& key) const
    {
        return find(key) == Data().cend() ? 0 : 1;
    }

    template <typename K, typename C = Compare, typename C::is_transparent* = nullptr>
    size_type count(const K& key)
    {
        return find(key) == Data().cend() ? 0 : 1;
    }

    iterator find(const key_type& key)
    {
        auto iter = lower_bound(key);
        if (IsEqual(iter, key)) {
            return iter;
        } else {
            return Data().end();
        }
    }

    const_iterator find(const key_type& key) const
    {
        auto iter = lower_bound(key);
        if (IsEqual(iter, key)) {
            return iter;
        } else {
            return Data().cend();
        }
    }

    template <typename K, typename C = Compare, typename C::is_transparent* = nullptr>
    iterator find(const K& key)
    {
        auto iter = lower_bound(key);
        if (IsEqual(iter, key)) {
            return iter;
        } else {
            return Data().end();
        }
    }

    template <typename K, typename C = Compare, typename C::is_transparent* = nullptr>
    const_iterator find(const K& key) const
    {
        auto iter = lower_bound(key);
        if (IsEqual(iter, key)) {
            return iter;
        } else {
            return Data().cend();
        }
    }

    iterator lower_bound(const key_type& key)
    {
        return std::lower_bound(Data().begin(), Data().end(), key, Cmp());
    }

    const_iterator lower_bound(const key_type& key) const
    {
        return std::lower_bound(Data().cbegin(), Data().cend(), key, Cmp());
    }

    template <typename K, typename C = Compare, typename C::is_transparent* = nullptr>
    iterator lower_bound(const K& key)
    {
        return std::lower_bound(Data().begin(), Data().end(), key, Cmp());
    }

    template <typename K, typename C = Compare, typename C::is_transparent* = nullptr>
    const_iterator lower_bound(const K& key) const
    {
        return std::lower_bound(Data().cbegin(), Data().cend(), key, Cmp());
    }

    void reserve(size_t count)
    {
        Data().reserve(count);
    }

private:
    template <typename K, typename C, typename A>
    friend bool operator==(
        const VectorSet<K, C, A>& lhs,
        const VectorSet<K, C, A>& rhs);

    template <typename K, typename C, typename A>
    friend bool operator!=(
        const VectorSet<K, C, A>& lhs,
        const VectorSet<K, C, A>& rhs);

    Container& Data() { return m_data.first(); }
    const Container& Data() const { return m_data.first(); }

    Compare& Cmp() { return m_data.second(); }
    const Compare& Cmp() const { return m_data.second(); }

    template <typename I, typename V>
    bool IsEqual(const I& iter, const V& value) const
    {
        return iter != Data().end() && !Cmp()(value, *iter);
    }

    boost::compressed_pair<Container, Compare> m_data;
};

template <typename Key, typename Compare, typename Allocator>
inline bool operator==(
    const VectorSet<Key, Compare, Allocator>& lhs,
    const VectorSet<Key, Compare, Allocator>& rhs)
{
    return lhs.m_data.first() = rhs.m_data.first();
}

template <typename Key, typename Compare, typename Allocator>
inline bool operator!=(
    const VectorSet<Key, Compare, Allocator>& lhs,
    const VectorSet<Key, Compare, Allocator>& rhs)
{
    return lhs.m_data.first() != rhs.m_data.first();
}

} // namespace tt

namespace std {

template <typename Key, typename Compare, typename Allocator>
inline void swap(
    tt::VectorSet<Key, Compare, Allocator>& lhs,
    tt::VectorSet<Key, Compare, Allocator>& rhs)
{
    return lhs.swap(rhs);
}

} // namespace std
