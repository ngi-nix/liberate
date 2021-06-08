/**
 * This file is part of liberate.
 *
 * Author(s): Jens Finkhaeuser <jens@finkhaeuser.de>
 *
 * Copyright (c) 2014 Unwesen Ltd.
 * Copyright (c) 2015-2020 Jens Finkhaeuser.
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

#include <liberate/net/socket_address.h>
#include <liberate/serialization/integer.h>

#include <cstring>
#include <sstream>
#include <algorithm>

#include <liberate/cpp/hash.h>
#include <liberate/fs/path.h>

#include "cidr.h"


namespace liberate::net {
/*****************************************************************************
 * Helper functions
 **/
namespace {

template <typename T>
char const * unwrap(T const & source)
{
  return source;
}

template<>
char const * unwrap<std::string>(std::string const & source)
{
  return source.c_str();
}


template <typename T>
void
parse_address(detail::address_data & data, T const & source, size_t size, uint16_t port)
{
  // Need to zero data.
  ::memset(&data.sa_storage, 0, sizeof(data));

  // Try parsing as a CIDR address.
  detail::parse_result_t result(data);
  try {
    detail::parse_extended_cidr(source, true, result, port);
    return;
  } catch (std::invalid_argument const & ex) {
    // Try with the below code.
  }

#if defined(LIBERATE_HAVE_SOCKADDR_UN)
  // If parsing got aborted, we either have AF_UNIX or AF_UNSPEC as the
  // actual socket type.
#if defined(LIBERATE_WIN32)
  std::string converted = fs::to_win32_path(source);
  auto unwrapped = converted.c_str();
#else // LIBERATE_WIN32
  auto unwrapped = unwrap(source);
#endif // LIBERATE_WIN32
  ::memset(&data.sa_storage, 0, sizeof(data));
  data.sa_un.sun_family = size > 0 ? AF_UNIX : AF_UNSPEC;
  ::memcpy(data.sa_un.sun_path, unwrapped,
      std::min(size, size_t{UNIX_PATH_MAX}));
#endif
}


inline size_t
calculate_minsize(address_type type, bool with_type, bool with_port)
{
  size_t ret = 0;

  switch (type) {
    case AT_INET4:
      ret = sizeof(in_addr);
      break;

    case AT_INET6:
      ret = sizeof(in6_addr);
      break;

    default:
      return 0; // Not valid
      break;
  }

  if (with_port) {
    ret += sizeof(uint16_t);
  }
  if (with_type) {
    ret += 1;
  }

  return ret;
}


} // anonymous namespace

/*****************************************************************************
 * Member functions
 **/
socket_address::socket_address()
{
  // Need to zero data.
  ::memset(&data.sa_storage, 0, sizeof(data));
}



socket_address::socket_address(void const * buf, size_t len)
{
  std::size_t copy_len = std::min(sizeof(data), len);

  // Need to zero data.
  ::memset(&data.sa_storage, 0, sizeof(data));

  if (nullptr != buf) {
    ::memcpy(&data.sa_storage, buf, copy_len);

    // Now that we have the memory in a struct, we can determine the type. If
    // the given buffer is too small for the type, we can't have successfully
    // parsed anything.
    if (len < bufsize()) {
      // Zero data again to indicate parse error
      ::memset(&data.sa_storage, 0, sizeof(data));
    }
  }
}



socket_address::socket_address(address_type type, void const * buf, size_t len,
    uint16_t port /* = 0 */)
{
  // Need to zero data.
  ::memset(&data.sa_storage, 0, sizeof(data));

  if (type == AT_UNSPEC) {
    data.sa_storage.ss_family = AF_UNSPEC;
    // No data copying necessary.
    return;
  }

  if (!buf) {
    throw std::invalid_argument("socket_address: cannot construct without "
        "data.");
  }

  switch (type) {
    case AT_INET4:
      if (len != sizeof(data.sa_in.sin_addr)) {
        throw std::invalid_argument("socket_address: input buffer size invalid"
            " for address type.");
      }
      data.sa_storage.ss_family = AF_INET;
      ::memcpy(&data.sa_in.sin_addr, buf, len);
      data.sa_in.sin_port = htons(port);
      break;

    case AT_INET6:
      if (len != sizeof(data.sa_in6.sin6_addr)) {
        throw std::invalid_argument("socket_address: input buffer size invalid"
            "for address type.");
      }
      data.sa_storage.ss_family = AF_INET6;
      ::memcpy(&data.sa_in6.sin6_addr, buf, len);
      data.sa_in6.sin6_port = htons(port);
      break;

#if defined(LIBERATE_HAVE_SOCKADDR_UN)
    case AT_LOCAL:
      if (len < 1) {
        throw std::invalid_argument("socket_address: input buffer size invalid"
            "for address type.");
      }
      data.sa_storage.ss_family = AF_UNIX;
      ::memcpy(&data.sa_un.sun_path, buf, std::min(len, size_t{UNIX_PATH_MAX}));
      break;
#endif

    default:
      throw std::logic_error("Line should be unreachable!");
      break;
  }
}



