/**
 * This file is part of liberate.
 *
 * Author(s): Jens Finkhaeuser <jens@finkhaeuser.de>
 *
 * Copyright (c) 2019-2020 Jens Finkhaeuser.
 *
 * This software is licensed under the terms of the GNU GPLv3 for personal,
 * educational and non-profit use. For all value uses, alternative license
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
#include <liberate/string/urlencode.h>

#include <liberate/string/hexencode.h>

namespace liberate::string {

std::string
urlencode(std::string const & input)
{
  std::string ret;

  for (auto ch : input) {
    // Keep alphanumeric and other accepted characters intact
    if (isalnum(ch) || ch == '-' || ch == '_' || ch == '.' || ch == '/') {
      ret += ch;
      continue;
    }

    ret += "%";
    ret += hexencode(&ch, 1, true);
  }

  return ret;
}



std::string
urldecode(std::string const & input)
{
  std::string ret;

  for (std::string::size_type i = 0 ; i < input.size() ; ++i) {
    auto ch = input[i];

    // Percent-encoded
    if (ch == '%') {
      ::liberate::types::byte val;
      auto used = hexdecode(&val, 1,
          reinterpret_cast<::liberate::types::byte const *>(input.c_str() + i + 1), 2);
      if (used > 0) {
        ret += static_cast<char>(val);
      }
      else {
        ret += "?";
      }
      i += 2;
      continue;
    }

    // Regular value
    ret += ch;
  }

  return ret;
}

} // namespace liberate::string
