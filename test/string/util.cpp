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
#include <liberate/string/util.h>

#include <gtest/gtest.h>


TEST(StringUtil, to_lower)
{
  namespace s = liberate::string;

  ASSERT_EQ("foo", s::to_lower("foo"));
  ASSERT_EQ("foo", s::to_lower("Foo"));
  ASSERT_EQ("foo", s::to_lower("fOo"));
  ASSERT_EQ("foo", s::to_lower("foO"));
  ASSERT_EQ("foo", s::to_lower("FOO"));

  ASSERT_EQ("", s::to_lower(""));
  ASSERT_EQ("a", s::to_lower("A"));
}


TEST(StringUtil, to_upper)
{
  namespace s = liberate::string;

  ASSERT_EQ("FOO", s::to_upper("foo"));
  ASSERT_EQ("FOO", s::to_upper("Foo"));
  ASSERT_EQ("FOO", s::to_upper("fOo"));
  ASSERT_EQ("FOO", s::to_upper("foO"));
  ASSERT_EQ("FOO", s::to_upper("FOO"));

  ASSERT_EQ("", s::to_upper(""));
  ASSERT_EQ("A", s::to_upper("a"));
}


TEST(StringUtil, case_insensitive_search)
{
  namespace s = liberate::string;

  auto res = s::ifind("This is a Test String", "test");
  ASSERT_GE(res, 0); // Less than zero would be failure

  // "test" is at offset 10
  ASSERT_EQ(10, res);

  // Find something at the beginning and end of strings
  ASSERT_EQ(0, s::ifind("foobar", "FOO"));
  ASSERT_EQ(3, s::ifind("foobar", "Bar"));

  // Return -1 if string can't be found
  ASSERT_EQ(-1, s::ifind("foobar", "quux"));
  ASSERT_EQ(-1, s::ifind("quu", "quux"));
  ASSERT_EQ(-1, s::ifind("", "quux"));

  // Find the empty string anywhere, so at 0.
  ASSERT_EQ(0, s::ifind("foobar", ""));
  // .. except in an empty string
  ASSERT_EQ(-1, s::ifind("", ""));
}


TEST(StringUtil, replace)
{
  namespace s = liberate::string;

  ASSERT_EQ("foo", s::replace("f0o", "0", "o"));
  ASSERT_EQ("fo0", s::replace("f00", "0", "o", true));

  ASSERT_EQ("\\\\quoted\\\\and\\\\separated\\\\",
      s::replace("\\quoted\\and\\separated\\", "\\", "\\\\"));
}
