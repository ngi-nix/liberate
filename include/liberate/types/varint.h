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
#ifndef LIBERATE_TYPES_VARINT_H
#define LIBERATE_TYPES_VARINT_H

#ifndef __cplusplus
#error You are trying to include a C++ only header file
#endif

#include <liberate.h>

#include <ostream>

namespace liberate::types {

/**
 * The varint type is an alias for its base type, but unlike a typedef or using
 * declaration, it creates a separate type for which templates can be
 * specialized.
 *
 * It's named varint, because other parts of liberate define specialized, variable
 * length encodings for this type. In itself, it should behave very much like it's
 * base type, varint_base.
 *
 * What is *not* defined are automatic conversions, which include conversions
 * to and from bool. So this will fail:
 *
 * varint foo = 32;
 * if (foo) ...
 * if (!foo) ...
 *
 * Convenience functions to_varint() and from_varint() are provided:
 *
 * varint foo = to_varint(32);
 * if (from_varint(foo)) ...
 * if (!from_varint(foo)) ...
 */
using varint_base = int64_t;
enum class varint : varint_base {};

/**
 * Help erase qualifiers on the operand type(s).
 */
template <typename T>
struct varint_operand
{};

template <typename T>
struct varint_operand<T const>
  : varint_operand<T>
{};

template <typename T>
struct varint_operand<T volatile>
  : varint_operand<T>
{};

template <typename T>
struct varint_operand<T const volatile>
  : varint_operand<T>
{};



template <typename T>
using varint_type = typename varint_operand<T>::type;


/**
 * Define operations; arithmetic operations.
 */

// Mathematical
constexpr varint
operator+(varint const & v) noexcept
{
  return varint{+static_cast<varint_base>(v)};
}

constexpr varint
operator-(varint const & v) noexcept
{
  return varint{-static_cast<varint_base>(v)};
}

template <typename T>
constexpr varint_type<T>
operator+(varint const & v, T const & t) noexcept
{
  return varint{static_cast<varint_base>(v) + t};
}

constexpr varint
operator+(varint const & v1, varint const & v2) noexcept
{
  return varint{static_cast<varint_base>(v1) + static_cast<varint_base>(v2)};
}

template <typename T>
constexpr varint_type<T>
operator-(varint const & v, T const & t) noexcept
{
  return varint{static_cast<varint_base>(v) - t};
}

constexpr varint
operator-(varint const & v1, varint const & v2) noexcept
{
  return varint{static_cast<varint_base>(v1) - static_cast<varint_base>(v2)};
}

template <typename T>
constexpr varint_type<T>
operator*(varint const & v, T const & t) noexcept
{
  return varint{static_cast<varint_base>(v) * t};
}

constexpr varint
operator*(varint const & v1, varint const & v2) noexcept
{
  return varint{static_cast<varint_base>(v1) * static_cast<varint_base>(v2)};
}

template <typename T>
constexpr varint_type<T>
operator/(varint const & v, T const & t) noexcept
{
  return varint{static_cast<varint_base>(v) / t};
}

constexpr varint
operator/(varint const & v1, varint const & v2) noexcept
{
  return varint{static_cast<varint_base>(v1) / static_cast<varint_base>(v2)};
}

template <typename T>
constexpr varint_type<T>
operator%(varint const & v, T const & t) noexcept
{
  return varint{static_cast<varint_base>(v) % t};
}

constexpr varint
operator%(varint const & v1, varint const & v2) noexcept
{
  return varint{static_cast<varint_base>(v1) % static_cast<varint_base>(v2)};
}


// Mathematical with assignment
template <typename T>
constexpr varint_type<T> &
operator+=(varint & v, T const & t) noexcept
{
  return v = varint{static_cast<varint_base>(v) + t};
}

constexpr varint &
operator+=(varint & v1, varint const & v2) noexcept
{
  return v1 = varint{static_cast<varint_base>(v1) + static_cast<varint_base>(v2)};
}

template <typename T>
constexpr varint_type<T> &
operator-=(varint & v, T const & t) noexcept
{
  return v = varint{static_cast<varint_base>(v) - t};
}

constexpr varint &
operator-=(varint & v1, varint const & v2) noexcept
{
  return v1 = varint{static_cast<varint_base>(v1) - static_cast<varint_base>(v2)};
}

template <typename T>
constexpr varint_type<T> &
operator*=(varint & v, T const & t) noexcept
{
  return v = varint{static_cast<varint_base>(v) * t};
}

constexpr varint &
operator*=(varint & v1, varint const & v2) noexcept
{
  return v1 = varint{static_cast<varint_base>(v1) * static_cast<varint_base>(v2)};
}

template <typename T>
constexpr varint_type<T> &
operator/=(varint & v, T const & t) noexcept
{
  return v = varint{static_cast<varint_base>(v) / t};
}

constexpr varint &
operator/=(varint & v1, varint const & v2) noexcept
{
  return v1 = varint{static_cast<varint_base>(v1) / static_cast<varint_base>(v2)};
}

template <typename T>
constexpr varint_type<T> &
operator%=(varint & v, T const & t) noexcept
{
  return v = varint{static_cast<varint_base>(v) % t};
}

constexpr varint &
operator%=(varint & v1, varint const & v2) noexcept
{
  return v1 = varint{static_cast<varint_base>(v1) % static_cast<varint_base>(v2)};
}


// Bitwise
constexpr varint
operator~(varint const & v) noexcept
{
  return varint{~static_cast<varint_base>(v)};
}

template <typename T>
constexpr varint_type<T>
operator&(varint const & v, T const & t) noexcept
{
  return varint{static_cast<varint_base>(v) & t};
}

constexpr varint
operator&(varint const & v1, varint const & v2) noexcept
{
  return varint{static_cast<varint_base>(v1) & static_cast<varint_base>(v2)};
}

template <typename T>
constexpr varint_type<T>
operator|(varint const & v, T const & t) noexcept
{
  return varint{static_cast<varint_base>(v) | t};
}

constexpr varint
operator|(varint const & v1, varint const & v2) noexcept
{
  return varint{static_cast<varint_base>(v1) | static_cast<varint_base>(v2)};
}

template <typename T>
constexpr varint_type<T>
operator^(varint const & v, T const & t) noexcept
{
  return varint{static_cast<varint_base>(v) ^ t};
}

constexpr varint
operator^(varint const & v1, varint const & v2) noexcept
{
  return varint{static_cast<varint_base>(v1) ^ static_cast<varint_base>(v2)};
}

template <typename T>
constexpr varint_type<T>
operator<<(varint const & v, T const & t) noexcept
{
  return varint{static_cast<varint_base>(v) << t};
}

constexpr varint
operator<<(varint const & v1, varint const & v2) noexcept
{
  return varint{static_cast<varint_base>(v1) << static_cast<varint_base>(v2)};
}

template <typename T>
constexpr varint_type<T>
operator>>(varint const & v, T const & t) noexcept
{
  return varint{static_cast<varint_base>(v) >> t};
}

constexpr varint
operator>>(varint const & v1, varint const & v2) noexcept
{
  return varint{static_cast<varint_base>(v1) >> static_cast<varint_base>(v2)};
}


// Bitwise with assignment
template <typename T>
constexpr varint_type<T> &
operator&=(varint & v, T const & t) noexcept
{
  return v = varint{static_cast<varint_base>(v) & t};
}

constexpr varint &
operator&=(varint & v1, varint const & v2) noexcept
{
  return v1 = varint{static_cast<varint_base>(v1) & static_cast<varint_base>(v2)};
}

template <typename T>
constexpr varint_type<T> &
operator|=(varint & v, T const & t) noexcept
{
  return v = varint{static_cast<varint_base>(v) | t};
}

constexpr varint &
operator|=(varint & v1, varint const & v2) noexcept
{
  return v1 = varint{static_cast<varint_base>(v1) | static_cast<varint_base>(v2)};
}

template <typename T>
constexpr varint_type<T> &
operator^=(varint & v, T const & t) noexcept
{
  return v = varint{static_cast<varint_base>(v) ^ t};
}

constexpr varint &
operator^=(varint & v1, varint const & v2) noexcept
{
  return v1 = varint{static_cast<varint_base>(v1) ^ static_cast<varint_base>(v2)};
}

template <typename T>
constexpr varint_type<T> &
operator<<=(varint & v, T const & t) noexcept
{
  return v = varint{static_cast<varint_base>(v) << t};
}

constexpr varint &
operator<<=(varint & v1, varint const & v2) noexcept
{
  return v1 = varint{static_cast<varint_base>(v1) << static_cast<varint_base>(v2)};
}

template <typename T>
constexpr varint_type<T> &
operator>>=(varint & v, T const & t) noexcept
{
  return v = varint{static_cast<varint_base>(v) >> t};
}

constexpr varint &
operator>>=(varint & v1, varint const & v2) noexcept
{
  return v1 = varint{static_cast<varint_base>(v1) >> static_cast<varint_base>(v2)};
}


// Comparison operators
template <typename T>
constexpr bool
operator==(varint const & v, T const & o) noexcept
{
  return static_cast<varint_base>(v) == o;
}

template <typename T>
constexpr bool
operator!=(varint const & v, T const & o) noexcept
{
  return static_cast<varint_base>(v) != o;
}

template <typename T>
constexpr bool
operator<(varint const & v, T const & o) noexcept
{
  return static_cast<varint_base>(v) < o;
}

template <typename T>
constexpr bool
operator>(varint const & v, T const & o) noexcept
{
  return static_cast<varint_base>(v) > o;
}

template <typename T>
constexpr bool
operator<=(varint const & v, T const & o) noexcept
{
  return static_cast<varint_base>(v) < o;
}

template <typename T>
constexpr bool
operator>=(varint const & v, T const & o) noexcept
{
  return static_cast<varint_base>(v) > o;
}


/**
 * Specialize the varind_operand for all integral types.
 */
template <> struct varint_operand<bool> { using type = varint; };
template <> struct varint_operand<char> { using type = varint; };
template <> struct varint_operand<signed char> { using type = varint; };
template <> struct varint_operand<unsigned char> { using type = varint; };
#if defined(LIBERATE_HAVE_WHCAR_T)
template <> struct varint_operand<wchar_t> { using type = varint; };
#endif
#if defined(LIBERATE_HAVE_CHAR8_T)
template <> struct varint_operand<char8_t> { using type = varint; };
#endif
template <> struct varint_operand<char16_t> { using type = varint; };
template <> struct varint_operand<char32_t> { using type = varint; };
template <> struct varint_operand<short> { using type = varint; };
template <> struct varint_operand<unsigned short> { using type = varint; };
template <> struct varint_operand<int> { using type = varint; };
template <> struct varint_operand<unsigned int> { using type = varint; };
template <> struct varint_operand<long> { using type = varint; };
template <> struct varint_operand<unsigned long> { using type = varint; };
template <> struct varint_operand<long long> { using type = varint; };
template <> struct varint_operand<unsigned long long> { using type = varint; };


/**
 * Simpler conversion (instead of cast)
 */

template <typename T>
constexpr varint_type<T>
to_varint(T const & t)
{
  return varint{static_cast<varint_base>(t)};
}

constexpr varint_base
from_varint(varint const & v)
{
  return static_cast<varint_base>(v);
}


/**
 * Output operator
 */

inline
std::ostream &
operator<<(std::ostream & os, varint const & v)
{
  os << static_cast<varint_base>(v);
  return os;
}

namespace literals {

constexpr varint operator"" _var(unsigned long long val) noexcept
{
  return varint{static_cast<varint_base>(val)};
}

} // namespace literals

} // namespace liberate::types

namespace std {

/**
 * Specialize hash
 */
template <>
struct hash<liberate::types::varint>
{
  std::size_t operator()(liberate::types::varint const & v) const noexcept
  {
    std::hash<liberate::types::varint_base> hasher;
    return hasher(static_cast<liberate::types::varint_base>(v));
  }
};

} // namespace std

#endif //guard