socket_address::socket_address(std::string const & address,
    uint16_t port /* = 0 */)
{
  parse_address(data, address, address.size(), port);
}



socket_address::socket_address(char const * address, uint16_t port /* = 0 */
    , size_t size /* = 0 */)
{
  parse_address(data, address, size > 0 ? size : ::strlen(address), port);
}



bool
socket_address::verify_cidr(std::string const & address)
{
  detail::address_data dummy_addr;
  detail::parse_result_t result(dummy_addr);
  try {
    bool res = detail::parse_extended_cidr(address, true, result);
    return res;
  } catch (std::invalid_argument const &) {
    return false;
  }
}



size_t
socket_address::max_netmask() const
{
  if (AF_INET == data.sa_storage.ss_family) {
    return 32;
  }
  else if (AF_INET6 == data.sa_storage.ss_family) {
    return 128;
  }
  return 0;
}




bool
socket_address::verify_netmask(size_t const & netmask) const
{
  return netmask <= max_netmask();
}



std::string
socket_address::cidr_str() const
{
  // Interpret data as sockaddr_storage, sockaddr_in and sockaddr_in6. Of the
  // latter two, only one is safe to use!
  char buf[INET6_ADDRSTRLEN] = { '\0' };
  if (AF_INET == data.sa_storage.ss_family) {
#if defined(LIBERATE_WIN32)
    PVOID addr = (PVOID) (&(data.sa_in.sin_addr));
#else
    void const * addr = &(data.sa_in.sin_addr);
#endif
    inet_ntop(data.sa_storage.ss_family, addr, buf, sizeof(buf));
  }
  else if (AF_INET6 == data.sa_storage.ss_family) {
#if defined(LIBERATE_WIN32)
    PVOID addr = (PVOID) (&(data.sa_in6.sin6_addr));
#else
    void const * addr = &(data.sa_in6.sin6_addr);
#endif
    inet_ntop(data.sa_storage.ss_family, addr, buf, sizeof(buf));
  }

  return buf;
}



uint16_t
socket_address::port() const
{
  // Interpret data as sockaddr_storage, sockaddr_in and sockaddr_in6. Of the
  // latter two, only one is safe to use!
  switch (data.sa_storage.ss_family) {
    case AF_INET:
      return ntohs(data.sa_in.sin_port);

    case AF_INET6:
      return ntohs(data.sa_in6.sin6_port);

    default:
      return 0;
  }
}



std::string
socket_address::full_str() const
{
  std::stringstream sstream;

  switch (data.sa_storage.ss_family) {
    case AF_INET:
      sstream << cidr_str() << ":" << port();
      break;

    case AF_INET6:
      sstream << "[" << cidr_str() << "]:" << port();
      break;

#if defined(LIBERATE_HAVE_SOCKADDR_UN)
    case AF_UNIX:
      {
        std::string tmp{data.sa_un.sun_path, UNIX_PATH_MAX};
        auto last = tmp.find_last_not_of('\0');
        tmp.resize(last + 1);

#if defined(LIBERATE_WIN32)
        sstream << fs::to_posix_path(tmp);
#else // LIBERATE_WIN32
        sstream << tmp;
#endif // LIBERATE_WIN32
      }
      break;
#endif // LIBERATE_HAVE_SOCKADDR_UN

    case AF_UNSPEC:
    default:
      break;
  }

  return sstream.str();
}



size_t
socket_address::bufsize() const
{
  switch (data.sa_storage.ss_family) {
    case AF_INET:
      return sizeof(sockaddr_in);

    case AF_INET6:
      return sizeof(sockaddr_in6);

#if defined(LIBERATE_HAVE_SOCKADDR_UN)
    case AF_UNIX:
      {
        if (data.sa_un.sun_path[0] == '\0') {
          return sizeof(sockaddr_un);
        }
        else {
          return sizeof(sa_family_t)
            + ::strlen(data.sa_un.sun_path) + 1;
        }
      }
#endif

    default:
      break;
  }
  return 0;
}



