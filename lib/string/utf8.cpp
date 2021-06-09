/**
 * This file is part of liberate.
 *
 * Author(s): Jens Finkhaeuser <jens@finkhaeuser.de>
 *
 * Copyright (c) 2019-2021 Jens Finkhaeuser.
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
#include <liberate/string/utf8.h>

#if defined(LIBERATE_WIN32)

#include <vector>

namespace liberate::string {

std::string
to_utf8(TCHAR const * source)
{
  size_t size = WideCharToMultiByte(CP_UTF8, 0, source, -1, nullptr, 0,
      nullptr, nullptr);
  std::vector<char> buf(size, '\0');
  WideCharToMultiByte(CP_UTF8, 0, source, -1, &buf[0], size, nullptr, nullptr);
  return {buf.begin(), buf.end() - 1};
}



std::wstring
from_utf8(char const * source)
{
  size_t size = MultiByteToWideChar(CP_UTF8, 0, source, -1, nullptr, 0);
  std::vector<wchar_t> buf(size, 0);
  MultiByteToWideChar(CP_UTF8, 0, source, -1, &buf[0], size);
  return {buf.begin(), buf.end() - 1};
}


} // namespace liberate::string

#endif // Win32
