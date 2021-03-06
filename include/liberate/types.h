/**
 * This file is part of liberate.
 *
 * Author(s): Jens Finkhaeuser <jens@finkhaeuser.de>
 *
 * Copyright (c) 2011 Jens Finkhaeuser.
 * Copyright (c) 2012-2014 Unwesen Ltd.
 * Copyright (c) 2015-2021 Jens Finkhaeuser.
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
#ifndef LIBERATE_TYPES_H
#define LIBERATE_TYPES_H

#ifndef __cplusplus
#error You are trying to include a C++ only header file
#endif

#include <liberate.h>

/**
 * We assume C++17, so these includes must work.
 **/
#include <cstdint>
#include <cinttypes>
#include <climits>

#if defined(LIBERATE_WIN32)
#  include <iso646.h>
#  include <BaseTsd.h>
using ssize_t = SSIZE_T;
#endif

/**
 * Some slightly older C++17 compilers don't have std::byte
 */
#include <liberate/types/byte.h>

#endif // guard
