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
#include <liberate/cpp/operators.h>

#include <gtest/gtest.h>

struct foo : public liberate::cpp::operators<foo>
{
  int a;

  foo(int _a)
    : liberate::cpp::operators<foo>{}
    , a(_a)
  {
  }

  bool is_equal_to(foo const & other) const
  {
    return a == other.a;
  }

  bool is_less_than(foo const & other) const
  {
    return a < other.a;
  }
};



TEST(CppOperators, test_equality)
{
  foo f1{42};
  foo f2{42};

  ASSERT_EQ(f1.a, 42);
  ASSERT_EQ(f1, f2);
}


TEST(CppOperators, test_inequality)
{
  foo f1{41};
  foo f2{42};

  ASSERT_NE(f1, f2);
}


TEST(CppOperators, test_less_than)
{
  foo f1{41};
  foo f2{42};

  ASSERT_LT(f1, f2);
}


TEST(CppOperators, test_less_equal)
{
  foo f1{41};
  foo f2{42};
  foo f3{42};

  ASSERT_LE(f1, f2);
  ASSERT_LE(f2, f3);
}



TEST(CppOperators, test_greater_than)
{
  foo f1{43};
  foo f2{42};

  ASSERT_GT(f1, f2);
}


TEST(CppOperators, test_greater_equal)
{
  foo f1{43};
  foo f2{42};
  foo f3{42};

  ASSERT_GE(f1, f2);
  ASSERT_GE(f2, f3);
}
