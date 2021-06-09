/**
 * This file is part of liberate.
 *
 * Author(s): Jens Finkhaeuser <jens@finkhaeuser.de>
 *
 * Copyright (c) 2014 Unwesen Ltd.
 * Copyright (c) 2015-2021 Jens Finkhaeuser.
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

#include <gtest/gtest.h>

#include <liberate/concurrency/tasklet.h>

#include <thread>
#include <cstdlib>

#include "../compare_times.h"

#define THREAD_TEST_SHORT_DELAY std::chrono::milliseconds(1)
#define THREAD_TEST_LONG_DELAY  std::chrono::milliseconds(100)

namespace lc = liberate::concurrency;

namespace {

static bool done = false;

void sleeper(lc::tasklet::context & t)
{
  // Sleep until woken up.
  while (t.sleep()) {
    // tum-tee-tum.
  }
  done = true;
}

void sleep_halfsec(lc::tasklet::context & t)
{
  t.sleep(std::chrono::milliseconds(500));
  done = true;
}

static int count = 0;

void counter(lc::tasklet::context & t)
{
  while (t.sleep()) {
    ++count;
  }
}

struct bind_test
{
  bool finished;

  bind_test()
    : finished(false)
  {
  }

  void sleep_member(lc::tasklet::context & t)
  {
    // Sleep until woken up.
    while (t.sleep()) {
      // tum-tee-tum.
    }
    finished = true;
  }
};


} // anonymous namespace


TEST(Tasklet, sleep_zero_msec)
{
  // Check whether sleep() correctly handles 0 msecs (no sleeping)
  done = false;
  lc::tasklet task(sleeper);

  auto t1 = std::chrono::steady_clock::now();
  ASSERT_TRUE(task.start());
  std::this_thread::sleep_for(THREAD_TEST_LONG_DELAY);
  ASSERT_TRUE(task.stop());
  task.wait();
  auto t2 = std::chrono::steady_clock::now();

  // The time difference must always be lower than the 1000 msecs
  // (1000000 nsecs) we specified as the sleep time.
  auto diff = t2 - t1;
  ASSERT_GT(diff, std::chrono::nanoseconds::zero());
  ASSERT_LT((t2 - t1).count(), 1'000'000'000);
}



TEST(Tasklet, sleep_some)
{
  // Same test, but with a half-second sleep. Now the elapsed time must
  // be larger than half a second.
  done = false;
  lc::tasklet task(sleep_halfsec);

  auto t1 = std::chrono::steady_clock::now();
  ASSERT_TRUE(task.start());
  task.wait();
  auto t2 = std::chrono::steady_clock::now();

  // The time difference must be very close to the sleep time of
  // 500msec.
  compare_times(t1, t2, std::chrono::milliseconds(500));
}



TEST(Tasklet, sleep_count_wakeup)
{
  // Count how often the thread got woken. Since it sleeps indefinitely,
  // it should get woken exactly twice: once due to wakeup(), and once
  // due to stop(). The stop() one would not result in wake_count to be
  // incremented, though.
  count = 0;
  lc::tasklet task(counter);

  ASSERT_TRUE(task.start());

  // Wait until the thread is running. Otherwise, wakeup() won't be
  // able to do anything (unless the thread runs quickly).
  std::this_thread::sleep_for(THREAD_TEST_LONG_DELAY);

  task.wakeup();

  // Wait until the wakup is handled. There's a possibility for a race
  // in that it's possible the thread has gone to sleep once more by
  // the time we're trying to wait on the condition, in which case the
  // thread will not notify us and stay in its sleep state.
  std::this_thread::sleep_for(THREAD_TEST_LONG_DELAY);

  ASSERT_EQ(1, count);

  ASSERT_TRUE(task.stop());
  task.wait();
}



TEST(Tasklet, member_function)
{
  // Binding member functions is done much manually in packeteer.
  using namespace std::placeholders;
  bind_test test;
  lc::tasklet task(
      std::bind(&bind_test::sleep_member, &test, _1));

  auto t1 = std::chrono::steady_clock::now();
  ASSERT_TRUE(task.start());
  std::this_thread::sleep_for(THREAD_TEST_LONG_DELAY);
  ASSERT_TRUE(task.stop());
  task.wait();
  auto t2 = std::chrono::steady_clock::now();

  // The time difference must always be lower than the 1000 msecs
  // (1000000 nsecs) we specified as the sleep time.
  auto diff = t2 - t1;
  ASSERT_GT(diff, std::chrono::nanoseconds::zero());
  ASSERT_LT((t2 - t1).count(), 1'000'000'000);
}



TEST(Tasklet, lambda)
{
  lc::tasklet task([](lc::tasklet::context &) {
      std::this_thread::sleep_for(THREAD_TEST_SHORT_DELAY);
  });

  auto t1 = std::chrono::steady_clock::now();
  ASSERT_TRUE(task.start());
  std::this_thread::sleep_for(THREAD_TEST_LONG_DELAY);
  ASSERT_TRUE(task.stop());
  task.wait();
  auto t2 = std::chrono::steady_clock::now();

  // The time difference must always be lower than the 1000 msecs
  // (1000000 nsecs) we specified as the sleep time.
  auto diff = t2 - t1;
  ASSERT_GT(diff, std::chrono::nanoseconds::zero());
  ASSERT_LT((t2 - t1).count(), 1'000'000'000);
}



TEST(Tasklet, scoped_behaviour_unused)
{
  lc::tasklet task(sleeper);
}



TEST(Tasklet, scoped_behaviour_started_manually)
{
  lc::tasklet task(sleeper);
  task.start();
}



TEST(Tasklet, scoped_behaviour_started_automatically)
{
  lc::tasklet task(sleeper, true);
}



TEST(Tasklet, scoped_behaviour_stopped_manually)
{
  lc::tasklet task(sleeper);
  task.start();
  task.stop();
}



TEST(Tasklet, shared_condition_variable)
{
  count = 0;

  lc::tasklet::sleep_condition cond;

  lc::tasklet t1(counter, &cond, true);
  lc::tasklet t2(counter, &cond, true);

  std::this_thread::sleep_for(THREAD_TEST_LONG_DELAY);

  t1.wakeup(); // also wakes up t2

  std::this_thread::sleep_for(THREAD_TEST_LONG_DELAY);

  // Both get woken
  ASSERT_LE(2, count);
  EXPECT_EQ(2, count) << "may fail under resource starvation.";

  // One gets stopped by this
  t1.stop();
  std::this_thread::sleep_for(THREAD_TEST_LONG_DELAY);
  ASSERT_LE(3, count);
  EXPECT_EQ(3, count) << "may fail under resource starvation.";

  // Now both should be stopped.
  t2.stop();
}
