/**
 * This file is part of liberate.
 *
 * Author(s): Jens Finkhaeuser <jens@finkhaeuser.de>
 *
 * Copyright (c) 2020-2021 Jens Finkhaeuser.
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
#ifndef LIBERATE_SYS_ERROR_H
#define LIBERATE_SYS_ERROR_H

#ifndef __cplusplus
#error You are trying to include a C++ only header file
#endif

#include <liberate.h>

namespace liberate::sys {

/**
 * Platform-independent version of getting the last error code, i.e.
 * returns errno on POSIX and WSAGetLastError() on Win32.
 */
LIBERATE_API
int error_code();


/**
 * Similar, platform-independent version of turning an error code as
 * returned by the above function into an error message. Takes care of
 * UTF-8 encoding, if necessary.
 */
LIBERATE_API
std::string error_message(int code);


} // namespace liberate::sys

#endif // guard
