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
#include <cstddef>

#include <liberate/serialization/integer.h>
#include <liberate/serialization/varint.h>

#include <gtest/gtest.h>

TEST(SerializationVarint, serialize_to_uint8_t)
{
  using namespace liberate::types::literals;
  using namespace liberate::types;
  using namespace liberate::serialization;

  uint8_t out[20] = { 0 };

  auto test = 0x01020304_var;

  // The following is not enabled.
  // auto written = serialize_int(&out[0], sizeof(out), test);

  // This, however, should work.
  auto written = serialize_varint(&out[0], sizeof(out), test);

  ASSERT_EQ(written, 4);

  ASSERT_EQ(out[0], 0x87);
  ASSERT_EQ(out[1], 0x87);
  ASSERT_EQ(out[2], 0x85);
  ASSERT_EQ(out[3], 0x04);

  // A value of 127 or less should be encoded in one byte.
  test = 127_var;
  written = serialize_varint(&out[0], sizeof(out), test);
  ASSERT_EQ(written, 1);
  ASSERT_EQ(out[0], 127);
}


TEST(SerializationVarint, serialize_to_byte)
{
  using namespace liberate::types::literals;
  using namespace liberate::types;
  using namespace liberate::serialization;

  std::byte out[20] = { 0 };

  auto test = 0x01020304_var;

  // The following is not enabled.
  // auto written = serialize_int(&out[0], sizeof(out), test);

  // This, however, should work.
  auto written = serialize_varint(&out[0], sizeof(out), test);

  ASSERT_EQ(written, 4);

  ASSERT_EQ(out[0], static_cast<std::byte>(0x87));
  ASSERT_EQ(out[1], static_cast<std::byte>(0x87));
  ASSERT_EQ(out[2], static_cast<std::byte>(0x85));
  ASSERT_EQ(out[3], static_cast<std::byte>(0x04));

  // A value of 127 or less should be encoded in one byte.
  test = 127_var;
  written = serialize_varint(&out[0], sizeof(out), test);
  ASSERT_EQ(written, 1);
  ASSERT_EQ(out[0], static_cast<std::byte>(127));
}
