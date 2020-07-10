/**
 * This file is part of liberate.
 *
 * Author(s): Jens Finkhaeuser <jens@finkhaeuser.de>
 *
 * Copyright (c) 2019-2020 Jens Finkhaeuser.
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
#ifndef LIBERATE_STRING_UTF8_H
#define LIBERATE_STRING_UTF8_H

#include <liberate.h>

namespace liberate::string {

#if defined(LIBERATE_WIN32)

/**
 * Convert from Windows UCS-2 to UTF-8 and back.
 */
LIBERATE_API
std::string to_utf8(TCHAR const * source);

LIBERATE_API
std::wstring from_utf8(char const * source);

#endif

} // namespace liberate::string

#endif // guard
