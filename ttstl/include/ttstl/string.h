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
 ***************************************************************************/
// Author: John Shaw
#ifndef SUPER_STRING_H_
#define SUPER_STRING_H_

#include <memory>
#include <type_traits>
#include <iterator>
#include <stdexcept>
#include <ostream>
#include <istream>
#include <cstdint>
#include <cassert>
#include <algorithm>
#include <cstdlib>
#include <limits>
#include <ttstl/custom_allocator.h>
#include <ttstl/hash.h>

#ifndef NO_SUPER_STRING_STD_STRING_COMPAT
  #ifndef SUPER_STRING_STD_STRING_COMPAT
    #define SUPER_STRING_STD_STRING_COMPAT
  #endif
#endif

#ifdef SUPER_STRING_STD_STRING_COMPAT
#include <string>
#endif

namespace tt {
namespace detail {

#ifdef _WIN32
template < typename _Iterator, typename _Container>
class __normal_iterator
{
protected:
    _Iterator _M_current;

public:
    typedef typename std::iterator_traits<_Iterator>::iterator_category iterator_category;
    typedef typename std::iterator_traits<_Iterator>::value_type  value_type;
    typedef typename std::iterator_traits<_Iterator>::difference_type difference_type;
    typedef typename std::iterator_traits<_Iterator>::reference reference;
    typedef typename std::iterator_traits<_Iterator>::pointer   pointer;

    __normal_iterator() : _M_current(_Iterator()) { }
     explicit __normal_iterator(const _Iterator& __i) : _M_current(__i) { }

     // Allow iterator to const_iterator conversion
    template < typename _Iter>
    __normal_iterator(const __normal_iterator<_Iter,
        typename std::enable_if<std::is_same<_Iter, typename _Container::pointer>::value, _Container>::type>& __i)
    : _M_current(__i.base()) { }

    // Forward iterator requirements
    reference operator*() const { return *_M_current; }
    pointer operator->() const { return _M_current; }
    __normal_iterator &operator++() { ++_M_current; return *this; }
    __normal_iterator  operator++(int) { return __normal_iterator(_M_current++); }

    // Bidirectional iterator requirements
    __normal_iterator& operator--() { --_M_current; return *this; }
    __normal_iterator operator--(int) { return __normal_iterator(_M_current--); }

    // Random access iterator requirements
    reference operator[](const difference_type& __n) const { return _M_current[__n]; }
    __normal_iterator& operator+=(const difference_type& __n) { _M_current += __n; return *this; }
    __normal_iterator  operator+(const difference_type& __n) const { return __normal_iterator(_M_current + __n); }
    __normal_iterator & operator-=(const difference_type& __n) { _M_current -= __n; return *this; }
    __normal_iterator operator-(const difference_type& __n) const { return __normal_iterator(_M_current - __n); }
    const _Iterator &  base() const { return _M_current; }
};

// Forward iterator requirements
template<typename _IteratorL, typename _IteratorR, typename _Container>
inline bool operator==(const __normal_iterator<_IteratorL, _Container>& __lhs, const __normal_iterator<_IteratorR, _Container>& __rhs)
{ return __lhs.base() == __rhs.base(); }

template < typename _Iterator, typename _Container>
inline bool operator==(const __normal_iterator<_Iterator, _Container>& __lhs, const __normal_iterator<_Iterator, _Container>& __rhs)
{ return __lhs.base() == __rhs.base(); }

template < typename _IteratorL, typename _IteratorR, typename _Container>
inline bool operator!=(const __normal_iterator<_IteratorL, _Container>& __lhs, const __normal_iterator<_IteratorR, _Container>& __rhs)
{ return __lhs.base() != __rhs.base(); }

template < typename _Iterator, typename _Container>
inline bool operator!=(const __normal_iterator<_Iterator, _Container>& __lhs, const __normal_iterator<_Iterator, _Container>& __rhs)
{ return __lhs.base() != __rhs.base(); }

// Random access iterator requirements
template < typename _IteratorL, typename _IteratorR, typename _Container>
inline bool operator<(const __normal_iterator<_IteratorL, _Container>& __lhs, const __normal_iterator<_IteratorR, _Container>& __rhs)
{ return __lhs.base() < __rhs.base(); }

template < typename _Iterator, typename _Container>
inline bool operator<(const __normal_iterator<_Iterator, _Container>& __lhs, const __normal_iterator<_Iterator, _Container>& __rhs)
{ return __lhs.base() < __rhs.base(); }

template < typename _IteratorL, typename _IteratorR, typename _Container>
inline bool operator>(const __normal_iterator<_IteratorL, _Container>& __lhs, const __normal_iterator<_IteratorR, _Container>& __rhs)
{ return __lhs.base() > __rhs.base(); }

template < typename _Iterator, typename _Container>
inline bool operator>(const __normal_iterator<_Iterator, _Container>& __lhs, const __normal_iterator<_Iterator, _Container>& __rhs)
{ return __lhs.base() > __rhs.base(); }

template<typename _IteratorL, typename _IteratorR, typename _Container>
inline bool operator<=(const __normal_iterator<_IteratorL, _Container>& __lhs, const __normal_iterator<_IteratorR, _Container>& __rhs)
{ return __lhs.base() <= __rhs.base(); }

template<typename _Iterator, typename _Container>
inline bool operator<=(const __normal_iterator<_Iterator, _Container>& __lhs, const __normal_iterator<_Iterator, _Container>& __rhs)
{ return __lhs.base() <= __rhs.base(); }

template<typename _IteratorL, typename _IteratorR, typename _Container>
inline bool operator>=(const __normal_iterator<_IteratorL, _Container>& __lhs, const __normal_iterator<_IteratorR, _Container>& __rhs)
{ return __lhs.base() >= __rhs.base(); }

template<typename _Iterator, typename _Container>
inline bool operator>=(const __normal_iterator<_Iterator, _Container>& __lhs, const __normal_iterator<_Iterator, _Container>& __rhs)
{ return __lhs.base() >= __rhs.base(); }

// _GLIBCXX_RESOLVE_LIB_DEFECTS
// According to the resolution of DR179 not only the various comparison
// operators but also operator- must accept mixed iterator/const_iterator
// parameters.
template<typename _IteratorL, typename _IteratorR, typename _Container>
inline typename __normal_iterator<_IteratorL, _Container>::difference_type
operator-(const __normal_iterator<_IteratorL, _Container>& __lhs, const __normal_iterator<_IteratorR, _Container>& __rhs)
{ return __lhs.base() - __rhs.base(); }

template<typename _Iterator, typename _Container>
inline __normal_iterator<_Iterator, _Container>
operator+(typename __normal_iterator<_Iterator, _Container>::difference_type __n, const __normal_iterator<_Iterator, _Container>& __i)
{ return __normal_iterator<_Iterator, _Container>(__i.base() + __n); }
#else
template<typename _Iterator, typename _Container>
using __normal_iterator = __gnu_cxx::__normal_iterator<_Iterator, _Container>;
#endif
 } // namespace detail


template<
    typename CharT,
    typename Traits = ::std::char_traits<CharT>,
    typename Alloc = ::std::allocator<CharT>
>
class SuperString;

using InPlaceString = SuperString<char, ::std::char_traits<char>, InPlaceAllocator<char>>;
using InPlaceRefString = SuperString<char, ::std::char_traits<char>, InPlaceRefAllocator<char>>;
using String = SuperString<char>;

/*
 * SuperString is equivalent to a standard string, but has an extra bit of
 * functionality which allows it to point to a buffer it doesn't own. This is
 * used as a performance optimization to eliminate string copies in certain
 * situations.
 */
template <typename CharT, typename Traits, typename Alloc>
class SuperString
{
    typedef typename Alloc::template rebind<CharT>::other CharTAlloc;

public:
    // This check isn't strictly necessary, but I haven't tested any other
    // character types, so I don't want to simply enable this class until some
    // basic testing has been done.
    static_assert(::std::is_same<CharT, char>::value, "SuperString only supports char type");

