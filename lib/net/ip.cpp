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
#include <liberate/net/ip.h>
#include <liberate/net/address_type.h>

#include "../macros.h"

namespace liberate::net {

namespace {

inline bool
parse_address_helper(socket_address * source, socket_address * dest,
    ::liberate::types::byte const * buffer, size_t bufsize)
{
  // Need to read the protocol bits at minimum
  if (bufsize < 1) {
    return false;
  }

  // Read protocol bits
  uint8_t proto = static_cast<uint8_t>(buffer[0] & ::liberate::types::byte(0xf0)) >> 4;

  // IPv4
  if (4 == proto) {
    // Source address
    if (source) {
      // Starts at byte 12 of IPv4 headers, is 4 Bytes
      if (bufsize < 12 + 4) {
        return false;
      }

      // Copy IPv4 address
      *source = socket_address{AT_INET4, buffer + 12, 4};
    }

    // Destination address
    if (dest) {
      // Starts at byte 16 of IPv4 headers, is 4 Bytes
      if (bufsize < 16 + 4) {
        return false;
      }

      // Copy IPv4 address
      *dest = socket_address{AT_INET4, buffer + 16, 4};
    }

    return true;
  }

  // IPv6
  if (6 == proto) {
    // Source address
    if (source) {
      // Starts at byte 8 of IPv6 headers, is 16 Bytes
      if (bufsize < 8 + 16) {
        return false;
      }

      // Copy IPv4 address
      *source = socket_address{AT_INET6, buffer + 8, 16};
    }

    // Destination address
    if (dest) {
      // Starts at byte 24 of IPv6 headers, is 16 Bytes
      if (bufsize < 24 + 16) {
        return false;
      }

      // Copy IPv4 address
      *dest = socket_address{AT_INET6, buffer + 24, 16};
    }

    return true;
  }

  return false;
}

} // anonymous namespace


bool
parse_addresses(socket_address & source, socket_address & dest,
    ::liberate::types::byte const * buffer, size_t bufsize)
{
  return parse_address_helper(&source, &dest, buffer, bufsize);
}


bool
parse_source_address(socket_address & source,
    ::liberate::types::byte const * buffer, size_t bufsize)
{
  return parse_address_helper(&source, nullptr, buffer, bufsize);
}


bool
parse_dest_address(socket_address & dest,
    ::liberate::types::byte const * buffer, size_t bufsize)
{
  return parse_address_helper(nullptr, &dest, buffer, bufsize);
}


} // namespace liberate::net
