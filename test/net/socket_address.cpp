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

#include <liberate/net/socket_address.h>

// For AF_UNSPEC, etc.
#include "../lib/net/netincludes.h"

#include <gtest/gtest.h>

#include <cstring>

#include <sstream>
#include <string>
#include <set>

#include "../value_tests.h"
#include "../test_name.h"

namespace net = liberate::net;

/*****************************************************************************
 * SocketAddressParsing
 */

namespace {

struct parsing_test_data
{
  int                 type;
  net::address_type  sa_type;
  std::string         address;
  std::string         expected;
  uint16_t            port;
  bool                is_any;
  bool                is_loopback;
} parsing_tests[] = {
  { AF_INET,  net::AT_INET4, "192.168.0.1", "192.168.0.1", 12344,
    false, false,
  },
  { AF_INET,  net::AT_INET4, "192.168.0.1", "192.168.0.1", 12345,
    false, false,
  },
  { AF_INET6, net::AT_INET6, "2001:0db8:85a3:0000:0000:8a2e:0370:7334",
    "2001:db8:85a3::8a2e:370:7334", 12345,
    false, false,
  },
  { AF_INET6, net::AT_INET6, "2001:db8:85a3:0:0:8a2e:370:7334",
    "2001:db8:85a3::8a2e:370:7334", 12345,
    false, false,
  },
  { AF_INET6, net::AT_INET6, "2001:db8:85a3::8a2e:370:7334",
    "2001:db8:85a3::8a2e:370:7334", 12344,
    false, false,
  },
  { AF_INET6, net::AT_INET6, "2001:db8:85a3::8a2e:370:7334",
    "2001:db8:85a3::8a2e:370:7334", 12345,
    false, false,
  },
  { AF_INET6, net::AT_INET6, "0:0:0:0:0:0:0:1", "::1", 12345,
    false, true,
  },
  { AF_INET6, net::AT_INET6, "::1", "::1", 12345,
    false, true,
  },
  { AF_INET6, net::AT_INET6, "0:0:0:0:0:0:0:0", "::", 12345,
    true, false,
  },
  { AF_INET6, net::AT_INET6, "::", "::", 12345,
    true, false,
  },
  { AF_INET, net::AT_INET4, "0.0.0.0", "0.0.0.0", 12345,
    true, false,
  },
  { AF_INET, net::AT_INET4, "127.0.0.1", "127.0.0.1", 12345,
    false, true,
  },
  { AF_UNIX,  net::AT_LOCAL, "/foo/bar", "/foo/bar", 0,
    false, false,
  },
  { AF_UNIX,  net::AT_LOCAL, "something else", "something else", 0,
    false, false,
  },
  { AF_UNIX,  net::AT_LOCAL, std::string{"\0abstract", 9}, std::string{"\0abstract", 9}, 0,
    false, false,
  },
  { AF_UNSPEC,  net::AT_UNSPEC, "", "", 0,
    false, false,
  },
};


inline std::string
full_expected(parsing_test_data const & td, uint16_t port = 0)
{
  std::stringstream s;

  if (net::AT_INET6 == td.sa_type) {
    s << "[";
  }

  s << td.expected;

  if (net::AT_INET6 == td.sa_type) {
    s << "]";
  }

  if (net::AT_INET4 == td.sa_type || net::AT_INET6 == td.sa_type) {
    s << ":" << port;
  }

  return s.str();
}


std::string generate_name_parsing(testing::TestParamInfo<parsing_test_data> const & info)
{
  using namespace net;

  std::string name{ std::to_string(info.param.type) };
  name += "_";
  name += std::to_string(info.param.sa_type);
  name += "_";
  name += info.param.address;

  if (AT_INET4 == info.param.sa_type || AT_INET6 == info.param.sa_type) {
    name += "_port_";
    name += std::to_string(info.param.port);
  }

  return symbolize_name(name);
}

net::socket_address create_address(parsing_test_data const & data)
{
  using namespace net;

  // IPv4
  if (AF_INET == data.type) {
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(data.port);
    inet_pton(AF_INET, data.address.c_str(), &addr.sin_addr);

    return socket_address{&addr, sizeof(addr)};
  }

  // IPv6
  if (AF_INET6 == data.type) {
    sockaddr_in6 addr;
    addr.sin6_family = AF_INET6;
    addr.sin6_port = htons(data.port);
    inet_pton(AF_INET6, data.address.c_str(), &addr.sin6_addr);

    return socket_address{&addr, sizeof(addr)};
  }

  // Pipes
  if (AF_UNIX == data.type) {
    sockaddr_un addr;
    addr.sun_family = AF_UNIX;
    ::memset(&addr.sun_path, 0, UNIX_PATH_MAX);
    ::memcpy(addr.sun_path, data.address.c_str(), data.address.size() + 1);

    return socket_address{&addr, sizeof(addr)};
  }

  // Unspec
  return socket_address{};
}


inline void
test_min_serialization(liberate::net::socket_address const & addr, bool with_type, bool with_port)
{
  using namespace liberate::net;
  auto expected = addr;
  if (!with_port) {
    expected.set_port(0);
  }

  // Serialize to minimal buffer
  auto required = addr.min_bufsize(with_type, with_port);
  if (!required) {
    GTEST_SKIP();
    return;
  }

  std::vector<char> buf;
  buf.resize(required);

  // Serialization to a too small buffer must fail.
  ASSERT_EQ(0, addr.serialize(&buf[0], required - 1, with_type, with_port));

  // Serialization to the full buffer should work, and return the required size.
  auto consumed = addr.serialize(&buf[0], required, with_type, with_port);
  ASSERT_EQ(required, consumed);

  // Now deserialize. If we had a type, we can deserialize the whole buffer and
  // detect the type. In either case, we can specify the type and deserialize
  // the rest.
  if (with_type) {
    // Failure first
    auto [success, result] = socket_address::deserialize(
        buf.data(), buf.size() - 1,
        with_port);
    ASSERT_FALSE(success);
    ASSERT_EQ(AT_UNSPEC, result.type());

    // Success
    std::tie(success, result) = socket_address::deserialize(
        buf.data(), buf.size(),
        with_port);
    ASSERT_TRUE(success);
    ASSERT_EQ(expected, result);

    // Success with type
    std::tie(success, result) = socket_address::deserialize(addr.type(),
        buf.data() + 1, buf.size() - 1,
        with_port);
    ASSERT_TRUE(success);
    ASSERT_EQ(expected, result);
  }
  else {
    // Failure first
    auto [success, result] = socket_address::deserialize(addr.type(),
        buf.data(), buf.size() - 1,
        with_port);
    ASSERT_FALSE(success);
    ASSERT_EQ(AT_UNSPEC, result.type());

    // Success - with type
    std::tie(success, result) = socket_address::deserialize(addr.type(),
        buf.data(), buf.size(),
        with_port);
    ASSERT_TRUE(success);
    ASSERT_EQ(expected, result);
  }
}

} // anonymous namespace

