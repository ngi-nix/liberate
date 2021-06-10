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
#include <liberate/types/byte.h>

#include <gtest/gtest.h>

TEST(ByteTest, literals)
{
  using namespace liberate::types;
  using namespace liberate::types::literals;

  // Basic conversion
  auto v = 0x42_b;
  ASSERT_EQ(typeid(v), typeid(byte));

  auto s = "hello"_b;
  ASSERT_EQ(typeid(s), typeid(std::vector<byte>));

  auto c = '\x10'_b;
  ASSERT_EQ(typeid(c), typeid(byte));

  // Long integers are truncated
  auto x = 123456_b;
  ASSERT_EQ(typeid(x), typeid(byte));
  ASSERT_EQ((123456 % 256), x);

  // Does not compile (good!)
  // auto y = 1.23_b;
}