    typedef Traits traits_type;
    typedef typename Traits::char_type value_type;
    typedef Alloc allocator_type;

    //typedef typename ::std::allocator_traits<CharTAlloc>::size_type size_type;
    //typedef typename ::std::allocator_traits<CharTAlloc>::difference_type difference_type;
    typedef typename CharTAlloc::size_type size_type;
    typedef typename CharTAlloc::difference_type difference_type;

    typedef value_type& reference;
    typedef const value_type& const_reference;

    //typedef typename ::std::allocator_traits<CharTAlloc>::pointer pointer;
    //typedef typename ::std::allocator_traits<CharTAlloc>::const_pointer const_pointer;
    typedef typename CharTAlloc::pointer pointer;
    typedef typename CharTAlloc::const_pointer const_pointer;

    typedef detail::__normal_iterator<pointer, SuperString>  iterator;
    typedef detail::__normal_iterator<const_pointer, SuperString> const_iterator;

    typedef ::std::reverse_iterator<iterator> reverse_iterator;
    typedef ::std::reverse_iterator<const_iterator> const_reverse_iterator;

    static const size_type npos = -1;
    static const size_type sso_length = 22;

    explicit SuperString(const Alloc& alloc = Alloc())
        : data_(alloc)
    {}

    SuperString(size_type count, CharT c, const Alloc& alloc = Alloc())
        : data_(alloc)
    {
        assign(count, c);
    }

    SuperString(const SuperString& s, size_type pos, size_type count = npos,
                const Alloc& alloc = Alloc())
        : data_(alloc)
    {
        assign(s, pos, count);
    }

    SuperString(const CharT* s, size_type count, const Alloc& alloc = Alloc())
        : data_(alloc)
    {
        assign(s, count);
    }

    SuperString(const CharT* s, const Alloc& alloc = Alloc())
        : data_(alloc)
    {
        assign(s);
    }

    template <typename Iterator>
    SuperString(Iterator first, Iterator last, const Alloc& alloc = Alloc())
        : data_(alloc)
    {
        assign(first, last);
    }

    SuperString(const SuperString& other)
        : data_(Alloc())
    {
        assign(other);
    }

    SuperString(const SuperString& other, const Alloc& alloc)
        : data_(alloc)
    {
        assign(other);
    }

    SuperString(SuperString&& other) noexcept
        : data_(Alloc())
    {
        swap(other);
    }

    SuperString(SuperString&& other, const Alloc& alloc)
        : data_(alloc)
    {
        if (alloc == other.get_allocator()) {
            swap(other);
        } else {
            assign(other);
        }
    }

    SuperString(::std::initializer_list<CharT> l, const Alloc& alloc = Alloc())
        : data_(alloc)
    {
        assign(l);
    }

#ifdef SUPER_STRING_STD_STRING_COMPAT
    SuperString(const ::std::basic_string<CharT>& s)
        : data_(Alloc())
    {
        assign(s);
    }
#endif

    ~SuperString()
    {
        if (data_.ext.flags == F_LONG) {
            data_.deallocate(data_.GetPtr(), data_.ext.capacity+1);
        }
    }

    SuperString& operator=(const SuperString& other)
    {
        assign(other);
        return *this;
    }

    SuperString& operator=(SuperString&& other) noexcept
    {
        swap(other);
        return *this;
    }

    SuperString& operator=(const CharT* s)
    {
        assign(s);
        return *this;
    }

    SuperString& operator=(CharT c)
    {
        assign(1, c);
        return *this;
    }

    SuperString& operator=(::std::initializer_list<CharT> l)
    {
        assign(l);
        return *this;
    }

    operator std::string()
    {
        return std::string(data(), length());
    }


#ifdef SUPER_STRING_STD_STRING_COMPAT
    SuperString& operator=(const ::std::basic_string<CharT>& s)
    {
        assign(s);
        return *this;
    }
#endif

    int compare(const SuperString& s) const noexcept
    {
        return compare(0, size(), s);
    }

#ifdef SUPER_STRING_STD_STRING_COMPAT
    int compare(const ::std::basic_string<CharT>& s) const noexcept
    {
        return compare(0, size(), s);
    }
#endif

    int compare(size_type pos1, size_type count1, const SuperString& s) const
    {
        return compare(pos1, count1, s, 0, s.length());
    }

#ifdef SUPER_STRING_STD_STRING_COMPAT
    int compare(size_type pos1, size_type count1,
                const ::std::basic_string<CharT>& s) const
    {
        return compare(pos1, count1, s, 0, s.length());
    }
#endif

    int compare(size_type pos1, size_type count1,
                const SuperString& s,
                size_type pos2, size_type count2) const
    {
        const size_type length = s.size();
        if (pos2 > length) {
            throw ::std::out_of_range("SuperString::compare");
        }

        count2 = ::std::min(count2, length - pos2);
        return compare(pos1, count1, s.data() + pos2, count2);
    }

#ifdef SUPER_STRING_STD_STRING_COMPAT
    int compare(size_type pos1, size_type count1,
                const ::std::basic_string<CharT>& s,
                size_type pos2, size_type count2) const
    {
        const size_type length = s.size();
        if (pos2 > length) {
            throw ::std::out_of_range("SuperString::compare");
        }

        count2 = ::std::min(count2, length - pos2);
        return compare(pos1, count1, s.data() + pos2, count2);
    }
#endif

    int compare(const CharT* s) const noexcept
    {
        return compare(0, size(), s, traits_type::length(s));
    }

    int compare(size_type pos1, size_type count1, const CharT* s) const
    {
        return compare(pos1, count1, s, traits_type::length(s));
    }

    int compare(size_type pos1, size_type count1, const CharT* s,
                size_type count2) const
    {
        const size_type length = size();
        if (pos1 > length) {
            throw ::std::out_of_range("SuperString::compare");
        }

        count1 = ::std::min(count1, length - pos1);
        const int res = traits_type::compare(Buffer() + pos1, s, ::std::min(count1, count2));
        if (res != 0) {
            return res;
        } else if (count1 < count2) {
            return -1;
        } else if (count1 == count2) {
            return 0;
        } else {
            return 1;
        }
    }

    reference at(size_type n)
    {
        if (n >= size()) {
            throw ::std::out_of_range("SuperString::at");
        }
        return Buffer()[n];
    }

    const_reference at(size_type n) const
    {
        if (n >= size()) {
            throw ::std::out_of_range("SuperString::at");
        }
        return Buffer()[n];
    }

    reference operator[](size_type n) noexcept
    {
        return Buffer()[n];
    }

    const_reference operator[](size_type n) const noexcept
    {
        return Buffer()[n];
    }

    reference front() noexcept
    {
        return operator[](0);
    }

    const_reference front() const noexcept
    {
        return operator[](0);
    }

    reference back() noexcept
    {
        return operator[](size()-1);
    }

    const_reference back() const noexcept
    {
        return operator[](size()-1);
    }

    const_pointer data() const noexcept
    {
        return Buffer();
    }

    const_pointer c_str() const noexcept
    {
        // c_str() may not be null terminated if this string was bound.
        // Use data() if you understand the risk.
        assert(Buffer()[size()] == CharT());
        return data();
    }

    size_type length() const noexcept
    {
        return size();
    }

    size_type size() const noexcept
    {
        if (data_.ext.flags == F_SHORT) {
            return data_.ext.shortLength;
        } else {
            return data_.ext.longLength;
        }
    }

