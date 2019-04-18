/***************************************************************************
 *
 *                  Unpublished Work Copyright (c) 2014
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

#include <ttstl/stringbuf.h>
#include <ttstl/string.h>
namespace tt {

template<
    class CharT,
    class Traits = std::char_traits<CharT>,
    class Allocator = std::allocator<CharT>
>
class SuperStringStream : public std::basic_iostream<CharT, Traits>
{
public:
    using char_type = CharT;
    using traits_type = Traits;
    using int_type = typename Traits::int_type;
    using pos_type = typename Traits::pos_type;
    using off_type = typename Traits::off_type;
    using allocator_type = Allocator;

    using buf_type = SuperStringBuf<CharT, Traits, Allocator>;

    SuperStringStream()
        : std::basic_iostream<CharT, Traits>(&m_buf)
    {}

    SuperStringStream(const typename buf_type::str_type& str)
        : m_buf(str)
        , std::basic_iostream<CharT, Traits>(&m_buf)
    {}

    ~SuperStringStream() = default;
    SuperStringStream(const SuperStringStream&) = default;
    SuperStringStream(SuperStringStream&&) = default;
    SuperStringStream& operator=(const SuperStringStream&) = default;
    SuperStringStream& operator=(SuperStringStream&&) = default;

    typename buf_type::str_type str() const
    {
        return m_buf.str();
    }

    void str(const typename buf_type::str_type & s)
    {
        m_buf.str(s);
    }

    buf_type* rdbuf() const
    {
        return &m_buf;
    }

    // Forwards to the underlying SuperStringBuf object.
    const typename buf_type::str_type& Finish()
    {
        return m_buf.Finish();
    }

    void Reset(typename buf_type::str_type::size_type size = buf_type::str_type::sso_length)
    {
        m_buf.Reset(size);
    }

private:
    buf_type m_buf;
};

using StringStream = SuperStringStream<char>;
using InPlaceStringStream = SuperStringStream<char, ::std::char_traits<char>, InPlaceAllocator<char>>;
using InPlaceRefStringStream = SuperStringStream<char, ::std::char_traits<char>, InPlaceRefAllocator<char>>;
} // namespace tt