size_t
socket_address::bufsize_available() const
{
  return sizeof(data);
}



void const *
socket_address::buffer() const
{
  return &(data.sa_storage);
}



void *
socket_address::buffer()
{
  return &(data.sa_storage);
}



size_t
socket_address::min_bufsize(bool with_type, bool with_port) const
{
  return calculate_minsize(type(), with_type, with_port);
}



size_t
socket_address::serialize(void * buf, size_t len, bool with_type, bool with_port) const
{
  auto remaining = min_bufsize(with_type, with_port);
  if (!remaining) {
    return 0;
  }

  if (len < remaining) {
    return 0;
  }

  uint8_t * offset = reinterpret_cast<uint8_t *>(buf);

  // Type first, if required.
  if (with_type) {
    uint8_t tmp = type();
    auto res = liberate::serialization::serialize_int(offset, remaining, tmp);
    if (res != 1) {
      return 0;
    }

    offset += res;
    remaining -= res;
  }

  // The main address buffer next. This one is a little complicated as it
  // depends on the type.
  switch (data.sa_storage.ss_family) {
    case AF_INET:
      {
        // The address should be in *network* Byte order, so we should just
        // copy it over.
        auto res = sizeof(in_addr);
        if (remaining < res) {
          return 0;
        }
        memcpy(offset, &(data.sa_in.sin_addr), res);

        offset += res;
        remaining -= res;
      }
      break;

    case AF_INET6:
      {
        // INET6 should be similar to INET4, except the address
        // is a Byte array instead of an integer. Doesn't really
        // make anything much different.
        auto res = sizeof(in6_addr);
        if (remaining < res) {
          return 0;
        }
        memcpy(offset, &(data.sa_in6.sin6_addr), res);

        offset += res;
        remaining -= res;
      }
      break;

    default:
      // Unreachable.
      break;
  }

  // Serialize port if necessary. Ports are also in network Byte order
  if (with_port) {
    auto tmp = data.sa_storage.ss_family == AF_INET
      ? &data.sa_in.sin_port
      : &data.sa_in6.sin6_port;

    auto required = sizeof(*tmp);
    if (remaining < required) {
      return 0;
    }

    memcpy(offset, tmp, required);

    offset += required;
    remaining -= required;
  }

  return offset - reinterpret_cast<uint8_t *>(buf);
}



std::tuple<size_t, socket_address>
socket_address::deserialize(address_type type, void const * buf, size_t len,
    bool with_port)
{
  if (type != AT_INET4 && type != AT_INET6) {
    return std::make_tuple(0, socket_address{});
  }

  if (!len) {
    return std::make_tuple(0, socket_address{});
  }

  auto remaining = calculate_minsize(type, false, with_port);
  if (remaining > len) {
    return std::make_tuple(0, socket_address{});
  }

  auto offset = reinterpret_cast<uint8_t const *>(buf);

  socket_address tmp;
  switch (type) {
    case AT_INET4:
      {
        tmp.data.sa_storage.ss_family = AF_INET;

        auto res = sizeof(in_addr);
        if (remaining < res) {
          return std::make_tuple(0, socket_address{});
        }
        memcpy(&(tmp.data.sa_in.sin_addr), offset, res);

        offset += res;
        remaining -= res;
      }
      break;

    case AT_INET6:
      {
        tmp.data.sa_storage.ss_family = AF_INET6;

        auto res = sizeof(in6_addr);
        if (remaining < res) {
          return std::make_tuple(0, socket_address{});
        }

        memcpy(&(tmp.data.sa_in6.sin6_addr), offset, res);

        offset += res;
        remaining -= res;
      }
      break;

    default:
      break;
  }

  // Deserialize port, if necessary
  if (with_port) {
    auto portp = tmp.data.sa_storage.ss_family == AF_INET
      ? &tmp.data.sa_in.sin_port
      : &tmp.data.sa_in6.sin6_port;

    auto required = sizeof(*portp);
    if (remaining < required) {
      return std::make_tuple(0, socket_address{});
    }

    memcpy(portp, offset, required);

    offset += required;
    remaining -= required;
  }

  return std::make_tuple(
      offset - reinterpret_cast<uint8_t const *>(buf),
      tmp);
}



