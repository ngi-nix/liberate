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

#include <liberate/sys/error.h>

#include <sstream>
#include <ios>

#if defined(LIBERATE_POSIX)
#  include <string.h>
#else
#  include "../net/netincludes.h"
#  include <liberate/string/utf8.h>
#endif

namespace liberate::sys {

namespace {

#if defined(LIBERATE_POSIX)

static constexpr char const * const FALLBACK_ERROR = "Error copying error message.";

std::string
strerror_helper(int code)
{
  static char buf[1024] = { 0 };
#if defined(LIBERATE_HAVE_STRERROR_S)
  auto e = ::strerror_s(buf, sizeof(buf), code);
  if (e) {
    return FALLBACK_ERROR;
  }
  return buf;
#elif defined(LIBERATE_HAVE_STRERROR_R)
#  if (_POSIX_C_SOURCE >= 200112L) && !  _GNU_SOURCE
  // XSI compliant
  int e = ::strerror_r(code, buf, sizeof(buf));
  if (e) {
    return FALLBACK_ERROR;
  }
  return buf;
#  else
  // GNU-specific
  char * e = ::strerror_r(code, buf, sizeof(buf));
  if (!e) {
    return FALLBACK_ERROR;
  }
  return e;
#  endif
#else
  // Fall back to plain strerror
  return ::strerror(code);
#endif
}

#endif

} // anonymous namespace



int error_code()
{
#if defined(LIBERATE_POSIX)
  return errno;
#else
  return WSAGetLastError();
#endif
}



std::string
error_message(int code)
{
#if defined(LIBERATE_POSIX)
  std::stringstream msg;
  msg << "[0x" << std::hex << code << std::dec << " (" << code << ")] "
    << strerror_helper(code);
  return msg.str();
#else
  TCHAR * errmsg = NULL;
  FormatMessageW(
      FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_IGNORE_INSERTS,
      NULL, code,
      MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
      (LPWSTR) &errmsg, 0, NULL);

  std::stringstream msg;
  msg << "[0x" << std::hex << code << std::dec << " (" << code << ")] "
      << ::liberate::string::to_utf8(errmsg);

  LocalFree(errmsg);
  return msg.str();
#endif
}


} // namespace liberate::sys
