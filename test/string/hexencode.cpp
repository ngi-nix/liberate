/**
 * This file is part of liberate.
 *
 * Author(s): Jens Finkhaeuser <jens@finkhaeuser.de>
 *
 * Copyright (c) 2021 Jens Finkhaeuser.
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
#include <liberate/string/hexencode.h>

#include <gtest/gtest.h>

static const std::string plain = "Hello, world!";
static const size_t plain_size = plain.size();
static const size_t out_size = plain_size * 2;
static const std::string expected{"48656c6c6f2c20776f726c6421"};


TEST(StringHexEncode, encode_raw)
{
  std::vector<::liberate::types::byte> out;
  out.resize(out_size);

  auto used = liberate::string::hexencode(
      &out[0], out.size(),
      reinterpret_cast<::liberate::types::byte const *>(plain.c_str()),
      plain_size);
  ASSERT_EQ(used, out_size);

  for (size_t i = 0 ; i < out_size ; ++i) {
    ASSERT_EQ(expected[i], static_cast<char>(out[i]));
  }
}



TEST(StringHexDecode, decode_raw)
{
  std::vector<::liberate::types::byte> out;
  out.resize(plain_size);

  auto used = liberate::string::hexdecode(
      &out[0], out.size(),
      reinterpret_cast<::liberate::types::byte const *>(expected.c_str()),
      out_size);
  ASSERT_EQ(used, plain_size);

  for (size_t i = 0 ; i < plain_size ; ++i) {
    ASSERT_EQ(plain[i], static_cast<char>(out[i]));
  }
}



TEST(StringHexEncode, encode_char)
{
  auto result = liberate::string::hexencode(plain.c_str(), plain.size());
  ASSERT_EQ(result, expected);
}



TEST(StringHexEncode, encode_bytes)
{
  auto result = liberate::string::hexencode(
      reinterpret_cast<::liberate::types::byte const *>(plain.c_str()), plain.size());
  ASSERT_EQ(result, expected);
}



TEST(StringHexDecode, decode_char)
{
  auto result = liberate::string::hexdecode(
      expected.c_str(), expected.size());
  ASSERT_EQ(result.size(), plain.size());

  for (size_t i = 0 ; i < result.size() ; ++i) {
    ASSERT_EQ(plain[i], static_cast<char>(result[i]));
  }
}



TEST(StringHexDecode, decode_bytes)
{
  auto result = liberate::string::hexdecode(
      reinterpret_cast<::liberate::types::byte const *>(expected.c_str()), expected.size());
  ASSERT_EQ(result.size(), plain.size());

  for (size_t i = 0 ; i < result.size() ; ++i) {
    ASSERT_EQ(plain[i], static_cast<char>(result[i]));
  }
}


// - use in urlencode
