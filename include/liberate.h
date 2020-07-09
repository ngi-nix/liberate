/**
 * This file is part of liberate.
 *
 * Author(s): Jens Finkhaeuser <jens@finkhaeuser.de>
 *
 * Copyright (c) 2011 Jens Finkhaeuser.
 * Copyright (c) 2012-2014 Unwesen Ltd.
 * Copyright (c) 2015-2020 Jens Finkhaeuser.
 *
 * This software is licensed under the terms of the GNU GPLv3 for personal,
 * educational and non-profit use. For all other uses, alternative license
 * options are available. Please contact the copyright holder for additional
 * information, stating your intended usage.
 *
 * You can find the full text of the GPLv3 in the COPYING file in this code
 * distribution.
 *
 * This software is distributed on an "AS IS" BASIS, WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
 * PARTICULAR PURPOSE.
 **/
#ifndef LIBERATE_LIBERATE_H
#define LIBERATE_LIBERATE_H

#ifndef __cplusplus
#error You are trying to include a C++ only header file
#endif

/**
 * Which platform are we on?
 **/
#if !defined(LIBERATE_PLATFORM_DEFINED)
#  if defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__)
#    define LIBERATE_WIN32
#  else
#    define LIBERATE_POSIX
#  endif
#  define LIBERATE_PLATFORM_DEFINED
#endif

// Visibility macros are used by all, so they must come first.
#include <liberate/visibility.h>
#include <liberate/version.h>

/**
 * Decide what to include globally
 **/
#if defined(LIBERATE_WIN32)
// Include windows.h with minimal definitions
#  ifndef WIN32_LEAN_AND_MEAN
#    define WIN32_LEAN_AND_MEAN
#    define __UNDEF_LEAN_AND_MEAN
#  endif
#  define NOMINMAX
// Unicode builds
#  define UNICODE
#  define _UNICODE
#  include <windows.h>
#  include <WinDef.h>
#  ifdef __UNDEF_LEAN_AND_MEAN
#    undef WIN32_LEAN_AND_MEAN
#    undef __UNDEF_LEAN_AND_MEAN
#  endif
// Link against winsock2 FIXME
#  pragma comment(lib, "Ws2_32.lib")
#endif

#endif // guard
