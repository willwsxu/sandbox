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

#include <cstring>

#include <ttstl/string.h>
#include <ttstl/sstream.h>

namespace tt {

template <typename Iter>
String ListToString(Iter first, Iter last, const char* delim)
{
    StringStream ss;

    for (; first != last; ++first) {
        ss << *first << delim;
    }

    String s = ss.str();

    // Remove trailing delimiter
    if (!s.empty()) {
        for (size_t i(0); i<std::strlen(delim); ++i) {
            s.pop_back();
        }
    }

    return s;
}

template <typename Container>
String ListToString(const Container& c, const char* delim)
{
    return ListToString(std::begin(c), std::end(c), delim);
}

} // namespace tt
