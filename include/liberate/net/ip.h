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
#ifndef LIBERATE_NET_IP_H
#define LIBERATE_NET_IP_H

#ifndef __cplusplus
#error You are trying to include a C++ only header file
#endif

#include <liberate.h>

#include <liberate/net/socket_address.h>
#include <liberate/types/byte.h>

namespace liberate::net {

/**
 * Given an byte buffer representing an IP packet, parses a socket_address from
 * the buffer. Returns true if an address could be parsed, false otherwise.
 *
 * This function does *not* verify that the packet is in fact a valid IP packet.
 * It merely treats it as such - it distinguishes between IPv4 and IPv6
 * protocols to determine which part of the buffer to read, but that is it.
 *
 * Consequently, if you feed it bad data, it will return a bad socket address.
 */
LIBERATE_API bool
parse_addresses(socket_address & source, socket_address & dest,
    ::liberate::types::byte const * buffer, size_t bufsize);

LIBERATE_API bool
parse_source_address(socket_address & source,
    ::liberate::types::byte const * buffer, size_t bufsize);

LIBERATE_API bool
parse_dest_address(socket_address & dest,
    ::liberate::types::byte const * buffer, size_t bufsize);


} // namespace liberate::net

#endif // guard