class SocketAddressParsing
  : public testing::TestWithParam<parsing_test_data>
{
};


TEST_P(SocketAddressParsing, verify_CIDR)
{
  // Tests that the verify_cidr() function works as expected.
  using namespace net;
  auto td = GetParam();
  if (AT_LOCAL == td.sa_type || AT_UNSPEC == td.sa_type) {
    GTEST_SKIP();
    return;
  }

  ASSERT_TRUE(socket_address::verify_cidr(td.address));
}



TEST_P(SocketAddressParsing, raw_construction)
{
  // Tests that information doesn't get mangled during construction or
  // formatting
  using namespace net;
  auto td = GetParam();

  socket_address address = create_address(td);

  ASSERT_EQ(td.sa_type, address.type());
  // XXX: this works also for AT_UNSPEC
  if (AT_LOCAL != td.sa_type) {
    ASSERT_EQ(std::string(td.expected), address.cidr_str());
  }
  ASSERT_EQ(td.port, address.port());

  std::stringstream s;
  s << address;

  std::string s2 = full_expected(td, td.port);
  ASSERT_EQ(s2, s.str());
}


TEST_P(SocketAddressParsing, string_construction_without_port)
{
  using namespace net;
  auto td = GetParam();

  // std::cout << "=== " << td.address << " without port" << std::endl;
  socket_address address{td.address};

  ASSERT_EQ(td.sa_type, address.type());
  // XXX: this also works for AT_UNSPEC
  if (AT_LOCAL != td.sa_type) {
   ASSERT_EQ(std::string(td.expected), address.cidr_str());
  }
  ASSERT_EQ(0, address.port()); // No port in ctor

  std::stringstream s;
  s << address;

  std::string s2 = full_expected(td, 0); // No port in ctor
  ASSERT_EQ(s2, s.str());

  // Let's also test the verify_netmask() function, as well as the feature
  // tests.
  if (AT_INET4 == td.sa_type || AT_INET6 == td.sa_type) {
    size_t max = AF_INET == td.type ? 32 : 128;
    for (size_t j = 0 ; j <= max ; ++j) {
      ASSERT_TRUE(address.verify_netmask(j));
    }

    // +1 can't work.
    ++max;
    ASSERT_FALSE(address.verify_netmask(max));

    // Feature tests
    ASSERT_EQ(td.is_any, address.is_any());
    ASSERT_EQ(td.is_loopback, address.is_loopback());
  }
}


