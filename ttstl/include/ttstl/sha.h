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

#include <array>
#include <cstring>
#include <openssl/sha.h>
#include <ttstl/string.h>

namespace tt {

using Sha1Digest = std::array<uint8_t, 20>;

inline Sha1Digest Sha1Hash(const char* data, unsigned long length)
{
    Sha1Digest digest;
    SHA1(reinterpret_cast<const unsigned char*>(data), length, digest.data());
    return digest;
}

inline Sha1Digest Sha1Hash(const char* s)
{
    return Sha1Hash(s, std::strlen(s));
}

inline Sha1Digest Sha1Hash(const String& s)
{
    return Sha1Hash(s.data(), s.length());
}

template <typename T>
inline Sha1Digest Sha1Hash(const T& v)
{
    return Sha1Hash(reinterpret_cast<const char*>(&v), sizeof(T));
}

class Sha1
{
public:
    Sha1()
    {
        SHA1_Init(&m_ctx);
    }

    Sha1& Update(const String& s)
    {
        return Update(s.data(), s.length());
    }

    Sha1& Update(const std::string& s)
    {
        return Update(s.data(), s.length());
    }

    Sha1& Update(const char* data, unsigned long length)
    {
        SHA1_Update(&m_ctx, data, length);
        return *this;
    }

    Sha1& Update(const char* s)
    {
        return Update(s, std::strlen(s));
    }

    template <typename T>
    Sha1& Update(const T& v)
    {
        return Update(reinterpret_cast<const char*>(&v), sizeof(T));
    }

    Sha1Digest Digest()
    {
        Sha1Digest digest;
        SHA1_Final(digest.data(), &m_ctx);
        return digest;
    }

private:
    SHA_CTX m_ctx;
};

} // namespace tt
