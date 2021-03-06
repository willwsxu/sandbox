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

#include <system_error>
#include <sys/prctl.h>

namespace tt {

// Set a pretty name for the thread which will be displayed in top and
// /proc/self/task/[tid]/comm.
// Name must not be longer than 16 characters.
inline void SetThreadName(const char* name)
{
    if (prctl(PR_SET_NAME, name, 0, 0, 0) != 0) {
        throw std::system_error(errno, std::system_category());
    }
}

} // namespace tt
