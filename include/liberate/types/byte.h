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
#ifndef LIBERATE_TYPES_BYTE_H
#define LIBERATE_TYPES_BYTE_H

#ifndef __cplusplus
#error You are trying to include a C++ only header file
#endif

#include <liberate.h>

#include <cstddef>
#include <vector>

namespace liberate::types {

#if defined(LIBERATE_HAVE_STD_BYTE)
using byte = std::byte;
#else
using byte = uint8_t;
#endif // LIBRATE_HAVE_STD_BYTE

namespace literals {


/**
 * Integer and character literals are converted to bytes. Note that for longer
 * integer literals, they are truncated by the cast, effectively resulting in
 * (val % 256).
 *
 * String literals are converted to vectors of bytes.
 */
inline constexpr byte operator ""_b(unsigned long long arg) noexcept
{
  return static_cast<byte>(arg);
}


inline constexpr byte operator ""_b(char arg) noexcept
{
  return static_cast<byte>(arg);
}


inline std::vector<byte> operator ""_b(char const * str, std::size_t len) noexcept
{
  auto start = reinterpret_cast<byte const *>(str);
  return {start, start + len};
}


} // namespace literals

} // namespace liberate::types

#endif //guard
