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
#ifndef LIBERATE_STRING_UTIL_H
#define LIBERATE_STRING_UTIL_H

#include <liberate.h>

namespace liberate::string {

/**
 * Lower- and uppercase strings.
 */
LIBERATE_API
std::string to_lower(std::string const & other);

LIBERATE_API
std::string to_upper(std::string const & other);


/**
 * Replace needle in haystack with substitute.
 */
LIBERATE_API
std::string replace(std::string const & haystack, std::string const & needle,
    std::string const & substitute, bool first_only = false);


/**
 * Perform case-insensitive search.
 */
LIBERATE_API
ssize_t ifind(std::string const & haystack, std::string const & needle);


} // namespace liberate::string

#endif // guard
