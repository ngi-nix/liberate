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
#include <liberate/sys/error.h>

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
    std::string const & hostname, uint16_t port)
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
    case EAI_SERVICE:
#  if defined(LIBERATE_HAVE_EAI_NODATA)
    case EAI_NODATA:
#  endif
#  if defined(LIBERATE_HAVE_EAI_ADDRFAMILY)
    case EAI_ADDRFAMILY:
#  endif
#  if defined(LIBERATE_WIN32)
    case WSANO_DATA:
#  endif
      return;

    case EAI_FAMILY:
    case EAI_SOCKTYPE:
    case EAI_BADFLAGS:
      throw std::invalid_argument(gai_strerror(err));

    case EAI_AGAIN:
    case EAI_FAIL:
    case EAI_MEMORY:
      throw std::runtime_error(gai_strerror(err));
#  if defined(LIBERATE_HAVE_EAI_SYSTEM)
    case EAI_SYSTEM:
      throw std::runtime_error(sys::error_message(sys::error_code()));
#  endif

#  if defined(LIBERATE_WIN32)
    case WSANOTINITIALISED:
      throw std::logic_error("WSA not initialized!");
#  endif

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
    auto addr = socket_address{cur->ai_addr, static_cast<size_t>(cur->ai_addrlen)};
    addr.set_port(port);
    results.insert(addr);
  }
#else
  throw std::domain_error("Not implemented on this platform.");
#endif
}

} // anonymous namespace


// Note: the api instance is unused, but we need an instance for WSASetup() to
//       have occurred.
std::set<socket_address>
resolve(api &, address_type type, std::string const & hostname)
{
  // Validate input & set hints
  if (hostname.empty()) {
    throw std::invalid_argument("Need to provide a hostname.");
  }

  auto lookup_host = hostname;
  auto sep = lookup_host.find(":");
  uint16_t port = 0;
  if (sep != std::string::npos) {
    lookup_host = hostname.substr(0, sep);
    auto port_str = hostname.substr(sep + 1);
    port = std::stoi(port_str);
  }

  std::set<socket_address> results;
  switch (type) {
    case AT_UNSPEC:
      resolve_internal(results, AF_INET, lookup_host, port);
      resolve_internal(results, AF_INET6, lookup_host, port);
      break;

    case AT_INET4:
      resolve_internal(results, AF_INET, lookup_host, port);
      break;

    case AT_INET6:
      resolve_internal(results, AF_INET6, lookup_host, port);
      break;

    default:
      throw std::invalid_argument("Unsupported address type specified.");
  }

  return results;
}


} // namespace liberate::net


