/**
 * This file is part of liberate.
 *
 * Author(s): Jens Finkhaeuser <jens@finkhaeuser.de>
 *
 * Copyright (c) 2014-2015 Unwesen Ltd.
 * Copyright (c) 2016-2020 Jens Finkhaeuser.
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

#ifndef LIBERATE_CPP_HASH_H
#define LIBERATE_CPP_HASH_H

#ifndef __cplusplus
#error You are trying to include a C++ only header file
#endif

#include <liberate.h>

#include <functional>
#include <cstddef>

/**
 * The macro injects a specialization for the given type into the std
 * namespace, delegating to the value's hash() function.
 */
#define LIBERATE_MAKE_HASHABLE(type) \
  namespace std { \
    template <> struct hash<type> { \
      std::size_t operator()(type const & t) const noexcept { \
        return t.hash(); \
      } \
    }; \
  }


namespace liberate::cpp {

/**
 * Combine two hash values.
 * Based on boost::hash_combine, which is based on
 * http://www.cs.rmit.edu.au/~jz/fulltext/jasist-tch.pdf
 **/
inline void
hash_combine(std::size_t & seed, std::size_t const & value)
{
  seed ^= value + 0x9e3779b9
    + (seed << 6) + (seed >> 2);
}


/**
 * Hash multiple values
 *
 * std::hash only exists from C++11 onwards.
 **/
template <typename T>
inline std::size_t
multi_hash(T const & t)
{
  return std::hash<T>()(t);
}

template <typename T0, typename... Ts>
inline std::size_t
multi_hash(T0 const & t0, Ts && ... ts)
{
  std::size_t seed = multi_hash(t0);
  if constexpr (0 == sizeof...(ts)) {
    return seed;
  }

  std::size_t remainder = multi_hash(std::forward<Ts>(ts)...);

  hash_combine(seed, remainder);
  return seed;
}


/**
 * Hash a range of T
 **/
template <typename iterT>
inline std::size_t
range_hash(iterT const & begin, iterT const & end)
{
  iterT cur = begin;
  std::size_t hash = 0;

  std::hash<typename std::iterator_traits<iterT>::value_type> hasher;
  for ( ; cur != end ; ++cur) {
    hash_combine(hash, hasher(*cur));
  }

  return hash;
}


} // namespace liberate::cpp

#endif // guard
