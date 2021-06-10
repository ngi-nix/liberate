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

static const std::string hd_canonical{
"00000000  48 65 6c 6c 6f 2c 20 77  6f 72 6c 64 21 20 54 68  |Hello, world! Th|\n"
"00000010  65 20 71 75 69 63 6b 20  62 72 6f 77 6e 20 66 6f  |e quick brown fo|\n"
"00000020  78 20 6a 75 6d 70 65 64  20 6f 76 65 72 20 74 68  |x jumped over th|\n"
"00000030  65 20 6c 61 7a 79 20 64  6f 67 27 73 20 62 61 63  |e lazy dog's bac|\n"
"00000040  6b 20 61 6e 64 20 73 61  74 20 6f 6e 20 61 20 74  |k and sat on a t|\n"
"00000050  61 63 6b 2e                                       |ack.|\n"
"00000054"};

static const std::string hd_wide{
"0000  48656c6c 6f2c2077 6f726c64 21205468  65207175 69636b20 62726f77 6e20666f  Hello,.world!.The.quick.brown.fo\n"
"0020  78206a75 6d706564 206f7665 72207468  65206c61 7a792064 6f672773 20626163  x.jumped.over.the.lazy.dog's.bac\n"
"0040  6b20616e 64207361 74206f6e 20612074  61636b2e                             k.and.sat.on.a.tack.\n"
"0054"};



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


TEST(StringHexDump, canonical_raw)
{
  std::string test{"Hello, world! The quick brown fox jumped over the lazy dog's back and sat on a tack."};

  liberate::string::canonical_hexdump dumper;
  auto result = dumper(test.c_str(), test.size());

  ASSERT_EQ(result.size(), hd_canonical.size());
  for (size_t i = 0 ; i < result.size() ; ++i) {
    ASSERT_EQ(hd_canonical[i], result[i]);
  }
}


TEST(StringHexDump, canonical_string)
{
  std::string test{"Hello, world! The quick brown fox jumped over the lazy dog's back and sat on a tack."};

  liberate::string::canonical_hexdump dumper;
  auto result = dumper(test);

  ASSERT_EQ(result.size(), hd_canonical.size());
  for (size_t i = 0 ; i < result.size() ; ++i) {
    ASSERT_EQ(hd_canonical[i], result[i]);
  }
}


namespace {

inline std::vector<std::string>
split(std::string const & input)
{
  std::vector<std::string> res;

  std::size_t pos = 0;
  std::size_t last = 0;

  while ((pos = input.find('\n', last)) != std::string::npos) {
    res.push_back(input.substr(last, (pos - last)));
    last = pos + 1;
  }

  res.push_back(input.substr(last));

  return res;
}

inline void
compare_wide_dump(std::string const & expected, std::string const & result)
{
  // The offset size is truncated to 16 bit in hd_wide, because we want to make
  // this test workable for every platform.
  auto missing_bytes = (sizeof(intptr_t) - 2) * 2;

  // We should have the same number of lines
  auto split_exp = split(expected);
  auto split_res = split(result);
  ASSERT_FALSE(split_exp.empty());
  ASSERT_EQ(split_exp.size(), split_res.size());

  // So now we can compare line-by-line
  for (size_t i = 0 ; i < split_exp.size() ; ++i) {
    ASSERT_EQ(split_exp[i].size() + missing_bytes, split_res[i].size());
    auto tmp = split_res[i].substr(missing_bytes);
    ASSERT_EQ(split_exp[i], tmp);
  }
}

} // anonymous namespace


TEST(StringHexDump, wide_raw)
{
  std::string test{"Hello, world! The quick brown fox jumped over the lazy dog's back and sat on a tack."};

  liberate::string::wide_hexdump dumper;
  auto result = dumper(test.c_str(), test.size());

  compare_wide_dump(hd_wide, result);
}


TEST(StringHexDump, wide_string)
{
  std::string test{"Hello, world! The quick brown fox jumped over the lazy dog's back and sat on a tack."};

  liberate::string::wide_hexdump dumper;
  auto result = dumper(test);

  compare_wide_dump(hd_wide, result);
}