std::tuple<size_t, socket_address>
socket_address::deserialize(void const * buf, size_t len, bool with_port)
{
  if (!len) {
    return std::make_tuple(0, socket_address{});
  }

  // Deserialize the type.
  auto ptr = reinterpret_cast<uint8_t const *>(buf);
  address_type type = static_cast<address_type>(*ptr);

  // Pass the rest of the buffer on to the typed function.
  auto [size, addr] = deserialize(type, ptr + 1, len - 1, with_port);
  if (!size) {
    return std::make_tuple(0, socket_address{});
  }
  return std::make_tuple(size + 1, addr);
}



bool
socket_address::set_port(uint16_t port)
{
  // Interpret data as sockaddr_storage, sockaddr_in and sockaddr_in6. Of the
  // latter two, only one is safe to use!
  switch (data.sa_storage.ss_family) {
    case AF_INET:
      data.sa_in.sin_port = htons(port);
      break;

    case AF_INET6:
      data.sa_in6.sin6_port = htons(port);
      break;

    default:
      return false;
  }

  return true;
}



bool
socket_address::is_any() const
{
  switch (data.sa_storage.ss_family) {
    case AF_INET:
      return data.sa_in.sin_addr.s_addr == ntohl(INADDR_ANY);

    case AF_INET6:
      return 0 == ::memcmp(&(data.sa_in6.sin6_addr), &in6addr_any,
          sizeof(in6addr_any));

    default:
      return false;
  }
}



bool
socket_address::is_loopback() const
{
  switch (data.sa_storage.ss_family) {
    case AF_INET:
      return data.sa_in.sin_addr.s_addr == ntohl(INADDR_LOOPBACK);

    case AF_INET6:
      return 0 == ::memcmp(&(data.sa_in6.sin6_addr), &in6addr_loopback,
          sizeof(in6addr_any));

    default:
      return false;
  }
}



bool
socket_address::is_equal_to(socket_address const & other) const
{
  // First compare families. It's a bit hard to decide what to return if the
  // families mismatch, but 'false' seems sensible.
  if (data.sa_storage.ss_family != other.data.sa_storage.ss_family) {
    return false;
  }

  // Next, depending on IPv4 or IPv6, compare the address part of the data.
  void const * compare_buf = nullptr;
  void const * compare_buf_other = nullptr;
  size_t compare_size = 0;

  // Now compare depending on family. IPv4 first
  if (AF_INET == data.sa_storage.ss_family) {
    if (data.sa_in.sin_port != other.data.sa_in.sin_port) {
      return false;
    }

    compare_size = sizeof(in_addr);
    compare_buf = &(data.sa_in.sin_addr);
    compare_buf_other = &(other.data.sa_in.sin_addr);
  }

  // IPv6
  else if (AF_INET6 == data.sa_storage.ss_family) {
    if (data.sa_in6.sin6_port != other.data.sa_in6.sin6_port) {
      return false;
    }

    compare_size = sizeof(in6_addr);
    compare_buf = &(data.sa_in6.sin6_addr);
    compare_buf_other = &(other.data.sa_in6.sin6_addr);
  }

#if defined(LIBERATE_HAVE_SOCKADDR_UN)
  // UNIX
  else {
    compare_size = UNIX_PATH_MAX;
    compare_buf = data.sa_un.sun_path;
    compare_buf_other = other.data.sa_un.sun_path;
  }
#endif

  return 0 == ::memcmp(compare_buf, compare_buf_other, compare_size);
}



bool
socket_address::is_less_than(socket_address const & other) const
{
  // First compare families. The order is unimportant, but comparing on
  // the actual family value is a nice quick way to deal with this.
  if (data.sa_storage.ss_family != other.data.sa_storage.ss_family) {
    return data.sa_storage.ss_family < other.data.sa_storage.ss_family;
  }

  // Now compare depending on family. IPv4 is simple.
  if (AF_INET == data.sa_storage.ss_family) {
    if (ntohl(data.sa_in.sin_addr.s_addr)
        < ntohl(other.data.sa_in.sin_addr.s_addr)) {
      return true;
    }
    return ntohs(data.sa_in.sin_port) < ntohs(other.data.sa_in.sin_port);
  }

  // IPv6 is harder, but not too hard. We need to compare byte by byte.
  else if (AF_INET6 == data.sa_storage.ss_family) {
    for (ssize_t i = 0 ; i < 16 ; ++i) {
      if (data.sa_in6.sin6_addr.s6_addr[i]
          < other.data.sa_in6.sin6_addr.s6_addr[i]) {
        return true;
      }
    }

    return ntohs(data.sa_in6.sin6_port) < ntohs(other.data.sa_in6.sin6_port);
  }

#if defined(LIBERATE_HAVE_SOCKADDR_UN)
  // Unix paths are simple again.
  return 0 > ::memcmp(data.sa_un.sun_path, other.data.sa_un.sun_path,
      UNIX_PATH_MAX);
#else
  // We don't know what to do, really.
  return false;
#endif
}



