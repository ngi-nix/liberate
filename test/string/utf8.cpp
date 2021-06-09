/**
 * This file is part of liberate.
 *
 * Author(s): Jens Finkhaeuser <jens@finkhaeuser.de>
 *
 * Copyright (c) 2019-2021 Jens Finkhaeuser.
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
#include <liberate/string/utf8.h>

#include <gtest/gtest.h>

#if defined(LIBERATE_WIN32)

#if defined(LIBERATE_BIGENDIAN)
#  error Define test_data appropriately
#else
TCHAR const test_data[] {
  TCHAR{'h'},
  TCHAR{'e'},
  TCHAR{'l'},
  TCHAR{'l'},
  TCHAR{'o'}
};
#endif

TEST(StringUTF8, ucs2_to_utf8)
{
  std::string res = liberate::string::to_utf8(test_data);
  ASSERT_EQ(res, std::string{"hello"});
}

TEST(StringUTF8, utf8_to_ucs2)
{
  std::wstring res = liberate::string::from_utf8("hello");
  ASSERT_EQ(res, std::wstring{test_data});
}

#endif // LIBERATE_WIN32