    size_type capacity() const noexcept
    {
        if (data_.ext.flags == F_SHORT) {
            return 22;
        } else if (data_.ext.flags == F_BOUND) {
            return data_.ext.longLength;
        } else { // F_LONG
            return data_.ext.capacity;
        }
    }

    size_type max_size() const noexcept
    {
        return data_.max_size();
    }

    void reserve(size_type newcap)
    {
        Reserve(newcap, true);
    }

    void shrink_to_fit()
    {
        size_type length = size();
        if (data_.ext.flags == F_LONG && data_.ext.capacity > length) {
            CharT* old = data_.GetPtr();
            const size_type oldcap = data_.ext.capacity;
            if (length <= 22) {
                traits_type::copy(data_.buf, old, length+1);
                data_.ext.flags = F_SHORT;
                data_.ext.shortLength = length;
            } else {
                CharT* p = data_.allocate(length+1);
                traits_type::copy(p, old, length+1);
                data_.SetPtr(p);
                data_.ext.capacity = length;
            }
            data_.deallocate(old, oldcap+1);
        }
    }

    bool empty() const noexcept
    {
        return size() == 0;
    }

    iterator begin()
    {
        CopyIfBound();
        return iterator(Buffer());
    }

    const_iterator begin() const noexcept
    {
        return cbegin();
    }

    const_iterator cbegin() const noexcept
    {
        return const_iterator(Buffer());
    }

    iterator end()
    {
        CopyIfBound();
        return iterator(Buffer() + size());
    }

    const_iterator end() const noexcept
    {
        return cend();
    }

    const_iterator cend() const noexcept
    {
        return const_iterator(Buffer() + size());
    }

    reverse_iterator rbegin()
    {
        CopyIfBound();
        return reverse_iterator(end());
    }

    const_reverse_iterator rbegin() const noexcept
    {
        return crbegin();
    }

    const_reverse_iterator crbegin() const noexcept
    {
        return const_reverse_iterator(cend());
    }

    reverse_iterator rend()
    {
        CopyIfBound();
        return reverse_iterator(begin());
    }

    const_reverse_iterator rend() const noexcept
    {
        return crend();
    }

    const_reverse_iterator crend() const noexcept
    {
        return const_reverse_iterator(cbegin());
    }

    SuperString& assign(size_type count, CharT c)
    {
        Reserve(count, false);
        CharT* buf = Buffer();
        traits_type::assign(buf, count, c);
        SetLength(count);
        return *this;
    }

    SuperString& assign(const SuperString& s)
    {
        if (this != &s) {
            assign(s.data(), s.length());
        }
        return *this;
    }

#ifdef SUPER_STRING_STD_STRING_COMPAT
    SuperString& assign(const ::std::basic_string<CharT>& s)
    {
        return assign(s.data(), s.length());
    }
#endif

    SuperString& assign(const SuperString& s, size_type pos, size_type count)
    {
        return assign(s.data() + pos, count);
    }

#ifdef SUPER_STRING_STD_STRING_COMPAT
    SuperString& assign(const ::std::basic_string<CharT>& s, size_type pos,
                        size_type count)
    {
        return assign(s.data() + pos, count);
    }
#endif

    SuperString& assign(SuperString&& s) noexcept
    {
        swap(s);
        return *this;
    }

    SuperString& assign(const CharT* s)
    {
        return assign(s, traits_type::length(s));
    }

    SuperString& assign(const CharT* s, size_type n)
    {
        Reserve(n, false);
        CharT* buf = Buffer();
        if(!isMemoryPreAllocated())
        {
            traits_type::move(buf, s, n);
        }
        SetLength(n);
        return *this;
    }

    template <typename Iterator>
    SuperString& assign(Iterator first, Iterator last)
    {
        auto len = ::std::distance(first, last);
        Reserve(len, false);
        CharT* buf = Buffer();
        for (; first != last; ++first, ++buf) {
            *buf = *first;
        }
        SetLength(len);
        return *this;
    }

    SuperString& assign(::std::initializer_list<CharT> l)
    {
        return assign(l.begin(), l.end());
    }

    allocator_type get_allocator() const noexcept
    {
        return allocator_type(data_);
    }

    void clear() noexcept
    {
        if (data_.ext.flags == F_BOUND) {
            data_.ext.flags = F_SHORT;
        }
        SetLength(0);
    }

    iterator insert(const_iterator pos, CharT c)
    {
        size_type off = pos - cbegin();
        replace(pos, pos, &c, 1);
        return iterator(begin() + off);
    }

    void insert(iterator pos, size_type count, CharT ch)
    {
        replace(pos, pos, count, ch);
    }

    SuperString& insert(size_type pos, size_type count, CharT c)
    {
        return replace(pos, 0, count, c);
    }

    SuperString& insert(size_type pos, const CharT* s)
    {
        return replace(pos, 0, s);
    }

    SuperString& insert(size_type pos, const CharT* s, size_type count)
    {
        return replace(pos, 0, s, count);
    }

    SuperString& insert(size_type pos, const SuperString& s)
    {
        return replace(pos, 0, s.data(), s.length());
    }

#ifdef SUPER_STRING_STD_STRING_COMPAT
    SuperString& insert(size_type pos, const ::std::basic_string<CharT>& s)
    {
        return replace(pos, 0, s.data(), s.length());
    }
#endif

    SuperString& insert(size_type pos, const SuperString& s, size_type pos2,
                        size_type count)
    {
        if (count > (s.length - pos2)) {
            throw ::std::out_of_range("SuperString::insert");
        }
        return replace(pos, 0, s.data() + pos2, count);
    }

#ifdef SUPER_STRING_STD_STRING_COMPAT
    SuperString& insert(size_type pos, const ::std::basic_string<CharT>& s,
                        size_type pos2, size_type count)
    {
        if (count > (s.length - pos2)) {
            throw ::std::out_of_range("SuperString::insert");
        }
        return replace(pos, 0, s.data() + pos2, count);
    }
#endif

    template <typename Iterator>
    iterator insert(const_iterator pos, Iterator first, Iterator last)
    {
        size_type off = pos - cbegin();
        replace(pos, pos, first, last);
        return iterator(begin() + pos);
    }

    iterator insert(const_iterator pos, ::std::initializer_list<CharT> l)
    {
        size_type off = pos - cbegin();
        replace(pos, pos, l);
        return iterator(begin() + pos);
    }

    SuperString& replace(size_type pos, size_type count, const CharT* str,
                         size_type count2)
    {
        size_type length = size();
        if (pos > length) {
            throw ::std::out_of_range("SuperString::replace");
        }

        size_type newLength = length - count + count2;
        Reserve(newLength);
        CharT* buf = Buffer();
        size_type overlap = ::std::min(count, count2);
        if (overlap > 0) {
            traits_type::move(buf + pos, str, overlap);
            pos += overlap;
            str += overlap;
            count2 -= overlap;
            count -= overlap;
        }
        if (count2 > 0) {
            // expand
            traits_type::move(buf + pos + count2, buf + pos, length - pos);
            traits_type::move(buf + pos, str, count2);
        } else if (count > 0) {
            // contract
            traits_type::move(buf + pos, buf + pos + count, length - pos - count);
        }
        SetLength(newLength);
        return *this;
    }

    SuperString& replace(size_type pos, size_type count, const SuperString& s)
    {
        return replace(pos, count, s.data(), s.length());
    }

#ifdef SUPER_STRING_STD_STRING_COMPAT
    SuperString& replace(size_type pos, size_type count,
                         const ::std::basic_string<CharT>& s)
    {
        return replace(pos, count, s.data(), s.length());
    }
#endif