TEST_P(SocketAddressParsing, string_construction_with_port)
{
  using namespace net;
  auto td = GetParam();

  // std::cout << "=== " << td.address << " with port " << td.port << std::endl;
  socket_address address(td.address, td.port);

  ASSERT_EQ(td.sa_type, address.type());
  if (AT_INET4 == td.sa_type || AT_INET6 == td.sa_type) {
    ASSERT_EQ(std::string(td.expected), address.cidr_str());
  }
  ASSERT_EQ(td.port, address.port());

  std::stringstream s;
  s << address;

  std::string s2 = full_expected(td, td.port);

  ASSERT_EQ(s2, s.str());

  // Let's also test the verify_netmask() function.
  if (AT_INET4 == td.sa_type || AT_INET6 == td.sa_type) {
    size_t max = AF_INET == td.type ? 32 : 128;
    for (size_t j = 0 ; j <= max ; ++j) {
      ASSERT_TRUE(address.verify_netmask(j));
    }

    // +1 can't work.
    ++max;
    ASSERT_FALSE(address.verify_netmask(max));
  }
}


TEST_P(SocketAddressParsing, serialization)
{
  // Tests that the verify_cidr() function works as expected.
  using namespace net;
  auto td = GetParam();
  if (AT_LOCAL == td.sa_type || AT_UNSPEC == td.sa_type) {
    GTEST_SKIP();
    return;
  }

  // First, parse regularly. This is a bit of a cheat in lieu of putting
  // test data into the array above, but we want to test if we can reconstruct
  // a socket_address from a buffer.
  socket_address tmp{td.address, td.port};

  // Reconstructing from the full buffer must produce an equal address.
  socket_address recon1{tmp.buffer(), tmp.bufsize()};
  ASSERT_EQ(tmp, recon1);

  // On the other hand, reconstructing from a partial buffer should
  // fail.
  socket_address failed{tmp.buffer(), 3};
  ASSERT_EQ(AT_UNSPEC, failed.type());

  // Serialize to minimum buffer size
  test_min_serialization(tmp, true, true);
  test_min_serialization(tmp, true, false);
  test_min_serialization(tmp, false, true);
  test_min_serialization(tmp, false, false);
}




