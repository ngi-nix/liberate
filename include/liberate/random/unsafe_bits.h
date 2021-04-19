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
 * PARTICULAR PURPOSE.
 **/
#ifndef LIBERATE_RANDOM_UNSAFE_BITS_H
#define LIBERATE_RANDOM_UNSAFE_BITS_H

#ifndef __cplusplus
#error You are trying to include a C++ only header file
#endif

#include <liberate.h>

#include <random>
#include <chrono>
#include <limits>

namespace liberate::random {

/**
 * Return a random value of type T.
 *
 * The structure is called "unsafe_bits" because it does not produce random
 * bits suitable for cryptographic applications.
 */
template <typename T>
struct unsafe_bits
{
  std::default_random_engine generator;
  std::uniform_int_distribution<T> distribution;

  inline unsafe_bits()
    : generator{
        static_cast<std::default_random_engine::result_type>(
            std::chrono::system_clock::now().time_since_epoch().count()
        )
      }
    , distribution{
        std::numeric_limits<T>::min(),
        std::numeric_limits<T>::max()
      }
  {
  }

  inline T get()
  {
    return distribution(generator);
  }

  inline double get_factor()
  {
    return static_cast<long double>(get()) /
      (std::numeric_limits<T>::max() - std::numeric_limits<T>::min());
  }
};


} // namespace liberate::random

#endif // guard