    SuperString& replace(const_iterator first, const_iterator last, const SuperString& s)
    {
        return replace(first - cbegin(), last - first, s.data(), s.length());
    }

#ifdef SUPER_STRING_STD_STRING_COMPAT
    SuperString& replace(const_iterator first, const_iterator last,
                         const ::std::basic_string<CharT>& s)
    {
        return replace(first - cbegin(), last - first, s.data(), s.length());
    }
#endif

    SuperString& replace(size_type pos, size_type count, const SuperString& s,
                    size_type pos2, size_type count2)
    {
        return replace(pos, count, s.data() + pos2, count2);
    }

#ifdef SUPER_STRING_STD_STRING_COMPAT
    SuperString& replace(size_type pos, size_type count,
                         const ::std::basic_string<CharT>& s,
                         size_type pos2, size_type count2)
    {
        return replace(pos, count, s.data() + pos2, count2);
    }
#endif

    template <typename Iterator>
    SuperString& replace(const_iterator first, const_iterator last,
                         Iterator first2, Iterator last2)
    {
        // Slow, but compliant. If you think this is causing a performance
        // problem in your code, please try using one of the other, more
        // efficient, replace methods.
        SuperString s(first2, last2);
        return replace(first - cbegin(), last - first, s);
    }

    SuperString& replace(const_iterator first, const_iterator last, const CharT* str,
                         size_type count)
    {
        return replace(first - cbegin(), last - first, str, count);
    }

    SuperString& replace(size_type pos, size_type count, const CharT* s)
    {
        return replace(pos, count, s, traits_type::length(s));
    }

    SuperString& replace(const_iterator first, const_iterator last, const CharT* s)
    {
        return replace(first - cbegin(), last - first, s, traits_type::length(s));
    }

    SuperString& replace(size_type pos, size_type count, size_type count2, CharT c)
    {
        size_type length = size();
        if (pos > length) {
            throw ::std::out_of_range("SuperString::replace");
        }

        size_type newLength = length - count + count2;
        Reserve(newLength);
        CharT* buf = Buffer();
        size_type overlap = ::std::min(count, count2);
        if (overlap > 0) {
            traits_type::assign(buf + pos, overlap, c);
            pos += overlap;
            count2 -= overlap;
            count -= overlap;
        }
        if (count2 > 0) {
            // exapand
            traits_type::move(buf + pos + count2, buf + pos, length - pos);
            traits_type::assign(buf + pos, count2, c);
        } else if (count > 0) {
            // contract
            traits_type::move(buf + pos, buf + pos + count, length - pos - count);
        }
        SetLength(newLength);
        return *this;
    }

    SuperString& replace(const_iterator first, const_iterator last, size_type count2,
                    CharT c)
    {
        return replace(first - cbegin(), last - first, count2, c);
    }

    SuperString& replace(const_iterator first, const_iterator last,
                         ::std::initializer_list<CharT> l)
    {
        return replace(first - cbegin(), last - first, l.begin(), l.end());
    }

    SuperString& erase(size_type pos = 0, size_type count = npos)
    {
        const size_type length = size();
        if (pos > length) {
            throw ::std::out_of_range("SuperString::erase");
        }

        const size_type rest = length - pos;
        if (count >= rest) { // Truncate
            SetLength(pos);
        } else {
            CopyIfBound();
            CharT* buf = Buffer();
            traits_type::move(buf + pos, buf + pos + count, rest - count);
            SetLength(length - count);
        }

        return *this;
    }

    iterator erase(const_iterator pos)
    {
        size_type off = pos - cbegin();
        erase(off, npos);
        return iterator(begin() + off);
    }

    iterator erase(const_iterator first, const_iterator last)
    {
        size_type off = first - cbegin();
        erase(off, last - first);
        return iterator(begin() + off);
    }

    void resize(size_type count)
    {
        resize(count, CharT());
    }

    void resize(size_type count, CharT c)
    {
        Reserve(count);
        CharT* buf = Buffer();
        size_type length = size();
        if (count > length) {
            traits_type::assign(buf + length, count - length, c);
        }
        SetLength(count);
    }

    // Same as resize, but does not memset the array
    void fast_resize(size_type count)
    {
        Reserve(count);
        SetLength(count);
    }

    void push_back(CharT c)
    {
        append(1, c);
    }

    void pop_back() noexcept
    {
        SetLength(size()-1);
    }

    SuperString& append(size_type count, CharT c)
    {
        return replace(size(), 0, count, c);
    }

    SuperString& append(const SuperString& s)
    {
        return replace(size(), 0, s);
    }

#ifdef SUPER_STRING_STD_STRING_COMPAT
    SuperString& append(const ::std::basic_string<CharT>& s)
    {
        return replace(size(), 0, s);
    }
#endif

    SuperString& append(const SuperString& s, size_type pos, size_type count)
    {
        return replace(size(), 0, s, pos, count);
    }

#ifdef SUPER_STRING_STD_STRING_COMPAT
    SuperString& append(const ::std::basic_string<CharT>& s, size_type pos,
                        size_type count)
    {
        return replace(size(), 0, s, pos, count);
    }
#endif

    SuperString& append(const CharT* s, size_type count)
    {
        return replace(size(), 0, s, count);
    }

    SuperString& append(const CharT* s)
    {
        return replace(size(), 0, s);
    }

    template <typename Iterator>
    SuperString& append(Iterator first, Iterator last)
    {
        return replace(cend(), cend(), first, last);
    }

    SuperString& append(std::initializer_list<CharT> l)
    {
        return replace(size(), 0, l);
    }

    SuperString& operator+=(const SuperString& s)
    {
        return append(s);
    }

#ifdef SUPER_STRING_STD_STRING_COMPAT
    SuperString& operator+=(const ::std::basic_string<CharT>& s)
    {
        return append(s);
    }
#endif

    SuperString& operator+=(CharT c)
    {
        return append(1, c);
    }

    SuperString& operator+=(const CharT* s)
    {
        return append(s);
    }

    SuperString& operator+=(std::initializer_list<CharT> l)
    {
        return append(l);
    }

    SuperString substr(size_type pos = 0, size_type count = npos) const
    {
        size_type length = size();

        if (pos > length) {
            throw ::std::out_of_range("SuperString::substr");
        }

        count = ::std::min(count, length - pos);

        return SuperString(data() + pos, count);
    }

    size_type copy(CharT* s, size_type count, size_type pos = 0) const
    {
        size_type length = size();

        if (pos > length) {
            throw ::std::out_of_range("SuperString::substr");
        }

        count = ::std::min(count, length - pos);

        traits_type::move(s, Buffer() + pos, count);
    }

    // Somehow iterators must remain valid, but I don't see how that is
    // possible with SSO.
    void swap(SuperString& other) noexcept
    {
        if (this == &other) {
            return;
        }

        ::std::swap(data_, other.data_);
    }

    size_type find(const CharT* s, size_type pos, size_type count) const noexcept
    {
        size_type length = size();

        if (count == 0) {
            return pos <= length ? pos : npos;
        }

        if (count <= length) {
            const CharT* buf = Buffer();
            for (; pos <= length - count; ++pos) {
                if (traits_type::eq(buf[pos], s[0]) &&
                    traits_type::compare(buf + pos + 1, s + 1, count - 1) == 0) {
                    return pos;
                }
            }
        }

        return npos;
    }

    size_type find(const SuperString& s, size_type pos = 0) const noexcept
    {
        return find(s.data(), pos, s.length());
    }

#ifdef SUPER_STRING_STD_STRING_COMPAT
    size_type find(const ::std::basic_string<CharT>& s, size_type pos = 0) const noexcept
    {
        return find(s.data(), pos, s.length());
    }
#endif

    size_type find(const CharT* s, size_type pos = 0) const noexcept
    {
        return find(s, pos, traits_type::length(s));
    }

