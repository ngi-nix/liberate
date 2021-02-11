/**
 * This file is part of liberate.
 *
 * Author(s): Jens Finkhaeuser <jens@finkhaeuser.de>
 *
 * Copyright (c) 2021 Jens Finkhaeuser.
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
#include <liberate/string/hexencode.h>

namespace liberate::string {

namespace {

static constexpr std::byte hexalpha_lower[] = {
  std::byte{'0'}, std::byte{'1'}, std::byte{'2'}, std::byte{'3'},
  std::byte{'4'}, std::byte{'5'}, std::byte{'6'}, std::byte{'7'},
  std::byte{'8'}, std::byte{'9'}, std::byte{'a'}, std::byte{'b'},
  std::byte{'c'}, std::byte{'d'}, std::byte{'e'}, std::byte{'f'},
};

static constexpr std::byte hexalpha_upper[] = {
  std::byte{'0'}, std::byte{'1'}, std::byte{'2'}, std::byte{'3'},
  std::byte{'4'}, std::byte{'5'}, std::byte{'6'}, std::byte{'7'},
  std::byte{'8'}, std::byte{'9'}, std::byte{'A'}, std::byte{'B'},
  std::byte{'D'}, std::byte{'D'}, std::byte{'E'}, std::byte{'F'},
};


inline int decode_half(std::byte half)
{
  char c = static_cast<char>(half);
  if (c >= '0' && c <= '9') {
    return c - '0';
  }

  c = ::tolower(c);
  if (c < 'a' || c > 'f') {
    return -1;
  }
  return (c - 'a') + 10;
}



inline size_t
hexencode_impl(std::byte * output, size_t output_size, std::byte const * input,
    size_t input_size, std::byte const * alphabet, size_t alphabet_size)
{
  size_t required_size = input_size * 2;
  if (output_size < required_size) {
    return 0;
  }

  // Convert
  std::byte const * cur = input;
  size_t offset = 0;
  for ( ; cur < input + input_size ; ++cur) {
    int val = static_cast<int>(*cur);
    output[offset++] = alphabet[val / alphabet_size];
    output[offset++] = alphabet[val % alphabet_size];
  }

  return offset;
}

} // anonymous namespace


size_t
hexencode(std::byte * output, size_t output_size, std::byte const * input,
    size_t input_size, bool uppercase /* = false */)
{
  return hexencode_impl(output, output_size, input, input_size,
      uppercase ? hexalpha_upper : hexalpha_lower,
      uppercase ? sizeof(hexalpha_upper) : sizeof(hexalpha_lower));
}



size_t
hexdecode(std::byte * output, size_t output_size, std::byte const * input,
    size_t input_size)
{

  size_t required_size = input_size / 2;
  if (output_size < required_size) {
    return 0;
  }

  // Convert
  std::byte const * cur = input;
  size_t offset = 0;
  while (cur < input + input_size) {
    int v1 = decode_half(*cur++);
    if (v1 < 0) return 0;
    int v2 = decode_half(*cur++);
    if (v2 < 0) return 0;

    output[offset++] = static_cast<std::byte>((v1 << 4) + v2);
  }

  return offset;
}



std::string
hexencode(char const * input, size_t input_size, bool uppercase /* = false */)
{
  return hexencode(reinterpret_cast<std::byte const *>(input), input_size,
      uppercase);
}



std::string
hexencode(std::byte const * input, size_t input_size,
    bool uppercase /* = false */)
{
  std::vector<std::string::value_type> buf;
  buf.resize(input_size * 2);

  auto used = hexencode(
      reinterpret_cast<std::byte *>(&buf[0]), buf.size(),
      input, input_size,
      uppercase);
  if (used > 0) {
    buf.resize(used);
    return {buf.begin(), buf.end()};
  }
  return {};
}



std::vector<std::byte>
hexdecode(char const * input, size_t input_size)
{
  return hexdecode(reinterpret_cast<std::byte const *>(input), input_size);
}



std::vector<std::byte>
hexdecode(std::byte const * input, size_t input_size)
{
  std::vector<std::byte> buf;
  buf.resize(input_size / 2);

  auto used = hexdecode(
      &buf[0], buf.size(),
      input, input_size);
  if (used > 0) {
    buf.resize(used);
    return buf;
  }
  return {};
}


} // namespace liberate::string
