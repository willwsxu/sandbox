/***************************************************************************
*
*                    Unpublished Work Copyright (c) 2013
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

#include <functional>
#include <boost/functional/hash.hpp>
#include <boost/uuid/uuid.hpp>

#include <ttstl/likely.h>

namespace std {

template<>
struct hash<boost::uuids::uuid>
{
    size_t operator()(const boost::uuids::uuid& u) const
    {
        // If the uuid is random (version 4), use the random bits directly as
        // the hash, but fall back to a proper hash function if not. Luckily,
        // the reserved bits of the uuid don't overlap so a simple XOR will
        // work.
        if (LIKELY(u.version() == boost::uuids::uuid::version_random_number_based)) {
            const char* data = (const char*)&u.data[0];
            return *(const uint64_t*)&data[0] ^ *(const uint64_t*)&data[8];
        } else {
            return boost::hash<boost::uuids::uuid>()(u);
        }
    }
};

} // namespace std
