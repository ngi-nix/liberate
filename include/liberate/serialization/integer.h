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
#ifndef LIBERATE_SERIALIZATION_INTEGER_H
#define LIBERATE_SERIALIZATION_INTEGER_H

#ifndef __cplusplus
#error You are trying to include a C++ only header file
#endif

#include <liberate.h>

#include <cstddef>
#include <type_traits>

#include <liberate/types/type_traits.h>

namespace liberate::serialization {

/**
 * Help erase qualifiers for the type, but also enable/disable the operations
 * for various types.
 */
template <typename T>
struct enable_integer_serialization
{};

template <typename T>
struct enable_integer_serialization<T const>
  : enable_integer_serialization<T>
{};

template <typename T>
struct enable_integer_serialization<T volatile>
  : enable_integer_serialization<T>
{};

template <typename T>
struct enable_integer_serialization<T const volatile>
  : enable_integer_serialization<T>
{};

template <typename T>
using integer_serialization_enabled = typename enable_integer_serialization<T>::type;

/**
 * Specialize the varind_operand for all integral types.
 */
template <> struct enable_integer_serialization<char> { using type = std::size_t; };
template <> struct enable_integer_serialization<signed char> { using type = std::size_t; };
template <> struct enable_integer_serialization<unsigned char> { using type = std::size_t; };
#if defined(LIBERATE_HAVE_WHCAR_T)
template <> struct enable_integer_serialization<wchar_t> { using type = std::size_t; };
#endif
#if defined(LIBERATE_HAVE_CHAR8_T)
template <> struct enable_integer_serialization<char8_t> { using type = std::size_t; };
#endif
template <> struct enable_integer_serialization<char16_t> { using type = std::size_t; };
template <> struct enable_integer_serialization<char32_t> { using type = std::size_t; };
template <> struct enable_integer_serialization<short> { using type = std::size_t; };
template <> struct enable_integer_serialization<unsigned short> { using type = std::size_t; };
template <> struct enable_integer_serialization<int> { using type = std::size_t; };
template <> struct enable_integer_serialization<unsigned int> { using type = std::size_t; };
template <> struct enable_integer_serialization<long> { using type = std::size_t; };
template <> struct enable_integer_serialization<unsigned long> { using type = std::size_t; };
template <> struct enable_integer_serialization<long long> { using type = std::size_t; };
template <> struct enable_integer_serialization<unsigned long long> { using type = std::size_t; };

namespace detail {

template <
  typename outT,
  typename inT
>
constexpr integer_serialization_enabled<inT>
serialize_int_impl(outT * output, std::size_t output_length, inT const & value)
{
  if (!output) {
    return 0;
  }

  constexpr std::size_t const input_size = sizeof(inT);
  constexpr std::size_t const out_unit_size = sizeof(outT);
  if (input_size > out_unit_size * output_length) {
    return 0;
  }

  std::size_t written = 0;
  for (std::size_t i = 0 ; i < input_size / out_unit_size ; ++i) {
    output[i] = static_cast<outT>(value >>
        ((input_size - ((i + 1) * out_unit_size)) * 8)
      );
    ++written;
  }

  return written;
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
 * Note: the function is restricted to 8-bit output buffer types only, though
 *       the detail implementation works for other values. The reason is that
 *       for actual cross-platform value serialization, only 8-bit buffers
 *       matter.
 */
template <
  typename outT,
  typename inT,
  std::enable_if_t<liberate::types::is_8bit_type<outT>::value, int> = 0
>
constexpr integer_serialization_enabled<inT>
serialize_int(outT * output, std::size_t output_length, inT const & value)
{
  return detail::serialize_int_impl(output, output_length, value);
}


} // namespace liberate::serialization

#endif // guard
