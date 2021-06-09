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
 * PARTICULAR sRPOSE.
 **/
#include <liberate/string/urlencode.h>

#include <gtest/gtest.h>


TEST(StringURLDecode, urlencode)
{
  namespace s = liberate::string;

  ASSERT_EQ("foo/bar", s::urlencode("foo/bar"));
  ASSERT_EQ("/%7Efoo/bar", s::urlencode("/~foo/bar"));
  ASSERT_EQ("%00abstract", s::urlencode(std::string{"\0abstract", 9}));
  ASSERT_EQ("%25asdf", s::urlencode("%asdf"));
}



TEST(StringURLDecode, urldecode)
{
  namespace s = liberate::string;

  ASSERT_EQ(s::urldecode("foo/bar"), "foo/bar");
  ASSERT_EQ(s::urldecode("/%7Efoo/bar"), "/~foo/bar");
  ASSERT_EQ(s::urldecode("%00abstract"), (std::string{"\0abstract", 9}));
  ASSERT_EQ(s::urldecode("%25asdf"), "%asdf");
}
