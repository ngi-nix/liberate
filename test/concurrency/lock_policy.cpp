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
#include <liberate/concurrency/lock_policy.h>

#include <gtest/gtest.h>

#include <mutex>

namespace lc = liberate::concurrency;

template <typename T>
class LockPolicy: public ::testing::Test {};
TYPED_TEST_SUITE_P(LockPolicy);

TYPED_TEST_P(LockPolicy, basic_scoped_locking)
{
  using policy = TypeParam;

  // This test doesn't really do anything - but if it works, it not only
  // compiles but exits cleanly.
  typename policy::mutex_type mutex;

  // Lock it successively in two scopes.
  {
    typename policy::lock_type lock{mutex};
  }
  {
    typename policy::lock_type lock{mutex};
  }
}


TYPED_TEST_P(LockPolicy, raw_ext_proxy)
{
  using policy = liberate::concurrency::raw_ext_lock_policy<
    TypeParam
  >;

  // This test is almost the same as the above, but it requires
  // the proxied mutex type to also exist.
  typename policy::mutex_type::proxied_type mutex;
  typename policy::mutex_type proxy{&mutex};

  // Lock it successively in two scopes.
  {
    typename policy::lock_type lock{proxy};
  }
  {
    typename policy::lock_type lock{proxy};
  }

  // We should be able to clear the proxied object without issues
  proxy.clear_proxied();
  {
    typename policy::lock_type lock{proxy};
  }
}


TYPED_TEST_P(LockPolicy, std_ext_proxy)
{
  using policy = liberate::concurrency::std_ext_lock_policy<
    TypeParam
  >;

  // Note: the test should be identical to raw_ext_proxy except for how
  // the mutex is crated and passed to the proxy.

  // This test is almost the same as the above, but it requires
  // the proxied mutex type to also exist.
  auto mutex = std::make_shared<typename policy::mutex_type::proxied_type>();
  typename policy::mutex_type proxy{mutex};

  // Lock it successively in two scopes.
  {
    typename policy::lock_type lock{proxy};
  }
  {
    typename policy::lock_type lock{proxy};
  }

  // We should be able to clear the proxied object without issues
  proxy.clear_proxied();
  {
    typename policy::lock_type lock{proxy};
  }
}


TYPED_TEST_P(LockPolicy, ref_ext_proxy)
{
  using policy = liberate::concurrency::ref_ext_lock_policy<
    TypeParam
  >;

  typename policy::mutex_type::proxied_type mutex;
  typename policy::mutex_type proxy{mutex};

  // Lock it successively in two scopes.
  {
    typename policy::lock_type lock{proxy};
  }
  {
    typename policy::lock_type lock{proxy};
  }

  // XXX does not compile, not implemented for references.
  // proxy.clear_proxied();
}


REGISTER_TYPED_TEST_SUITE_P(LockPolicy,
    basic_scoped_locking,
    raw_ext_proxy,
    std_ext_proxy,
    ref_ext_proxy
);

typedef ::testing::Types<
  liberate::concurrency::null_lock_policy,
  liberate::concurrency::std_lock_policy<
    std::mutex, std::lock_guard<std::mutex>
  >,
  liberate::concurrency::std_lock_policy<
    std::mutex, std::unique_lock<std::mutex>
  >,
  liberate::concurrency::std_lock_policy<
    std::recursive_mutex, std::lock_guard<std::recursive_mutex>
  >,
  liberate::concurrency::std_lock_policy<
    std::recursive_mutex, std::unique_lock<std::recursive_mutex>
  >,
  liberate::concurrency::std_lock_policy<
    std::timed_mutex, std::lock_guard<std::timed_mutex>
  >,
  liberate::concurrency::std_lock_policy<
    std::timed_mutex, std::unique_lock<std::timed_mutex>
  >,
  liberate::concurrency::std_lock_policy<
    std::recursive_timed_mutex, std::lock_guard<std::recursive_timed_mutex>
  >,
  liberate::concurrency::std_lock_policy<
    std::recursive_timed_mutex, std::unique_lock<std::recursive_timed_mutex>
  >
> test_types;
INSTANTIATE_TYPED_TEST_SUITE_P(concurrency, LockPolicy, test_types);