    size_type find(CharT c, size_type pos = 0) const noexcept
    {
        const CharT* buf = Buffer();

        size_type length = size();
        for (; pos <= length; ++pos) {
            if (buf[pos] == c) {
                return pos;
            }
        }

        return npos;
    }

    size_type rfind(const CharT* s, size_type pos, size_type count) const noexcept
    {
        size_type length = size();

        if (count == 0) {
            return pos <= length ? pos : npos;
        }

        if (count <= length) {
            pos = ::std::min(length- count, pos);
            const CharT* buf = Buffer();
            do {
                if (traits_type::compare(buf + pos, s, count) == 0) {
                    return pos;
                }
            } while (pos-- > 0);
        }

        return npos;
    }

    size_type rfind(const SuperString& s, size_type pos = npos) const noexcept
    {
        return rfind(s.data(), pos, s.length());
    }

#ifdef SUPER_STRING_STD_STRING_COMPAT
    size_type rfind(const ::std::basic_string<CharT>& s, size_type pos = npos) const noexcept
    {
        return rfind(s.data(), pos, s.length());
    }
#endif

    size_type rfind(const CharT* s, size_type pos = npos) const noexcept
    {
        return rfind(s, pos, traits_type::length(s));
    }

    size_type rfind(CharT c, size_type pos = npos) const noexcept
    {
        size_type length = size();

        if (length > 0) {
            const CharT* buf = Buffer();
            if (--length > pos) {
                length = pos;
            }
            for (++length; length-- > 0; ) {
                if (traits_type::eq(buf[length], c)) {
                    return length;
                }
            }
        }

        return npos;
    }

    size_type find_first_of(const CharT* s, size_type pos, size_type count) const noexcept
    {
        const CharT* buf = Buffer();
        size_type length = size();
        for (; count && pos < length; ++pos) {
            if (traits_type::find(s, count, buf[pos])) {
                return pos;
            }
        }
        return npos;
    }

    size_type find_first_of(const SuperString& s, size_type pos = 0) const noexcept
    {
        return find_first_of(s.data(), pos, s.length());
    }

#ifdef SUPER_STRING_STD_STRING_COMPAT
    size_type find_first_of(const ::std::basic_string<CharT>& s, size_type pos = 0) const noexcept
    {
        return find_first_of(s.data(), pos, s.length());
    }
#endif

    size_type find_first_of(const CharT* s, size_type pos = 0) const noexcept
    {
        return find_first_of(s, pos, traits_type::length(s));
    }

    size_type find_first_of(CharT c, size_type pos = 0) const noexcept
    {
        return find(c, pos);
    }

    size_type find_first_not_of(const CharT* s, size_type pos, size_type count) const noexcept
    {
        const CharT* buf = Buffer();
        size_type length = size();
        for (; pos < length; ++pos) {
            if (!traits_type::find(s, count, buf[pos])) {
                return pos;
            }
        }
        return npos;
    }

    size_type find_first_not_of(const SuperString& s, size_type pos = 0) const noexcept
    {
        return find_first_not_of(s.data(), pos, s.length());
    }

#ifdef SUPER_STRING_STD_STRING_COMPAT
    size_type find_first_not_of(const ::std::basic_string<CharT>& s, size_type pos = 0) const noexcept
    {
        return find_first_not_of(s.data(), pos, s.length());
    }
#endif

    size_type find_first_not_of(const CharT* s, size_type pos = 0) const noexcept
    {
        return find_first_not_of(s, pos, traits_type::length(s));
    }

    size_type find_first_not_of(CharT c, size_type pos = 0) const noexcept
    {
        const CharT* buf = Buffer();
        size_type length = size();
        for (; pos < length; ++pos) {
            if (!traits_type::eq(buf[pos], c)) {
                return pos;
            }
        }
        return npos;
    }

    size_type find_last_of(const CharT* s, size_type pos, size_type count) const noexcept
    {
        size_type length = size();

        if (length && count) {
            const CharT* buf = Buffer();
            if (--length > pos) {
                length = pos;
            }
            do {
                if (traits_type::find(s, count, buf[length])) {
                    return length;
                }
            } while (length-- != 0);
        }

        return npos;
    }

    size_type find_last_of(const SuperString& s, size_type pos = npos) const noexcept
    {
        return find_last_of(s.data(), pos, s.length());
    }

#ifdef SUPER_STRING_STD_STRING_COMPAT
    size_type find_last_of(const ::std::basic_string<CharT>& s, size_type pos = npos) const noexcept
    {
        return find_last_of(s.data(), pos, s.length());
    }
#endif

    size_type find_last_of(const CharT* s, size_type pos = npos) const noexcept
    {
        return find_last_of(s, pos, traits_type::length(s));
    }

    size_type find_last_of(CharT c, size_type pos = npos) const noexcept
    {
        return rfind(c, pos);
    }

    size_type find_last_not_of(const CharT* s, size_type pos, size_type count) const noexcept
    {
        size_type length = size();

        if (length > 0) {
            const CharT* buf = Buffer();
            if (--length > pos) {
                length = pos;
            }
            do {
                if (traits_type::find(s, count, buf[length])) {
                    return length;
                }
            } while (length--);
        }
        return npos;
    }

    size_type find_last_not_of(const SuperString& s, size_type pos = npos) const noexcept
    {
        return find_last_not_of(s.data(), pos, s.length());
    }

#ifdef SUPER_STRING_STD_STRING_COMPAT
    size_type find_last_not_of(const ::std::basic_string<CharT>& s, size_type pos = npos) const noexcept
    {
        return find_last_not_of(s.data(), pos, s.length());
    }
#endif

    size_type find_last_not_of(const CharT* s, size_type pos = npos) const noexcept
    {
        return find_last_not_of(s, pos, traits_type::length(s));
    }

    size_type find_last_not_of(CharT c, size_type pos = npos) const noexcept
    {
        size_type length = size();

        if (length > 0) {
            const CharT* buf = Buffer();
            if (--length > pos) {
                length = pos;
            }
            do {
                if (traits_type::eq(buf[length], c)) {
                    return length;
                }
            } while (length--);
        }
        return npos;
    }

    // Bind this string to buf. The character array passed to this method
    // should be null-terminated. If it isn't the array returned by data()
    // and c_str() will not be null terminated. This is not standard
    // compliant, but I assume you are smart enough to deal with the
    // consequences.
    void bind(const CharT* buf, size_type len)
    {
        if (len > max_size()) {
            throw ::std::length_error("SuperString::bind");
        }

        if (data_.ext.flags == F_LONG) {
            data_.deallocate(data_.GetPtr(), data_.ext.capacity+1);
        }

        data_.SetPtr(const_cast<CharT*>(buf));
        data_.ext.capacity = 0;
        data_.ext.flags = F_BOUND;
        data_.ext.longLength = len;
    }

    void bind(const CharT* buf)
    {
        bind(buf, traits_type::length(buf));
    }

    // Returns true if this string is currently bound.
    bool is_bound() const noexcept
    {
        return data_.ext.flags == F_BOUND;
    }

    // Ubind the string by copying the contents of the bound string into
    // internal storage.
    void unbind()
    {
        CopyIfBound();
    }

#ifdef SUPER_STRING_STD_STRING_COMPAT
    // Convert this string to a std::basic_string
    ::std::basic_string<CharT> to_std() const
    {
        return ::std::basic_string<CharT>(data(), size());
    }
#endif

protected:
    enum Flags {
        F_SHORT = 0,
        F_LONG = 1,
        F_BOUND = 2
    };

    struct Data : public CharTAlloc
    {
        Data()
            : CharTAlloc()
        {
            buf[0] = CharT();
            ext.flags = F_SHORT;
            ext.shortLength = 0;
        }

