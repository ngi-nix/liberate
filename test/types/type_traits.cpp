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
#include <liberate/types/type_traits.h>

#include <gtest/gtest.h>

#include <cstddef>

TEST(TypesTraits, has_byte_size)
{
  using namespace liberate::types;

  // Spot checks.

  // 8-bit must be 1 byte and must not be 2 byte
  ASSERT_TRUE((has_byte_size<uint8_t, 1>::value));
  ASSERT_FALSE((has_byte_size<uint8_t, 2>::value));

  // 16-bit must be 2 byte and must not be 3 byte
  ASSERT_TRUE((has_byte_size<uint16_t, 2>::value));
  ASSERT_FALSE((has_byte_size<uint16_t, 3>::value));

  // 64 bit signed and unsigned must both be 8 byte
  ASSERT_TRUE((has_byte_size<uint64_t, 8>::value));
  ASSERT_TRUE((has_byte_size<int64_t, 8>::value));
}


TEST(TypesTraits, is_8bit_type)
{
  using namespace liberate::types;

  // True
  ASSERT_TRUE(is_8bit_type<uint8_t>::value);
  ASSERT_TRUE(is_8bit_type<int8_t>::value);
  ASSERT_TRUE(is_8bit_type<char>::value);
  ASSERT_TRUE(is_8bit_type<signed char>::value);
  ASSERT_TRUE(is_8bit_type<unsigned char>::value);
  ASSERT_TRUE(is_8bit_type<std::byte>::value);

  ASSERT_FALSE(is_8bit_type<uint16_t>::value);
}


TEST(TypesTraits, is_16bit_type)
{
  using namespace liberate::types;

  // True
  ASSERT_TRUE(is_16bit_type<uint16_t>::value);
  ASSERT_TRUE(is_16bit_type<int16_t>::value);
  ASSERT_TRUE(is_16bit_type<char16_t>::value);

  ASSERT_FALSE(is_16bit_type<uint8_t>::value);
}


TEST(TypesTraits, is_32bit_type)
{
  using namespace liberate::types;

  // True
  ASSERT_TRUE(is_32bit_type<uint32_t>::value);
  ASSERT_TRUE(is_32bit_type<int32_t>::value);
  ASSERT_TRUE(is_32bit_type<char32_t>::value);

  ASSERT_FALSE(is_32bit_type<uint8_t>::value);
}


TEST(TypesTraits, is_64bit_type)
{
  using namespace liberate::types;

  // True
  ASSERT_TRUE(is_64bit_type<uint64_t>::value);
  ASSERT_TRUE(is_64bit_type<int64_t>::value);

  ASSERT_FALSE(is_64bit_type<uint8_t>::value);
}
