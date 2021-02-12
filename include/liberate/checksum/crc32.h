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
#ifndef LIBERATE_CHECKSUM_CRC32_H
#define LIBERATE_CHECKSUM_CRC32_H

#ifndef __cplusplus
#error You are trying to include a C++ only header file
#endif

#include <liberate.h>

#include <numeric>

namespace liberate::checksum {

/**
 * Type holding a crc32 checksum, and initializer value.
 */
using crc32_checksum = std::uint_fast32_t;

// Clip the initalizer value to 32 bits.
constexpr crc32_checksum CRC32_MASK = ~crc32_checksum{0} & crc32_checksum{0xFFFFFFFFuL};
constexpr crc32_checksum CRC32_INITIALIZER = CRC32_MASK;

/**
 * Define polynomials for use in the algorithm. Note that these are *reversed*
 * polynomials from
 * https://en.wikipedia.org/wiki/Cyclic_redundancy_check#Polynomial_representations_of_cyclic_redundancy_checks
 * as the algorithm works with those.
 *
 * Define polynomials also with some useful aliases. There are probably more
 * useful aliases, but we don't need to confuse eveything.
 */
enum crc32_polynomials : crc32_checksum
{
  CRC32 = crc32_checksum{0xEDB88320uL},
  CRC32_ISO3309 = CRC32,
  CRC32_IEEE802_3 = CRC32,
  CRC32_GZIP = CRC32,
  CRC32_BZIP2 = CRC32,
  CRC32_POSIX = CRC32,

  CRC32C = crc32_checksum{0x82F63B78uL},
  CRC32_CASTAGNOLI = CRC32C,
  CRC32C_SCTP = CRC32C,
  CRC32C_SSE42 = CRC32C,

  CRC32K = crc32_checksum{0xEB31D82EuL},
  CRC32_KOOPMAN = CRC32K,

  CRC32K2 = crc32_checksum{0x992C1A4CuL},
  CRC32_KOOPMAN2 = CRC32K,

  CRC32Q = crc32_checksum{0xD5828281uL},
  CRC32_AIXM = CRC32Q,
};


namespace {

/**
 * Create CRC32 lookup table at compile time via constexpr.
 */
template <
  crc32_checksum POLYNOMIAL,
  size_t table_size = 256
>
struct crc32_table_generator
{
private:
  // Calculate iteration value based on current value and LSB.
  template <crc32_checksum VAL, bool flag>
  struct iter_value;

  template <crc32_checksum VAL>
  struct iter_value<VAL, true>
  {
    static constexpr crc32_checksum value = (VAL >> 1) ^ POLYNOMIAL;
  };

  template <crc32_checksum VAL>
  struct iter_value<VAL, false>
  {
    static constexpr crc32_checksum value = (VAL >> 1);
  };

  // Calculation for table elements; index is the table index, N is the
  // iteration value.
  template <uint8_t index, uint8_t N = 0>
  struct table_element
  {
    static constexpr bool lsb =
      static_cast<bool>(
          table_element<index, N + 1>::value & 0x01u
      );

    static constexpr crc32_checksum value = iter_value<
      table_element<index, N + 1>::value,
      lsb
    >::value;
  };

  template <uint8_t index>
  struct table_element<index, 7>
  {
    static constexpr bool lsb =
      static_cast<bool>(
          index & 0x01u
      );

    static constexpr crc32_checksum value = iter_value<
      index,
      lsb
    >::value;
  };

  // Calculation of table
  template <
    size_t N = table_size - 1,
    crc32_checksum ...Indices
  >
  struct table
  {
    static constexpr auto value = table<
      N - 1,
      table_element<N>::value,
      Indices...
    >::value;
  };

  template <
    crc32_checksum ...Indices
  >
  struct table<0, Indices...>
  {
    static constexpr std::array<crc32_checksum, sizeof...(Indices) + 1> value
      = {{ table_element<0>::value, Indices... }};
  };

public:

  // Final array calculation
  static constexpr std::array<crc32_checksum, table_size> value = table<>::value;
};


} // anonymous namespace

/**
 * Calculate a crc32 checksum, using a given polynomial.
 *
 * The function takes a range as an input, and should work with any 8-bit
 * inputs. An optional previous checksum value permits accumulating a final
 * checksum iteratively.
 */
template <
  crc32_checksum POLYNOMIAL,
  typename iterT
>
crc32_checksum
crc32(iterT begin, iterT end, crc32_checksum initial = CRC32_INITIALIZER)
{
  // Memoize lookup table
  static auto const table = crc32_table_generator<POLYNOMIAL>::value;

  auto init = initial == CRC32_INITIALIZER
    ? initial
    : ~initial & CRC32_MASK;

  // Calculate checksum
  return CRC32_MASK &
    ~std::accumulate(begin, end, init,
        [](crc32_checksum checksum, std::uint_fast8_t value) -> crc32_checksum
        {
          return table[(checksum ^ value) & 0xFFu] ^ (checksum >> 8);
        }
    );
}

} // namespace liberate::checksum

#endif // guard
