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

#include <liberate/net/resolve.h>

#include <gtest/gtest.h>

#include "../test_name.h"
#include "../env.h"


#define INTERPEER_IO_V4 "88.99.83.249"
#define INTERPEER_IO_V6 "2a01:4f8:c17:85e4::1"

namespace net = liberate::net;

TEST(Resolve, localhost_any)
{
  // The difficulty here is that we may have one or two results,
  // depending on how the test host is connected. Still, that's not
  // actually bad.
  // We could also have zero in an entirely non-networked situation.
  auto result = net::resolve(test_env->api, net::AT_UNSPEC, "localhost");
  if (result.empty()) {
    GTEST_SKIP() << "Perhaps the test host has no network?";
  }
  ASSERT_TRUE(result.size() == 1 || result.size() == 2);

  for (auto addr : result) {
    if (addr.type() == net::AT_INET4) {
      ASSERT_EQ(addr.cidr_str(), "127.0.0.1");
    }
    else if (addr.type() == net::AT_INET6) {
      ASSERT_EQ(addr.cidr_str(), "::1");
    }
  }
}


TEST(Resolve, localhost_ipv4_with_port)
{
  auto result = net::resolve(test_env->api, net::AT_INET4, "localhost:1234");
  ASSERT_EQ(result.size(), 1);
  auto addr = *result.begin();
  ASSERT_EQ(addr.type(), net::AT_INET4);
  ASSERT_EQ(1234, addr.port());
  ASSERT_EQ(addr.cidr_str(), "127.0.0.1");
}


TEST(Resolve, test_domain_error)
{
  // As per specs, domains ending in .test should never be resolvable.
  auto result = net::resolve(test_env->api, net::AT_UNSPEC, "resolve.test");
  ASSERT_TRUE(result.empty());
}


TEST(Resolve, interpeer_any)
{
  // If the interpeer configuration does not change, this should yield IPv4
  // and IPv6 addresses
  auto result = net::resolve(test_env->api, net::AT_UNSPEC, "interpeer.io");
  if (result.empty()) {
    GTEST_SKIP() << "Perhaps the test host has no network?";
  }
  for (auto addr : result) {
    if (addr.type() == net::AT_INET4) {
      ASSERT_EQ(addr.cidr_str(), INTERPEER_IO_V4);
    }
    else if (addr.type() == net::AT_INET6) {
      ASSERT_EQ(addr.cidr_str(), INTERPEER_IO_V6);
    }
  }
}

TEST(Resolve, interpeer_ipv4)
{
  auto result = net::resolve(test_env->api, net::AT_INET4, "interpeer.io");
  if (result.empty()) {
    GTEST_SKIP() << "Perhaps the test host has no network?";
  }
  ASSERT_EQ(result.size(), 1);
  auto addr = *result.begin();
  ASSERT_EQ(addr.type(), net::AT_INET4);
  ASSERT_EQ(addr.cidr_str(), INTERPEER_IO_V4);
}

TEST(Resolve, interpeer_ipv6)
{
  auto result = net::resolve(test_env->api, net::AT_INET6, "interpeer.io");
  if (result.empty()) {
    GTEST_SKIP() << "Perhaps the test host has no network?";
  }

  ASSERT_EQ(result.size(), 1);
  auto addr = *result.begin();
  ASSERT_EQ(addr.type(), net::AT_INET6);
  ASSERT_EQ(addr.cidr_str(), INTERPEER_IO_V6);
}