        Data(const CharTAlloc& alloc)
            : CharTAlloc(alloc)
        {
            buf[0] = CharT();
            ext.flags = F_SHORT;
            ext.shortLength = 0;
        }

        void SetPtr(CharT* p)
        {
            ext.ptr = reinterpret_cast<uintptr_t>(p);
        }

        CharT* GetPtr()
        {
            return reinterpret_cast<CharT*>(ext.ptr);
        }

        const CharT* GetPtr() const
        {
            return reinterpret_cast<const CharT*>(ext.ptr);
        }

        union {
            struct {
                // Number of bytes available not including '\0'
                uint64_t longLength;
                uint64_t capacity;
                uint64_t ptr:48;
                uint64_t flags:8;
                uint64_t shortLength:8;
            } ext;
            CharT buf[24];
        };
    };

    const CharT* Buffer() const
    {
        if (data_.ext.flags == F_SHORT) {
            return data_.buf;
        } else {
            return data_.GetPtr();
        }
        return NULL;
    }

    CharT* Buffer()
    {
        if (data_.ext.flags == F_SHORT) {
            return data_.buf;
        } else {
            return data_.GetPtr();
        }
        return NULL;
    }

    void SetLength(size_type len)
    {
        if (data_.ext.flags == F_SHORT) {
            assert(len <= 22);
            // GCC issues an "array subscript is above array bounds" if there
            // is no length check.
            if (len <= 22) {
                data_.ext.shortLength = len;
                data_.buf[len] = CharT();
            }
        } else {
            data_.ext.longLength = len;
            if (data_.ext.flags == F_LONG) {
                data_.GetPtr()[len] = CharT();
            }
        }
    }

    void CopyIfBound()
    {
        if (data_.ext.flags == F_BOUND) {
            if (data_.ext.longLength <= 22) {
                traits_type::copy(data_.buf, data_.GetPtr(), data_.ext.longLength);
                data_.buf[data_.ext.longLength] = CharT();
                data_.ext.flags = F_SHORT;
                data_.ext.shortLength = data_.ext.longLength;
            } else {
                CharT* p = data_.allocate(data_.ext.longLength+1);
                traits_type::copy(p, data_.GetPtr(), data_.ext.longLength);
                p[data_.ext.longLength] = CharT();
                data_.SetPtr(p);
                data_.ext.flags = F_LONG;
                data_.ext.capacity = data_.ext.longLength;
            }
        }
    }

    CharT* Allocate(size_type oldcap, size_type& newcap)
    {
        // The following reserve algo is equivalent to libstdc++.
        const size_type pageSize = 4096;
        const size_type mallocHeaderSize = 4 * sizeof(void*);

        // Increase size exponentially to meet the amortized O(n) time
        // requirements.
        if (newcap < oldcap*2) {
            newcap = oldcap*2;
        }

        // Expand to fill up a full page.
        const size_type totalSize = mallocHeaderSize + newcap + 1;
        if (totalSize > pageSize) {
            newcap += pageSize - (totalSize % pageSize);
        }

        // Don't allocate more than the maximum size allowed.
        const size_type maxSize = max_size();
        if (newcap > maxSize) {
            newcap = maxSize;
        }

        return data_.allocate(newcap+1);
    }

    // Note: If copy is false, a null-terminator will *not* be appended to the
    // end of the newly allocated array. The string is no longer valid so you
    // must call SetLength(...) before returning back to the application.
    void Reserve(size_type newcap, bool copy = true)
    {
        const size_type maxSize = max_size();
        if (newcap > maxSize) {
            throw ::std::length_error("SuperString::reserve");
        }

        if (data_.ext.flags == F_SHORT) {
            if (newcap > 22 || isMemoryPreAllocated()) {
                // Reserve more space and switch to dynamic allocation
                CharT* p = data_.allocate(newcap+1);
                if (copy) {
                    traits_type::copy(p, data_.buf, data_.ext.shortLength+1);
                    data_.ext.longLength = data_.ext.shortLength;
                }
                data_.SetPtr(p);
                data_.ext.flags = F_LONG;
                data_.ext.capacity = newcap;
            }
        } else if (data_.ext.flags == F_BOUND) {
            newcap = ::std::max((size_type)data_.ext.longLength, newcap);
            if (newcap > 22) { // Convert to F_LONG
                CharT* p = Allocate(0, newcap);
                if (copy) {
                    traits_type::copy(p, data_.GetPtr(), data_.ext.longLength);
                    // Don't trust bound strings to be null-terminated.
                    p[data_.ext.longLength] = CharT();
                }
                data_.SetPtr(p);
                data_.ext.flags = F_LONG;
                data_.ext.capacity = newcap;
            } else { // Convert to F_SHORT
                if (copy) {
                    traits_type::copy(data_.buf, data_.GetPtr(), data_.ext.longLength);
                    // Don't trust bound strings to be null-terminated.
                    data_.buf[data_.ext.longLength] = 0;
                    data_.ext.shortLength = data_.ext.longLength;
                }
                data_.ext.flags = F_SHORT;
            }
        } else { // F_LONG
            const size_type oldcap = data_.ext.capacity;
            if (newcap > oldcap) {
                CharT* p = Allocate(oldcap, newcap);
                CharT* old = data_.GetPtr();
                if (copy) {
                    traits_type::copy(p, old, data_.ext.longLength+1);
                }
                data_.SetPtr(p);
                data_.ext.capacity = newcap;
                data_.deallocate(old, oldcap+1);
            }
        }
    }

