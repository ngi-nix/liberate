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
#include <liberate/cpp/hash.h>

#include <gtest/gtest.h>

TEST(CppHash, hash_combine)
{
  using namespace liberate::cpp;

  std::size_t seed = 42;
  hash_combine(seed, 123);
  ASSERT_NE(seed, 42);
  ASSERT_NE(seed, 123);
}


TEST(CppHash, multi_hash_is_hash_combine)
{
  using namespace liberate::cpp;

  std::hash<std::size_t> hasher;
  std::size_t seed = hasher(42);
  hash_combine(seed, hasher(123));

  auto result = multi_hash(std::size_t{42}, std::size_t{123});
  ASSERT_EQ(seed, result);
}


TEST(CppHash, multi_hash_single_int_is_std_hash)
{
  int value = 42;

  using namespace liberate::cpp;
  auto result = multi_hash(42);

  std::hash<int> hasher;
  auto stdresult = hasher(value);

  ASSERT_EQ(stdresult, result);
}


TEST(CppHash, multi_hash_single_str_is_std_hash)
{
  std::string value = "Hello, world!";

  using namespace liberate::cpp;
  auto result = multi_hash(value);

  std::hash<std::string> hasher;
  auto stdresult = hasher(value);

  ASSERT_EQ(stdresult, result);
}


namespace test {

struct foo
{
  int a;

  size_t hash() const
  {
    return a;
  }
};


} // namespace test

LIBERATE_MAKE_HASHABLE(test::foo)


TEST(CppHash, multi_hash_custom_type_direct)
{
  test::foo value{42};

  using namespace liberate::cpp;
  auto result = multi_hash(value);

  auto stdresult = value.hash();

  ASSERT_EQ(stdresult, result);
}


TEST(CppHash, multi_hash_custom_type_indirect)
{
  test::foo value{42};

  using namespace liberate::cpp;
  auto result = multi_hash(value);

  std::hash<test::foo> hasher;
  auto stdresult = hasher(value);

  ASSERT_EQ(stdresult, result);
}



TEST(CppHash, range_hash_iterators)
{
  std::string a{"hello"};
  std::string b{"world"};

  using namespace liberate::cpp;
  auto ha = range_hash(a.begin(), a.end());
  auto hb = range_hash(b.begin(), b.end());

  ASSERT_NE(ha, hb);

  std::string c{"hello"};
  auto hc = range_hash(c.begin(), c.end());
  ASSERT_EQ(ha, hc);
}


TEST(CppHash, range_hash_pointers)
{
  std::string a{"hello"};
  std::string b{"world"};

  using namespace liberate::cpp;
  char const * ptr = a.c_str();
  auto ha = range_hash(ptr, ptr + a.size());
  ptr = b.c_str();
  auto hb = range_hash(ptr, ptr + b.size());

  ASSERT_NE(ha, hb);

  std::string c{"hello"};
  ptr = c.c_str();
  auto hc = range_hash(ptr, ptr + c.size());
  ASSERT_EQ(ha, hc);
}
