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

#include <string.h>

#include <cmath>
#include <type_traits>
#include <iostream> // FIXME

#include <liberate/types/type_traits.h>
#include <liberate/types/varint.h>

namespace liberate::serialization {

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
  if (!output) {
    return 0;
  }

  constexpr std::size_t const input_size = sizeof(liberate::types::varint);

  auto input = static_cast<liberate::types::varint_base>(value);

  // We require a buffer of at least enough size to store the largest possible
  // value in 7 bits instead of 8.
  constexpr std::size_t const buf_size = std::ceil(double{input_size * 8} / 7);
  outT buf[buf_size];

  // Start at the end
  std::size_t offset = sizeof(buf) - 1;
  buf[offset] = static_cast<outT>(input & 127);
  while (input >>= 7) {
    buf[--offset] = static_cast<outT>(128) | static_cast<outT>(--input & 127);
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


} // namespace liberate::serialization

#endif // guard
