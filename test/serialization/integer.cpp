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
#include <liberate/serialization/integer.h>

#include <gtest/gtest.h>

TEST(SerializationInteger, serialize_to_byte)
{
  using namespace liberate::serialization;

  std::vector<std::byte> out;
  out.resize(20);

  uint32_t test = 0x01020304;
  auto written = serialize_int(&out[0], out.size(), test);
  ASSERT_EQ(written, 4);

  ASSERT_EQ(out[0], static_cast<std::byte>(0x01));
  ASSERT_EQ(out[1], static_cast<std::byte>(0x02));
  ASSERT_EQ(out[2], static_cast<std::byte>(0x03));
  ASSERT_EQ(out[3], static_cast<std::byte>(0x04));
}


TEST(SerializationInteger, serialize_to_char)
{
  using namespace liberate::serialization;

  std::vector<char> out;
  out.resize(20);

  uint32_t test = 0x01020304;
  auto written = serialize_int(&out[0], out.size(), test);
  ASSERT_EQ(written, 4);

  ASSERT_EQ(out[0], 0x01);
  ASSERT_EQ(out[1], 0x02);
  ASSERT_EQ(out[2], 0x03);
  ASSERT_EQ(out[3], 0x04);
}


TEST(SerializationInteger, detail_serialize_to_uint16_t)
{
  using namespace liberate::serialization;

  std::vector<uint16_t> out;
  out.resize(10);

  uint32_t test = 0x01020304;
  auto written = detail::serialize_int_impl(&out[0], out.size(), test);
  ASSERT_EQ(written, 2);

  ASSERT_EQ(out[0], 0x0102);
  ASSERT_EQ(out[1], 0x0304);
}


TEST(SerializationInteger, deserialize_from_char)
{
  using namespace liberate::serialization;

  char buf[] = { 0x01, 0x02, 0x03, 0x04,
    static_cast<char>(0xde), static_cast<char>(0xad) };

  uint32_t result = 0;
  auto read = deserialize_int(result, buf, sizeof(buf));

  ASSERT_EQ(read, 4);

  ASSERT_EQ(result, 0x01020304);
}


TEST(SerializationInteger, deserialize_from_byte)
{
  using namespace liberate::serialization;

  std::byte buf[] = {
    static_cast<std::byte>(0x01),
    static_cast<std::byte>(0x02),
    static_cast<std::byte>(0x03),
    static_cast<std::byte>(0x04),
    static_cast<std::byte>(0xde),
    static_cast<std::byte>(0xad),
  };

  uint32_t result = 0;
  auto read = deserialize_int(result, buf, sizeof(buf));

  ASSERT_EQ(read, 4);

  ASSERT_EQ(result, 0x01020304);
}


TEST(SerializationInteger, detail_deserialize_from_uint16_t)
{
  using namespace liberate::serialization;

  uint16_t buf[] = { 0x0102, 0x0304, 0xdead };

  uint32_t result = 0;
  auto read = detail::deserialize_int_impl(result, buf, sizeof(buf));
  ASSERT_EQ(read, 2);

  ASSERT_EQ(result, 0x01020304);
}
