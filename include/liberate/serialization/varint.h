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
#ifndef LIBERATE_SERIALIZATION_VARINT_H
#define LIBERATE_SERIALIZATION_VARINT_H

#ifndef __cplusplus
#error You are trying to include a C++ only header file
#endif

#include <liberate.h>

#include <cstring>

#include <type_traits>

#include <liberate/types/type_traits.h>
#include <liberate/types/varint.h>

namespace liberate::serialization {

namespace detail {

/**
 * Unfortunately, std::ceil is not constexpr everywhere, so we use our own
 * version here.
 */
constexpr std::size_t ceil(double input)
{
  return (static_cast<double>(static_cast<std::size_t>(input)) == input)
    ? static_cast<std::size_t>(input)
    : static_cast<std::size_t>(input) + ((input > 0) ? 1 : 0);
}

} // namespace detail

/**
 * Serialize to buffer.
 *
 * The buffer is defined by a pointer and a size. The input is an
 * integer value for which serialize_int is explicitly enabled (see above).
 *
 * If the buffer is too small, zero is returned. Otherwise, the number of
 * units written to the buffer is.
 *
 * Note: the function is restricted to 8-bit output buffer types only.
 */
template <
  typename outT,
  std::enable_if_t<liberate::types::is_8bit_type<outT>::value, int> = 0
>
std::size_t
serialize_varint(outT * output, std::size_t output_length, ::liberate::types::varint const & value)
{
  if (!output || !output_length) {
    return 0;
  }

  constexpr std::size_t const input_size = sizeof(liberate::types::varint);

  auto input = static_cast<liberate::types::varint_base>(value);

  // We require a buffer of at least enough size to store the largest possible
  // value in 7 bits instead of 8.
  constexpr std::size_t const buf_size = detail::ceil(double{input_size * 8} / 7);
  outT buf[buf_size];

  // Start at the end
  ssize_t offset = sizeof(buf) - 1;
  buf[offset] = static_cast<outT>(input & 127);
  while (input >>= 7) {
    --offset;
    if (offset < 0) {
      // Ran out of room
      return 0;
    }
    buf[offset] = static_cast<outT>(128) | static_cast<outT>(--input & 127);
  }

  std::size_t written = sizeof(buf) - offset;

  // Unfortunately, we can only check whether the output buffer is large enough
  // at this point.
  if (written > output_length) {
    return 0;
  }

  // Copy over.
  ::memcpy(output, buf + offset, written);
  return written;
}


/**
 * Deserialize from buffer.
 *
 * If decoding did not work, zero is returned. Otherwise, the number of
 * units from the the buffer is.
 *
 * Note: the function is restricted to 8-bit output buffer types only.
 */
template <
  typename inT,
  std::enable_if_t<liberate::types::is_8bit_type<inT>::value, int> = 0
>
std::size_t
deserialize_varint(::liberate::types::varint & value, inT const * input, std::size_t input_length)
{
  using varint_base = liberate::types::varint_base;

  if (!input || !input_length) {
    return 0;
  }

  // Some things are simpler without C++ warnings. Ah, well, lots of casts
  // it is.
  using unsigned_base = std::make_unsigned<varint_base>::type;
  constexpr auto const mask = static_cast<varint_base>(
      std::numeric_limits<unsigned_base>::max() << ((sizeof(unsigned_base) * 8) - 7)
  );

  inT const * buf = input;
  inT c = *buf++;
  varint_base val = static_cast<varint_base>(c & static_cast<inT>(127));
  while ((c & static_cast<inT>(128)) > static_cast<inT>(0)) {
    val += 1;
    if (!val || (val & mask)) {
      // Overflow
      return 0;
    }
    c = *buf++;
    val = (val << 7) + static_cast<varint_base>((c & static_cast<inT>(127)));

    if (static_cast<std::size_t>(buf - input) >= input_length) {
      // Not done decoding, but the buffer ends
      return 0;
    }
  }

  value = liberate::types::varint{val};
  return (buf - input);
}


} // namespace liberate::serialization

#endif // guard
