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

#include <boost/uuid/uuid.hpp>

#include <ttstl/string.h>

namespace tt {
namespace uuid {

inline String to_string(const boost::uuids::uuid& u)
{
    const char digits[] = "0123456789abcdef";

    String result;
    result.fast_resize(36);

    char* buf = &result[0];

    size_t i(0);
    size_t j(0);
    for (; i<16; ++i) {
        const uint8_t b = u.data[i];
        buf[j++] = digits[(b >> 4) & 0x0F];
        buf[j++] = digits[b & 0x0F];

        if (j == 8 || j == 13 || j == 18 || j == 23) {
            buf[j++] = '-';
        }
    }

    return result;
}

} // uuid
} // namespace tt
