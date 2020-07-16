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
#include <liberate/types/varint.h>

#include <gtest/gtest.h>

template <typename T>
class VarintTest: public ::testing::Test {};
TYPED_TEST_CASE_P(VarintTest);

TYPED_TEST_P(VarintTest, instanciate)
{
  using namespace liberate::types;
  auto x = static_cast<varint>(TypeParam{1});
  ASSERT_EQ(x, static_cast<varint_base>(1));
}

TYPED_TEST_P(VarintTest, negate)
{
  using namespace liberate::types;
  auto x = static_cast<varint>(TypeParam{1});
  auto y = -x;
  ASSERT_EQ(y, static_cast<varint_base>(-1));
}


TYPED_TEST_P(VarintTest, add)
{
  using namespace liberate::types;
  auto x = static_cast<varint>(TypeParam{1});
  x = x + 1;
  ASSERT_EQ(x, static_cast<varint_base>(2));
}

TYPED_TEST_P(VarintTest, subtract)
{
  using namespace liberate::types;
  auto x = static_cast<varint>(TypeParam{1});
  x = x - 1;
  ASSERT_EQ(x, static_cast<varint_base>(0));
}

TYPED_TEST_P(VarintTest, multiply)
{
  using namespace liberate::types;
  auto x = static_cast<varint>(TypeParam{1});
  x = x * 2;
  ASSERT_EQ(x, static_cast<varint_base>(2));
}

TYPED_TEST_P(VarintTest, divide)
{
  using namespace liberate::types;
  auto x = static_cast<varint>(TypeParam{2});
  x = x / 2;
  ASSERT_EQ(x, static_cast<varint_base>(1));
}

TYPED_TEST_P(VarintTest, modulo)
{
  using namespace liberate::types;
  auto x = static_cast<varint>(TypeParam{3});
  x = x % 2;
  ASSERT_EQ(x, static_cast<varint_base>(1));
}


TYPED_TEST_P(VarintTest, add_assign)
{
  using namespace liberate::types;
  auto x = static_cast<varint>(TypeParam{1});
  x += 1;
  ASSERT_EQ(x, static_cast<varint_base>(2));
}

TYPED_TEST_P(VarintTest, subtract_assign)
{
  using namespace liberate::types;
  auto x = static_cast<varint>(TypeParam{1});
  x -= 1;
  ASSERT_EQ(x, static_cast<varint_base>(0));
}

TYPED_TEST_P(VarintTest, multiply_assign)
{
  using namespace liberate::types;
  auto x = static_cast<varint>(TypeParam{1});
  x *= 2;
  ASSERT_EQ(x, static_cast<varint_base>(2));
}

TYPED_TEST_P(VarintTest, divide_assign)
{
  using namespace liberate::types;
  auto x = static_cast<varint>(TypeParam{2});
  x /= 2;
  ASSERT_EQ(x, static_cast<varint_base>(1));
}

TYPED_TEST_P(VarintTest, modulo_assign)
{
  using namespace liberate::types;
  auto x = static_cast<varint>(TypeParam{3});
  x %= 2;
  ASSERT_EQ(x, static_cast<varint_base>(1));
}


REGISTER_TYPED_TEST_CASE_P(VarintTest,
    instanciate,
    negate,
    add,
    subtract,
    multiply,
    divide,
    modulo,
    add_assign,
    subtract_assign,
    multiply_assign,
    divide_assign,
    modulo_assign
    // TODO
    // - bitwise operators
    // - comparison operators
    // - stream
);


typedef ::testing::Types<
// FIXME treat special  bool,
  char,
  signed char,
  unsigned char,
#if defined(LIBERATE_HAVE_WHCAR_T)
  wchar_t,
#endif
#if defined(LIBERATE_HAVE_CHAR8_T)
  char8_t,
#endif
  char16_t,
  char32_t,
  short,
  unsigned short,
  int,
  unsigned int,
  long,
  unsigned long,
  long long,
  unsigned long long,
  liberate::types::varint
> test_types;
INSTANTIATE_TYPED_TEST_SUITE_P(types, VarintTest, test_types);


TEST(TypesVarintTest, literals)
{
  using namespace liberate::types::literals;
  auto x = 42_var;
  ASSERT_EQ(x, 42);
}