void
socket_address::operator++()
{
  // As always, IPv4 is pretty simple.
  if (AF_INET == data.sa_storage.ss_family) {
    uint32_t ip = ntohl(data.sa_in.sin_addr.s_addr);
    data.sa_in.sin_addr.s_addr = htonl(ip + 1);
    return;
  }
  else if (AF_INET6 == data.sa_storage.ss_family) {
    // IPv6 is still simple enough, we just have to handle overflow from one
    // byte into the next.
    bool done = false;
    for (ssize_t i = 15 ; i >= 0 && !done ; --i) {
      if (data.sa_in6.sin6_addr.s6_addr[i] < 0xff) {
        ++data.sa_in6.sin6_addr.s6_addr[i];
        done = true;
      }
    }
    return;
  }

  throw std::domain_error("Don't know how to increment paths.");
}



address_type
socket_address::type() const
{
  switch (data.sa_storage.ss_family) {
    case AF_INET:
      return AT_INET4;

    case AF_INET6:
      return AT_INET6;

#if defined(LIBERATE_HAVE_SOCKADDR_UN)
    case AF_UNIX:
      return AT_LOCAL;
#endif

    default:
      return AT_UNSPEC;
  }
}



size_t
socket_address::hash() const
{
  if (AF_UNSPEC == data.sa_storage.ss_family) {
    return 0;
  }

  // Figure out buffer to compare.
  void const * hash_buf = nullptr;
  size_t hash_size = 0;

  uint16_t port = 0;

  // IPv4
  if (AF_INET == data.sa_storage.ss_family) {
    hash_size = sizeof(in_addr);
    hash_buf = &(data.sa_in.sin_addr);
    port = data.sa_in.sin_port;
  }

  // IPv6
  else if (AF_INET6 == data.sa_storage.ss_family) {
    hash_size = sizeof(in6_addr);
    hash_buf = &(data.sa_in6.sin6_addr);
    port = data.sa_in6.sin6_port;
  }

#if defined(LIBERATE_HAVE_SOCKADDR_UN)
  // UNIX
  else {
    hash_size = ::strlen(data.sa_un.sun_path);
    hash_buf = data.sa_un.sun_path;
    port = 0;
  }
#endif

  return liberate::cpp::multi_hash(
      std::string(static_cast<char const *>(hash_buf), hash_size),
      port);
}



// cppcheck-suppress constParameter
void socket_address::swap(socket_address & other)
{
  void * buf1 = &(data.sa_storage);
  void * buf2 = &(other.data.sa_storage);

  // Use int wise XOR, because int should be most optimized.
  size_t offset = 0;
  size_t max_offset = sizeof(data.sa_storage) / sizeof(int);
  for ( ; offset < max_offset ; ++offset) {
    static_cast<int *>(buf1)[offset] ^= static_cast<int *>(buf2)[offset];
    static_cast<int *>(buf2)[offset] ^= static_cast<int *>(buf1)[offset];
    static_cast<int *>(buf1)[offset] ^= static_cast<int *>(buf2)[offset];
  }

  // Now char-wise until the end of the buffer
  max_offset = sizeof(data.sa_storage) - max_offset;
  offset *= sizeof(int);
  for ( ; offset < max_offset ; ++offset) {
    static_cast<char *>(buf1)[offset] ^= static_cast<char *>(buf2)[offset];
    static_cast<char *>(buf2)[offset] ^= static_cast<char *>(buf1)[offset];
    static_cast<char *>(buf1)[offset] ^= static_cast<char *>(buf2)[offset];
  }
}



/*****************************************************************************
 * Friend functions
 **/
std::ostream &
operator<<(std::ostream & os, socket_address const & addr)
{
  os << addr.full_str();
  return os;
}


} // namespace liberate::net
