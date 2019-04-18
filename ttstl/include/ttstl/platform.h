/***************************************************************************
 *
 *                  Unpublished Work Copyright (c) 2010-2014
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
// Author: John Shaw (jshaw)
#pragma once

// Detect OS
#if defined(_WIN32)
  #define OS_WIN
#elif defined(__linux__)
  #define OS_LINUX
#elif defined(__CYGWIN__)
  #define OS_CYGWIN
#else
  #error Unsupported operating system
#endif

// Detect architecture
#if defined(_M_IX86) || defined(__i386__)
  #if !defined(ARCH_X86)
    #define ARCH_X86
  #endif
#elif defined(_M_AMD64) || defined(_M_X64) || defined(__x86_64__)
  #if !defined(ARCH_X86_64)
    #define ARCH_X86_64
  #endif
#else
  #error Unsupported architecture
#endif

// Sanity check. At the time of writing, the current build system sets the
// architecture macro instead of having this file auto-detect the correct
// architecture. This should return an error if the build system is wrong.
#if defined(ARCH_X86) && defined(ARCH_X86_64)
  #error Multiple architectures defined
#endif

// Detect compiler
#if defined(_MSC_VER)
  #define COMP_MSVC
#elif defined(__GNUC__)
  #define COMP_GCC
#elif defined(__clang__)
  #define COMP_CLANG
#else
  #error Unsupported compiler
#endif

// TODO: Auto-detect this value
// Cache size can be queried using getconf
//   getconf LEVEL1_DCACHE_LINESIZE
#define TT_CACHE_LINE_SIZE 64
