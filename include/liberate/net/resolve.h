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
#ifndef LIBERATE_NET_RESOLVE_H
#define LIBERATE_NET_RESOLVE_H

#ifndef __cplusplus
#error You are trying to include a C++ only header file
#endif

#include <liberate.h>

#include <string>
#include <set>

#include <liberate/net/address_type.h>
#include <liberate/net/socket_address.h>

namespace liberate::net {

/**
 * Resolves network host names to IP addresses.
 * - specify AT_UNSPEC if you want both IPv4 and IPv6 addresses.
 * - specify AT_INET4 or AT_INET6 for only IPv4 or only IPv6 addresses
 *   respectively.
 * - any other address type will yield errors.
 *
 * An empty list means the name could not be resolved. Actual errors
 * are reported as exceptions.
 * - std::invalid_argument if arguments were invalid.
 * - std::domain_error if the functionality is not implemented.
 * - std::runtime_error if an error occurred in the underlying system
 *   call.
 * - std::range_error if the underlying system call returns
 *   unexpected information.
 * - std::logic_error for unspecified errors.
 */
LIBERATE_API
std::set<socket_address>
resolve(address_type type, std::string const & hostname);

} // namespace liberate::net

#endif // guard
