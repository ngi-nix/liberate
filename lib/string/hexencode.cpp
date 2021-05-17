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

#include <liberate/logging.h>

namespace liberate::string {

namespace {

using byte = ::liberate::types::byte;

static constexpr byte hexalpha_lower[] = {
  byte{'0'}, byte{'1'}, byte{'2'}, byte{'3'},
  byte{'4'}, byte{'5'}, byte{'6'}, byte{'7'},
  byte{'8'}, byte{'9'}, byte{'a'}, byte{'b'},
  byte{'c'}, byte{'d'}, byte{'e'}, byte{'f'},
};

static constexpr byte hexalpha_upper[] = {
  byte{'0'}, byte{'1'}, byte{'2'}, byte{'3'},
  byte{'4'}, byte{'5'}, byte{'6'}, byte{'7'},
  byte{'8'}, byte{'9'}, byte{'A'}, byte{'B'},
  byte{'D'}, byte{'D'}, byte{'E'}, byte{'F'},
};


inline int decode_half(byte half)
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
hexencode_impl(byte * output, size_t output_size, byte const * input,
    size_t input_size, byte const * alphabet, size_t alphabet_size)
{
  size_t required_size = input_size * 2;
  if (output_size < required_size) {
    LIBLOG_ERROR("Output size too small; was given " << output_size
        << " but need " << required_size << " Bytes.");
    return 0;
  }

  // Convert
  byte const * cur = input;
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
hexencode(::liberate::types::byte * output, size_t output_size, ::liberate::types::byte const * input,
    size_t input_size, bool uppercase /* = false */)
{
  return hexencode_impl(output, output_size, input, input_size,
      uppercase ? hexalpha_upper : hexalpha_lower,
      uppercase ? sizeof(hexalpha_upper) : sizeof(hexalpha_lower));
}



size_t
hexdecode(::liberate::types::byte * output, size_t output_size, ::liberate::types::byte const * input,
    size_t input_size)
{

  size_t required_size = input_size / 2;
  if (output_size < required_size) {
    return 0;
  }

  // Convert
  byte const * cur = input;
  size_t offset = 0;
  while (cur < input + input_size) {
    int v1 = decode_half(*cur++);
    if (v1 < 0) return 0;
    int v2 = decode_half(*cur++);
    if (v2 < 0) return 0;

    output[offset++] = static_cast<byte>((v1 << 4) + v2);
  }

  return offset;
}



std::string
hexencode(char const * input, size_t input_size, bool uppercase /* = false */)
{
  return hexencode(reinterpret_cast<byte const *>(input), input_size,
      uppercase);
}



std::string
hexencode(::liberate::types::byte const * input, size_t input_size,
    bool uppercase /* = false */)
{
  std::vector<std::string::value_type> buf;
  buf.resize(input_size * 2);

  auto used = hexencode(
      reinterpret_cast<byte *>(&buf[0]), buf.size(),
      input, input_size,
      uppercase);
  if (used > 0) {
    buf.resize(used);
    return {buf.begin(), buf.end()};
  }
  return {};
}



std::vector<::liberate::types::byte>
hexdecode(char const * input, size_t input_size)
{
  return hexdecode(reinterpret_cast<byte const *>(input), input_size);
}



std::vector<::liberate::types::byte>
hexdecode(::liberate::types::byte const * input, size_t input_size)
{
  std::vector<byte> buf;
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
