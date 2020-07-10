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

#include <liberate/fs/tmp.h>

#include <stdexcept>

#if defined(LIBERATE_WIN32)

#include <chrono>
#include <sstream>

#include <liberate/string/utf8.h>

namespace liberate::fs {

std::string
temp_name(std::string const & prefix /* = "" */)
{
  // Temporary directory
  wchar_t tmpdir[MAX_PATH + 1] = { 0 };
  auto len = GetTempPath(sizeof(tmpdir) / sizeof(wchar_t), tmpdir);
  if (len <= 0) {
    throw std::runtime_error{"GetTempPath() failed."};
  }

  // Now get the temporary file name
  auto now = std::chrono::steady_clock::now();
  std::stringstream ts;
  ts << std::hex << now.time_since_epoch().count();

  // Generate return value
  std::string ret = string::to_utf8(tmpdir);
  if (prefix.empty()) {
    ret += "liberate";
  }
  else {
    ret += prefix;
  }
  ret += "-" + ts.str() + ".tmp";

  return ret;
}

} // namespace liberate::fs

#else // LIBERATE_WIN32

#include <stdlib.h>
#include <unistd.h>

#include <vector>

namespace liberate::fs {

std::string
temp_name(std::string const & prefix /* = "" */)
{
  // Get temporary directory
  char const * tmpdir = getenv("TMPDIR");
  if (!tmpdir) {
    tmpdir = "/tmp";
  }

  // Template
  std::string templ = tmpdir;
  if (prefix.empty()) {
    templ += "/liberate-XXXXXX";
  }
  else {
    templ += "/" + prefix + "-XXXXXX";
  }

  // Copy to writable buffer
  std::vector<char> buf{templ.begin(), templ.end()};
  buf.push_back(0);

  // Create temporary file
  int fd = mkstemp(&buf[0]);
  if (fd < 0) {
    throw std::runtime_error{"mkstemp() failed."};
  }
  std::string ret{buf.begin(), buf.end()};

  // Cleanup
  close(fd);
  unlink(ret.c_str());

  return ret;
}


} // namespace liberate::util

#endif // LIBERATE_WIN32
