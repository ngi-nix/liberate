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
#include <liberate/random/unsafe_bits.h>

#include <gtest/gtest.h>

#include <set>

#define NUM_GENERATED 1000

template <typename T>
class RandomUnsafeBits: public ::testing::Test {};
TYPED_TEST_SUITE_P(RandomUnsafeBits);


TYPED_TEST_P(RandomUnsafeBits, generate_distribution)
{
  using namespace liberate::random;

  unsafe_bits<TypeParam> rng;

  // What are we testing with this rng? Well, if it's reasonably random, it
  // shouldn't produce the same result over and over. Technically, that's
  // still possible, just unlikely the more often we produce results.
  std::set<TypeParam> results;
  for (size_t i = 0 ; i < NUM_GENERATED ; ++i) {
    results.insert(rng.get());
  }
  ASSERT_GT(results.size(), 1);
}


TYPED_TEST_P(RandomUnsafeBits, generate_negative)
{
  using namespace liberate::random;

  if (std::is_unsigned<TypeParam>::value) {
    GTEST_SKIP() << "Only enabled for signed values.";
    return;
  }

  unsafe_bits<TypeParam> rng;

  // This test is much the same as the previous, except we're only counting
  // negative values - and this test is skipped for unsigned integers.
  std::set<TypeParam> results;
  for (size_t i = 0 ; i < NUM_GENERATED ; ++i) {
    auto res = rng.get();
    if (res < 0) {
      results.insert(res);
    }
  }
  ASSERT_GT(results.size(), 1);
}


REGISTER_TYPED_TEST_SUITE_P(RandomUnsafeBits,
    generate_distribution,
    generate_negative
);

typedef ::testing::Types<
  // Types that Visual Studio reports as the only defined types in
  // uniform_int_distribution: N4659 29.6.1.1 [rand.req.genl]/1e
  short,
  int,
  long,
  long long,
  unsigned short,
  unsigned int,
  unsigned long,
  unsigned long long,
  // Types we use
  char,
  uint8_t,
  int8_t,
  intptr_t
> test_types;
INSTANTIATE_TYPED_TEST_SUITE_P(types, RandomUnsafeBits, test_types);


