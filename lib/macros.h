/**
 * This file is part of liberate.
 *
 * Author(s): Jens Finkhaeuser <jens@finkhaeuser.de>
 *
 * Copyright (c) 2020 Jens Finkhaeuser.
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
#ifndef LIBERATE_MACROS_H
#define LIBERATE_MACROS_H

#ifndef __cplusplus
#error You are trying to include a C++ only header file
#endif

#include <build-config.h>

/**
 * Support for clang/OCLint suppressions
 **/
#if defined(__clang__) and defined(OCLINT_IS_RUNNING)
#  define OCLINT_SUPPRESS(suppression) \
    __attribute__(( \
      annotate("oclint:suppress[" suppression "]") \
    ))
#else
#  define OCLINT_SUPPRESS(annotation)
#endif

#endif // guard
