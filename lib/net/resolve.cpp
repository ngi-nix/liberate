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
#include <build-config.h>

#include "netincludes.h"

#if defined(LIBERATE_HAVE_GETADDRINFO)
#  include <sys/types.h>
#  include <sys/socket.h>
#  include <netdb.h>
#  define GETADDRINFO_IS_IMPLEMENTED
#endif

#if defined(LIBERATE_HAVE_WS2TCPIP_H)
#  define GETADDRINFO_IS_IMPLEMENTED
#endif

#include <liberate/net/resolve.h>

#include "../macros.h"

namespace liberate::net {

namespace {

// Helper struct for clearing up addrinfo
struct addrinfo_holder
{
  addrinfo * info = nullptr;

  ~addrinfo_holder()
  {
    if (info) {
      freeaddrinfo(info);
      info = nullptr;
    }
  }
};



void
resolve_internal(std::set<socket_address> & results, int family,
    std::string const & hostname)
{
#if defined(GETADDRINFO_IS_IMPLEMENTED)
  // Construct hints
  struct addrinfo hints = {};
  hints.ai_family = family;
  hints.ai_socktype = 0;
  hints.ai_protocol = 0;
  hints.ai_flags = 0;
#  if defined(HAVE_AI_IDN)
  hints.ai_flags |= AI_IDN;
#  endif

  // Call getaddrinfo()
  addrinfo_holder holder;
  int err = getaddrinfo(hostname.c_str(), nullptr, &hints, &holder.info);
  switch (err) {
    case 0: // All good
      break;

    case EAI_NONAME: // Just no results
#  if defined(LIBERATE_HAVE_EAI_NODATA)
    case EAI_NODATA:
#  endif
#  if defined(LIBERATE_HAVE_EAI_ADDRFAMILY)
    case EAI_ADDRFAMILY:
#  endif
    case EAI_SERVICE:
      return;

    case EAI_FAMILY:
    case EAI_SOCKTYPE:
    case EAI_BADFLAGS:
      throw std::invalid_argument(gai_strerror(err));

    case EAI_AGAIN:
    case EAI_FAIL:
    case EAI_MEMORY:
#if  defined(LIBERATE_HAVE_EAI_SYSTEM)
    case EAI_SYSTEM:
#endif
      throw std::runtime_error(gai_strerror(err));

    default:
      throw std::logic_error("Unknown error encountered.");
      break;
  }

  // Parse results
  for (auto cur = holder.info ; cur != nullptr ; cur = cur->ai_next) {
    switch (cur->ai_family) {
      case AF_INET:
      case AF_INET6:
        break;

      default:
        throw std::range_error("Unexpected address type returned!");
    }

    // Parse the address and add it to our list
    auto addr = socket_address{cur->ai_addr, cur->ai_addrlen};
    results.insert(addr);
  }
#else
  throw std::domain_error("Not implemented on this platform.");
#endif
}

} // anonymous namespace


std::set<socket_address>
resolve(address_type type, std::string const & hostname)
{
  // Validate input & set hints
  if (hostname.empty()) {
    throw std::invalid_argument("Need to provide a hostname.");
  }

  std::set<socket_address> results;
  switch (type) {
    case AT_UNSPEC:
      resolve_internal(results, AF_INET, hostname);
      resolve_internal(results, AF_INET6, hostname);
      break;

    case AT_INET4:
      resolve_internal(results, AF_INET, hostname);
      break;

    case AT_INET6:
      resolve_internal(results, AF_INET6, hostname);
      break;

    default:
      throw std::invalid_argument("Unsupported address type specified.");
  }

  return results;
}


} // namespace liberate::net


