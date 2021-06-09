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
#ifndef LIBERATE_TYPES_TYPE_TRAITS_H
#define LIBERATE_TYPES_TYPE_TRAITS_H

#ifndef __cplusplus
#error You are trying to include a C++ only header file
#endif

#include <liberate.h>

#include <type_traits>

namespace liberate::types {

/**
 * An integral constant that is either equivalent to std::true_type or
 * std::false_type, depending on whether the byte size of the given type
 * is the given value.
 */
template <typename T, std::size_t BYTE_SIZE>
using has_byte_size = std::integral_constant<
  bool,
  sizeof(T) == BYTE_SIZE
>;


/**
 * Determine whether the type has 8, 16, 32, 64 or 128 bits in
 * size. There is not much use for other sizes at the moment.
 */
template <typename T>
struct is_8bit_type
  : public has_byte_size<
      typename std::remove_cv<T>::type,
      1
    >::type
{};

template <typename T>
struct is_16bit_type
  : public has_byte_size<
      typename std::remove_cv<T>::type,
      2
    >::type
{};

template <typename T>
struct is_32bit_type
  : public has_byte_size<
      typename std::remove_cv<T>::type,
      4
    >::type
{};

template <typename T>
struct is_64bit_type
  : public has_byte_size<
      typename std::remove_cv<T>::type,
      8
    >::type
{};

template <typename T>
struct is_128bit_type
  : public has_byte_size<
      typename std::remove_cv<T>::type,
      16
    >::type
{};


} // namespace liberate::types

#endif // guard