    Data data_;
private:
    bool isMemoryPreAllocated(){return false;}
};

template<>
inline bool
SuperString<char, ::std::char_traits<char>, InPlaceAllocator<char>>:: isMemoryPreAllocated()
{
    return data_.isMemoryPreAllocated();
}

template<>
inline bool
SuperString<char, ::std::char_traits<char>, InPlaceRefAllocator<char>>::isMemoryPreAllocated()
{
    return data_.isMemoryPreAllocated();
}

// operator ==
template<typename CharT, typename Traits, typename Alloc>
inline bool operator==(const SuperString<CharT, Traits, Alloc>& s1,
                       const SuperString<CharT, Traits, Alloc>& s2)
{
    return s1.compare(s2) == 0;
}

template<typename CharT, typename Traits, typename Alloc>
inline bool operator==(const char* s1, const SuperString<CharT, Traits, Alloc>& s2)
{
    return s2.compare(s1) == 0;
}

template<typename CharT, typename Traits, typename Alloc>
inline bool operator==(const SuperString<CharT, Traits, Alloc>& s1, const char* s2)
{
    return s1.compare(s2) == 0;
}

// operator <
template<typename CharT, typename Traits, typename Alloc>
inline bool operator<(const SuperString<CharT, Traits, Alloc>& s1,
                      const SuperString<CharT, Traits, Alloc>& s2)
{
    return s1.compare(s2) < 0;
}

template<typename CharT, typename Traits, typename Alloc>
inline bool operator<(const char* s1, const SuperString<CharT, Traits, Alloc>& s2)
{
    return -s2.compare(s1) < 0;
}

template<typename CharT, typename Traits, typename Alloc>
inline bool operator<(const SuperString<CharT, Traits, Alloc>& s1, const char* s2)
{
    return s1.compare(s2) < 0;
}

template<typename CharT, typename Traits, typename Alloc>
inline ::std::basic_ostream<CharT, Traits>&
operator<<(::std::basic_ostream<CharT, Traits>& stream,
           const SuperString<CharT, Traits, Alloc>& s)
{
    stream.write(s.data(), s.length());
    return stream;
}

template<typename CharT, typename Traits, typename Alloc>
inline ::std::basic_istream<CharT, Traits>&
operator>>(::std::basic_istream<CharT, Traits>& in,
           SuperString<CharT, Traits, Alloc>& str)
{
    typedef ::std::basic_istream<CharT, Traits> istream_type;
    typedef typename istream_type::ios_base     ios_base;
    typedef SuperString<CharT, Traits, Alloc>   string_type;
    typedef typename istream_type::int_type     int_type;
    typedef typename string_type::size_type     size_type;
    typedef ::std::ctype<CharT>                 ctype_type;
    typedef typename ctype_type::ctype_base     ctype_base;

    size_type extracted = 0;
    typename ios_base::iostate err = ios_base::goodbit;
    typename istream_type::sentry cerb(in, false);
    if (cerb)
    {
        try {
            // Avoid reallocation for common case.
            str.erase();
            CharT buf[128];
            size_type len = 0;
            const ::std::streamsize w = in.width();
            const size_type n = w > 0 ? static_cast<size_type>(w) : str.max_size();
            const ctype_type& ct = ::std::use_facet<ctype_type>(in.getloc());
            const int_type eof = Traits::eof();
            int_type c = in.rdbuf()->sgetc();

            while (extracted < n
               && !Traits::eq_int_type(c, eof)
               && !ct.is(ctype_base::space, Traits::to_char_type(c)))
            {
                if (len == sizeof(buf) / sizeof(CharT)) {
                    str.append(buf, sizeof(buf) / sizeof(CharT));
                    len = 0;
                }
                buf[len++] = Traits::to_char_type(c);
                ++extracted;
                c = in.rdbuf()->snextc();
            }
            str.append(buf, len);

            if (Traits::eq_int_type(c, eof)) {
                err |= ios_base::eofbit;
            }
            in.width(0);
        } catch(...) {
            // _GLIBCXX_RESOLVE_LIB_DEFECTS
            // 91. Description of operator>> and getline() for string<>
            // might cause endless loop
            in._M_setstate(ios_base::badbit);
        }
    }

    // 211.  operator>>(istream&, string&) doesn't set failbit
    if (!extracted) {
        err |= ios_base::failbit;
    }
    if (err) {
        in.setstate(err);
    }
    return in;
}

template<typename CharT, typename Traits, typename Alloc>
inline ::std::basic_istream<CharT, Traits>&
getline(::std::basic_istream<CharT, Traits>& in,
        SuperString<CharT, Traits, Alloc>& str,
        CharT delim)
{
    typedef ::std::basic_istream<CharT, Traits> istream_type;
    typedef typename istream_type::ios_base     ios_base;
    typedef SuperString<CharT, Traits, Alloc>   string_type;
    typedef typename istream_type::int_type     int_type;
    typedef typename string_type::size_type     size_type;

    size_type extracted = 0;
    const size_type n = str.max_size();
    typename ios_base::iostate err = ios_base::goodbit;
    typename istream_type::sentry cerb(in, true);
    if (cerb)
    {
        try
        {
            // Avoid reallocation for common case.
            str.erase();
            CharT buf[128];
            size_type len = 0;
            const int_type idelim = Traits::to_int_type(delim);
            const int_type eof = Traits::eof();
            int_type c = in.rdbuf()->sgetc();

            while (extracted < n
                    && !Traits::eq_int_type(c, eof)
                    && !Traits::eq_int_type(c, idelim))
            {
                if (len == sizeof(buf) / sizeof(CharT))
                {
                    str.append(buf, sizeof(buf) / sizeof(CharT));
                    len = 0;
                }
                buf[len++] = Traits::to_char_type(c);
                ++extracted;
                c = in.rdbuf()->snextc();
            }
            str.append(buf, len);

            if (Traits::eq_int_type(c, eof)) {
                err |= ios_base::eofbit;
            } else if (Traits::eq_int_type(c, idelim)) {
                ++extracted;
                in.rdbuf()->sbumpc();
            } else {
                err |= ios_base::failbit;
            }
        } catch(...) {
            // _GLIBCXX_RESOLVE_LIB_DEFECTS
            // 91. Description of operator>> and getline() for string<>
            // might cause endless loop
            in._M_setstate(ios_base::badbit);
        }
    }
    if (!extracted) {
        err |= ios_base::failbit;
    }
    if (err) {
        in.setstate(err);
    }
    return in;
}

template<typename CharT, typename Traits, typename Alloc>
inline ::std::basic_istream<CharT, Traits>&
getline(::std::basic_istream<CharT, Traits>& in,
        SuperString<CharT, Traits, Alloc>& str)
{
    return getline(in, str, '\n');
}

// operator +
template<typename CharT, typename Traits, typename Alloc>
inline SuperString<CharT, Traits, Alloc> operator+(
        const SuperString<CharT, Traits, Alloc>& s1,
        const SuperString<CharT, Traits, Alloc>& s2)
{
    SuperString<CharT, Traits, Alloc> out(s1);
    out.append(s2);
    return out;
}

template<typename CharT, typename Traits, typename Alloc>
inline SuperString<CharT, Traits, Alloc> operator+(
        const SuperString<CharT, Traits, Alloc>& s1, const CharT* s2)
{
    SuperString<CharT, Traits, Alloc> out(s1);
    out.append(s2);
    return out;
}

template<typename CharT, typename Traits, typename Alloc>
inline SuperString<CharT, Traits, Alloc> operator+(
        const SuperString<CharT, Traits, Alloc>& s1, const CharT c)
{
    SuperString<CharT, Traits, Alloc> out(s1);
    out.append(1, c);
    return out;
}

template<typename CharT, typename Traits, typename Alloc>
inline SuperString<CharT, Traits, Alloc> operator+(
        const CharT* s1, const SuperString<CharT, Traits, Alloc>& s2)
{
    SuperString<CharT, Traits, Alloc> out(s1);
    out.append(s2);
    return out;
}

template<typename CharT, typename Traits, typename Alloc>
inline SuperString<CharT, Traits, Alloc> operator+(const CharT c,
        const SuperString<CharT, Traits, Alloc>& s2)
{
    SuperString<CharT, Traits, Alloc> out(1, c);
    out.append(s2);
    return out;
}

#ifdef SUPER_STRING_STD_STRING_COMPAT
// operator ==
template<typename CharT, typename Traits, typename Alloc>
inline bool operator==(const SuperString<CharT, Traits, Alloc>& s1,
                       const ::std::basic_string<CharT, Traits, Alloc>& s2)
{
    return s1.compare(s2) == 0;
}

template<typename CharT, typename Traits, typename Alloc>
inline bool operator==(const ::std::basic_string<CharT, Traits, Alloc>& s1,
                       const SuperString<CharT, Traits, Alloc>& s2)
{
    return s1.compare(0, s1.length(), s2.data(), s2.length()) == 0;
}

// operator <
template<typename CharT, typename Traits, typename Alloc>
inline bool operator<(const SuperString<CharT, Traits, Alloc>& s1,
                      const ::std::basic_string<CharT, Traits, Alloc>& s2)
{
    return s1.compare(s2) < 0;
}

template<typename CharT, typename Traits, typename Alloc>
inline bool operator<(const ::std::basic_string<CharT, Traits, Alloc>& s1,
                      const SuperString<CharT, Traits, Alloc>& s2)
{
    return s1.compare(0, s1.length(), s2.data(), s2.length()) < 0;
}

// operator !=
template<typename CharT, typename Traits, typename Alloc>
inline bool operator!=(const SuperString<CharT, Traits, Alloc>& s1,
                       const ::std::basic_string<CharT, Traits, Alloc>& s2)
{
    return !(s1 == s2);
}

template<typename CharT, typename Traits, typename Alloc>
inline bool operator!=(const ::std::basic_string<CharT, Traits, Alloc>& s1,
                       const SuperString<CharT, Traits, Alloc>& s2)
{
    return !(s1 == s2);
}

// operator >
template<typename CharT, typename Traits, typename Alloc>
inline bool operator>(const SuperString<CharT, Traits, Alloc>& s1,
                      const ::std::basic_string<CharT, Traits, Alloc>& s2)
{
    return s2 < s1;
}

template<typename CharT, typename Traits, typename Alloc>
inline bool operator>(const ::std::basic_string<CharT, Traits, Alloc>& s1,
                      const SuperString<CharT, Traits, Alloc>& s2)
{
    return s2 < s1;
}

// operator <=
template<typename CharT, typename Traits, typename Alloc>
inline bool operator<=(const SuperString<CharT, Traits, Alloc>& s1,
                       const ::std::basic_string<CharT, Traits, Alloc>& s2)
{
    return !(s2 < s1);
}

template<typename CharT, typename Traits, typename Alloc>
inline bool operator<=(const ::std::basic_string<CharT, Traits, Alloc>& s1,
                       const SuperString<CharT, Traits, Alloc>& s2)
{
    return !(s2 < s1);
}

// operator >=
template<typename CharT, typename Traits, typename Alloc>
inline bool operator>=(const SuperString<CharT, Traits, Alloc>& s1,
                       const ::std::basic_string<CharT, Traits, Alloc>& s2)
{
    return !(s1 < s2);
}

template<typename CharT, typename Traits, typename Alloc>
inline bool operator>=(const ::std::basic_string<CharT, Traits, Alloc>& s1,
                       const SuperString<CharT, Traits, Alloc>& s2)
{
    return !(s1 < s2);
}

// operator +
template<typename CharT, typename Traits, typename Alloc>
inline SuperString<CharT, Traits, Alloc> operator+(
        const SuperString<CharT, Traits, Alloc>& s1,
        const ::std::basic_string<CharT, Traits, Alloc>& s2)
{
    SuperString<CharT, Traits, Alloc> out(s1);
    out.append(s2);
    return out;
}

template<typename CharT, typename Traits, typename Alloc>
inline ::std::basic_string<CharT, Traits, Alloc> operator+(
        const ::std::basic_string<CharT, Traits, Alloc>& s1,
        const SuperString<CharT, Traits, Alloc>& s2)
{
    ::std::basic_string<CharT, Traits, Alloc> out(s1);
    out.append(s2.data(), s2.length());
    return out;
}
#endif

// operator !=
template<typename CharT, typename Traits, typename Alloc>
inline bool operator!=(const SuperString<CharT, Traits, Alloc>& s1,
                       const SuperString<CharT, Traits, Alloc>& s2)
{
    return !(s1 == s2);
}

template<typename CharT, typename Traits, typename Alloc>
inline bool operator!=(const char* s1, const SuperString<CharT, Traits, Alloc>& s2)
{
    return !(s1 == s2);
}

template<typename CharT, typename Traits, typename Alloc>
inline bool operator!=(const SuperString<CharT, Traits, Alloc>& s1, const char* s2)
{
    return !(s1 == s2);
}

// operator >
template<typename CharT, typename Traits, typename Alloc>
inline bool operator>(const SuperString<CharT, Traits, Alloc>& s1,
                      const SuperString<CharT, Traits, Alloc>& s2)
{
    return s2 < s1;
}

template<typename CharT, typename Traits, typename Alloc>
inline bool operator>(const char* s1, const SuperString<CharT, Traits, Alloc>& s2)
{
    return s2 < s1;
}

template<typename CharT, typename Traits, typename Alloc>
inline bool operator>(const SuperString<CharT, Traits, Alloc>& s1, const char* s2)
{
    return s2 < s1;
}

// operator <=
template<typename CharT, typename Traits, typename Alloc>
inline bool operator<=(const SuperString<CharT, Traits, Alloc>& s1,
                       const SuperString<CharT, Traits, Alloc>& s2)
{
    return !(s2 < s1);
}

template<typename CharT, typename Traits, typename Alloc>
inline bool operator<=(const char* s1, const SuperString<CharT, Traits, Alloc>& s2)
{
    return !(s2 < s1);
}

template<typename CharT, typename Traits, typename Alloc>
inline bool operator<=(const SuperString<CharT, Traits, Alloc>& s1, const char* s2)
{
    return !(s2 < s1);
}

// operator >=
template<typename CharT, typename Traits, typename Alloc>
inline bool operator>=(const SuperString<CharT, Traits, Alloc>& s1,
                       const SuperString<CharT, Traits, Alloc>& s2)
{
    return !(s1 < s2);
}

template<typename CharT, typename Traits, typename Alloc>
inline bool operator>=(const char* s1, const SuperString<CharT, Traits, Alloc>& s2)
{
    return !(s1 < s2);
}

template<typename CharT, typename Traits, typename Alloc>
inline bool operator>=(const SuperString<CharT, Traits, Alloc>& s1, const char* s2)
{
    return !(s1 < s2);
}

inline long
stol(const String& str, size_t* = 0, int base = 10) { return strtol(str.c_str(), 0, base); }

inline unsigned long
stoul(const String& str, size_t* = 0, int base = 10) { return strtoul(str.c_str(), 0, base); }

inline long long
stoll(const String& str, size_t* = 0, int base = 10) { return strtoll(str.c_str(), 0, base); }

inline unsigned long long
stoull(const String& str, size_t* = 0, int base = 10) { return strtoull(str.c_str(), 0, base); }

inline float
stof(const String& str, size_t* = 0) { return strtof(str.c_str(), 0); }

inline double
stod(const String& str, size_t* = 0) { return strtod(str.c_str(), 0); }

inline long double
stold(const String& str, size_t* = 0) { return strtold(str.c_str(), 0); }

#define TTSTRING_INT_TO_STRING(val, format)                         \
    static constexpr size_t sz = (4u * sizeof(decltype(val))) + 1;  \
    char buffer[sz];                                                \
    snprintf(buffer, sz, format, val);                              \
    return buffer;

inline String
to_string(int val) { TTSTRING_INT_TO_STRING(val, "%d") }

inline String
to_string(unsigned val) { TTSTRING_INT_TO_STRING(val, "%u") }

inline String
to_string(long val) { TTSTRING_INT_TO_STRING(val, "%ld") }

inline String
to_string(unsigned long val) { TTSTRING_INT_TO_STRING(val, "%lu") }

inline String
to_string(long long val) { TTSTRING_INT_TO_STRING(val, "%lld") }

inline String
to_string(unsigned long long val) { TTSTRING_INT_TO_STRING(val, "%llu") }

#undef TTSTRING_INT_TO_STRING

#define TTSTRING_REAL_TO_STRING(val, format)                                                \
    static constexpr size_t sz = std::numeric_limits<decltype(val)>::max_digits10 +         \
                                 std::numeric_limits<decltype(val)>::max_exponent10 + 2;    \
    char buffer[sz];                                                                        \
    snprintf(buffer, sz, format, val);                                                      \
    return buffer;

inline String
to_string(float val) { TTSTRING_REAL_TO_STRING(val, "%f") }

inline String
to_string(double val) { TTSTRING_REAL_TO_STRING(val, "%f") }

inline String
to_string(long double val) { TTSTRING_REAL_TO_STRING(val, "%Lf") }

#undef TTSTRING_REAL_TO_STRING


inline namespace literals {
inline namespace string_literals {

inline SuperString<char> operator"" _s(const char* str, size_t len)
{
    return SuperString<char>(str, len);
}

} // inline namespace string_literals
} // inline namespace literals

} // namespace tt

namespace std {
    template<>
    struct hash<tt::SuperString<char>>
    {
        size_t operator()(const tt::SuperString<char>& s) const
        {
            return tt::fnv_hash(s.data(), s.length());
        }
    };
}

#endif // SUPER_STRING_H_