INSTANTIATE_TEST_SUITE_P(net, SocketAddressParsing,
    testing::ValuesIn(parsing_tests),
    generate_name_parsing);


TEST(SocketAddressParsing, unique_hashes)
{
  // Tests that all unique IP addresses in the tests generate unique hashes.
  using namespace net;

  std::set<size_t> hashes;
  std::set<std::string> canonical;
  std::hash<socket_address> hasher;

  for (size_t i = 0 ; i < sizeof(parsing_tests) / sizeof(parsing_test_data) ; ++i) {
    socket_address address = create_address(parsing_tests[i]);
    hashes.insert(hasher(address));
    canonical.insert(address.full_str());
  }

  // Each unique full string must have a unique hash
  ASSERT_EQ(canonical.size(), hashes.size());
}





/*****************************************************************************
 * SocketAddressAsValues
 */

namespace {

struct value_test_data
{
  net::socket_address addr1;
  net::socket_address addr2;
} value_tests[] = {
  { net::socket_address{"192.168.0.1"},
    net::socket_address{"192.168.0.2"} },
  { net::socket_address{"2001:0db8:85a3::8a2e:0370:7334"},
    net::socket_address{"2001:0db8:85a3::8a2e:0370:7335"} },
  { net::socket_address{"192.168.0.1", 1234},
    net::socket_address{"192.168.0.1", 4321} },
  { net::socket_address{"2001:0db8:85a3::8a2e:0370:7334", 1234},
    net::socket_address{"2001:0db8:85a3::8a2e:0370:7334", 4321} },
  { net::socket_address{"/foo/bar"},
    net::socket_address{"/foo/baz"} },
};


std::string generate_name_value(testing::TestParamInfo<value_test_data> const & info)
{
  using namespace net;

  std::string name;
  switch (info.param.addr1.type()) {
    case AT_INET4:
      name = "ipv4_";
      break;

    case AT_INET6:
      name = "ipv6_";
      break;

    case AT_LOCAL:
      name = "local_";
      break;

    default:
      ADD_FAILURE() << "Untestable spec: " << info.param.addr1.full_str();
      break;
  }

  name += info.param.addr1.full_str();

  return symbolize_name(name);
}

} // anonymous namespace

class SocketAddressOperators
  : public testing::TestWithParam<value_test_data>
{
};


TEST_P(SocketAddressOperators, equality)
{
  using namespace net;
  auto td = GetParam();

  test_equality(
      socket_address{td.addr1},
      socket_address{td.addr1}
  );
}


TEST_P(SocketAddressOperators, inequality)
{
  using namespace net;
  auto td = GetParam();

  test_less_than(
      socket_address{td.addr1},
      socket_address{td.addr2}
  );
}


TEST_P(SocketAddressOperators, copy_construction)
{
  using namespace net;
  auto td = GetParam();

  test_copy_construction(td.addr1);
}


TEST_P(SocketAddressOperators, assignment)
{
  using namespace net;
  auto td = GetParam();

  test_assignment(td.addr1);
}


TEST_P(SocketAddressOperators, hashing)
{
  using namespace net;
  auto td = GetParam();

  test_hashing_inequality(td.addr1, td.addr2);
  test_hashing_equality(td.addr1, socket_address{td.addr1});
  test_hashing_equality(td.addr2, socket_address{td.addr2});
}


TEST_P(SocketAddressOperators, swapping)
{
  using namespace net;
  auto td = GetParam();

  test_swapping(td.addr1, td.addr2);
}


TEST_P(SocketAddressOperators, incrementing)
{
  using namespace net;
  auto td = GetParam();

  if (td.addr1.type() == AT_LOCAL) {
    GTEST_SKIP();
    return;
  }

  test_incrementing(td.addr1);
}


INSTANTIATE_TEST_SUITE_P(net, SocketAddressOperators,
    testing::ValuesIn(value_tests),
    generate_name_value);
