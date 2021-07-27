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
#ifndef LIBERATE_TIMEOUT_EXPONENTIAL_BACKOFF_H
#define LIBERATE_TIMEOUT_EXPONENTIAL_BACKOFF_H

#ifndef __cplusplus
#error You are trying to include a C++ only header file
#endif

#include <liberate.h>

#include <cmath>

#include <liberate/random/unsafe_bits.h>

namespace liberate::timeout {

/**
 * Implement exponential backoff.
 *
 * Given a number of collisions (failures), return some multiplier for a
 * backoff factor, defined as an integer between 0 and (2^c - 1).
 */
inline std::size_t
backoff_multiplier(std::size_t const & collisions)
{
  ::liberate::random::unsafe_bits<std::size_t> rng;
  std::size_t clamp = static_cast<std::size_t>(exp2(collisions)) - 1;
  auto rand = rng.get_factor();
  auto ret = static_cast<std::size_t>(std::nearbyint(rand * clamp));
  return ret;
}


/**
 * The backoff function is statically parametrized with the backoff factor.
 * This is to provide for a simpler function prototype that just requires
 * the number of collisions, and returns the actual backoff value.
 */
template <
  typename backoffT
>
inline backoffT
backoff(backoffT const & backoff, std::size_t const & collisions)
{
  return backoff * backoff_multiplier(collisions);
}

} // namespace liberate::timeout


#endif // guard
