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
#ifndef LIBERATE_LOGGING_H
#define LIBERATE_LOGGING_H

#ifndef __cplusplus
#error You are trying to include a C++ only header file
#endif

#include <liberate.h>

#include <sstream>
#include <ios>

#if defined(LIBERATE_IS_BUILDING) && LIBERATE_IS_BUILDING > 0
#include <build-config.h>
#endif

/**
 * Possible backends & default
 */
#define LIBERATE_LOG_BACKEND_STDERR 1
#define LIBERATE_LOG_BACKEND_PLOG   2
#define LIBERATE_LOG_BACKEND_SPDLOG 3
#define LIBERATE_LOG_BACKEND_LOGURU 4

#if !defined(LIBERATE_LOG_BACKEND)
#define LIBERATE_LOG_BACKEND LIBERATE_LOG_BACKEND_STDERR
#endif

/**
 * Generate error message string from platform error code.
 */
#if defined(LIBERATE_POSIX)

#include <string.h>

namespace liberate {

inline std::string
error_message(int code)
{
  std::stringstream msg;
  msg << "[0x" << std::hex << code << std::dec << " (" << code << ")] "
    << ::strerror(code);
  return msg.str();
}

} // namespace liberate

#else // LIBERATE_POSIX/LIBERATE_WIN32

#include <liberate/string/utf8.h>

namespace liberate {

inline std::string
error_message(int code)
{
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
}

} // namespace liberate

#endif // LIBERATE_WIN32

/**
 * Backends
 */
#if LIBERATE_LOG_BACKEND == LIBERATE_LOG_BACKEND_PLOG

#include <plog/Log.h>

#if defined(PLOG)
#define PLOG_MACRO PLOG
#else
#define PLOG_MACRO LOG
#endif

// Levels
#define LIBLOG_LEVEL_TRACE plog::verbose
#define LIBLOG_LEVEL_DEBUG plog::debug
#define LIBLOG_LEVEL_INFO plog::info
#define LIBLOG_LEVEL_WARN plog::warning
#define LIBLOG_LEVEL_ERROR plog::error
#define LIBLOG_LEVEL_FATAL plog::fatal

// Basic macro
#define LIBLOG(level, message) PLOG_MACRO(level) << message;

#elif LIBERATE_LOG_BACKEND == LIBERATE_LOG_BACKEND_SPDLOG

#include <spdlog/spdlog.h>

// Levels
#define LIBLOG_LEVEL_TRACE spdlog::level::trace
#define LIBLOG_LEVEL_DEBUG spdlog::level::debug
#define LIBLOG_LEVEL_INFO spdlog::level::info
#define LIBLOG_LEVEL_WARN spdlog::level::warn
#define LIBLOG_LEVEL_ERROR spdlog::level::err
#define LIBLOG_LEVEL_FATAL spdlog::level::critical

// Basic macro
#define LIBLOG(level, message) { \
  std::stringstream sstream; \
  sstream << message; \
  SPDLOG_LOGGER_CALL(spdlog::default_logger_raw(), level, sstream.str()); \
}

#elif LIBERATE_LOG_BACKEND == LIBERATE_LOG_BACKEND_LOGURU

#define LOGURU_WITH_STREAMS 1
#include <loguru.hpp>

// Levels
#define LIBLOG_LEVEL_TRACE MAX
#define LIBLOG_LEVEL_DEBUG 1
#define LIBLOG_LEVEL_INFO INFO
#define LIBLOG_LEVEL_WARN WARNING
#define LIBLOG_LEVEL_ERROR ERROR
#define LIBLOG_LEVEL_FATAL FATAL

// Basic macro
#define LIBLOG(level, message) LOG_S(level) << message;

#else // LIBERATE_LOG_BACKEND == LIBERATE_LOG_BACKEND_STDERR

#if defined(DEBUG) && !defined(NDEBUG)

#include <iostream>

#define LIBLOG(level, msg) { \
  std::stringstream sstream; \
  sstream << "[" << __FILE__ << ":" \
  << __LINE__ << "] " << level << ": " << msg << std::endl; \
  std::cerr << sstream.str(); \
}
#else
#define LIBLOG(level, msg)
#endif

// Levels
#define LIBLOG_LEVEL_TRACE "TRACE"
#define LIBLOG_LEVEL_DEBUG "DEBUG"
#define LIBLOG_LEVEL_INFO "INFO"
#define LIBLOG_LEVEL_WARN "WARN"
#define LIBLOG_LEVEL_ERROR "ERROR"
#define LIBLOG_LEVEL_FATAL "FATAL"

#endif // LIBERATE_LOG_BACKEND == LIBERATE_LOG_BACKEND_STDERR

// Long macros (apply to all backends)
#define LIBLOG_TRACE(msg) LIBLOG(LIBLOG_LEVEL_TRACE, msg)
#define LIBLOG_DEBUG(msg) LIBLOG(LIBLOG_LEVEL_DEBUG, msg)
#define LIBLOG_INFO(msg) LIBLOG(LIBLOG_LEVEL_INFO, msg)
#define LIBLOG_WARN(msg) LIBLOG(LIBLOG_LEVEL_WARN, msg)
#define LIBLOG_ERROR(msg) LIBLOG(LIBLOG_LEVEL_ERROR, msg)
#define LIBLOG_FATAL(msg) LIBLOG(LIBLOG_LEVEL_FATAL, msg)

// Short macros (apply to all backends)
#define LLOG_T(msg) LIBLOG_TRACE(msg)
#define LLOG_D(msg) LIBLOG_DEBUG(msg)
#define LLOG_I(msg) LIBLOG_INFO(msg)
#define LLOG_W(msg) LIBLOG_WARN(msg)
#define LLOG_E(msg) LIBLOG_ERROR(msg)
#define LLOG_F(msg) LIBLOG_FATAL(msg)

/**
 * Error logging macros
 */
#define LIBLOG_ERR(code, message) LIBLOG_ERROR(message << " // " << liberate::error_message(code))

#if defined(LIBERATE_POSIX)
#define LIBLOG_ERRNO(message) LIBLOG_ERR(errno, message)
#else
#define LIBLOG_ERRNO(message) LIBLOG_ERR(GetLastError(), message)
#endif

#define LIBLOG_EXC(exc, message) LIBLOG_ERROR(message << " // " << (exc.what()))

#endif // guard
