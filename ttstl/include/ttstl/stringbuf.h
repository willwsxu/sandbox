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

#include <ttstl/string.h>

namespace tt {

// std::basic_streambuf which writes to a SuperString
template<
    class CharT,
    class Traits = std::char_traits<CharT>,
    class Allocator = std::allocator<CharT>
>
class SuperStringBuf : public std::basic_streambuf<CharT, Traits>
{
public:
    using char_type = CharT;
    using traits_type = Traits;
    using int_type = typename Traits::int_type;
    using pos_type = typename Traits::pos_type;
    using off_type = typename Traits::off_type;

    using str_type = SuperString<CharT, Traits, Allocator>;

    SuperStringBuf(typename str_type::size_type size = str_type::sso_length)
    {
        m_buf.fast_resize(size);
        Setp();
    }

    SuperStringBuf(const str_type& str)
        : m_buf(str)
    {
        Setp();
    }

    ~SuperStringBuf()
    {
        this->setp(nullptr, nullptr);
    }

    str_type str() const
    {
        if (this->pptr() > this->egptr()) {
            return str_type(this->pbase(), this->pptr());
        } else {
            return str_type(this->pbase(), this->egptr());
        }
    }

    void str(const str_type& s)
    {
        m_buf = s;
        Setp();
    }

    // This method must be called after streaming into this object. It will
    // resize the underlying string. Subsequent stream operations on this
    // object are invalid.
    const str_type& Finish()
    {
        m_buf.resize(this->pptr() - this->pbase());
        return m_buf;
    }

    void Reset(typename str_type::size_type size = str_type::sso_length)
    {
        m_buf.fast_resize(size);
        Setp();
    }

protected:
    virtual int_type overflow(int_type c = traits_type::eof())
    {
        if (c != traits_type::eof()) {
            auto len = m_buf.size();
            m_buf.fast_resize(len*2);
            this->setp(&*m_buf.begin(), &*m_buf.end());
            this->pbump(len);
            this->sputc(c);
        }
        return c;
    }

private:
    void Setp()
    {
        this->setp(&*m_buf.begin(), &*m_buf.end());
    }

    str_type m_buf;
};

} // namespace tt
