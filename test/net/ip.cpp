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

#include <liberate/net/ip.h>

#include <gtest/gtest.h>

#include "../test_name.h"

namespace net = liberate::net;

namespace {

// Two packets grabbed with wireshark
constexpr std::byte const ipv4_buf[] = {
    std::byte{0x45}, std::byte{0x00}, std::byte{0x00}, std::byte{0x28},
    std::byte{0x00}, std::byte{0x00}, std::byte{0x40}, std::byte{0x00},
    std::byte{0x40}, std::byte{0x06}, std::byte{0x3c}, std::byte{0xce},
    std::byte{0x7f}, std::byte{0x00}, std::byte{0x00}, std::byte{0x01},
    std::byte{0x7f}, std::byte{0x00}, std::byte{0x00}, std::byte{0x01}
  };


constexpr std::byte const ipv6_buf[] = {
    std::byte{0x60}, std::byte{0x03}, std::byte{0x9b}, std::byte{0xe1},
    std::byte{0x00}, std::byte{0x14}, std::byte{0x06}, std::byte{0x40},
    std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
    std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
    std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
    std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x01},
    std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
    std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
    std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
    std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x01}
  };

} // anonymous namespace


TEST(IP, parse_source_address_ipv4)
{
  net::socket_address addr;
  auto res = net::parse_source_address(addr, ipv4_buf, sizeof(ipv4_buf));
  ASSERT_TRUE(res);
  ASSERT_EQ(addr.type(), net::AT_INET4);
  ASSERT_EQ(addr, net::socket_address{"127.0.0.1"});
}



TEST(IP, parse_source_address_ipv6)
{
  net::socket_address addr;
  auto res = net::parse_source_address(addr, ipv6_buf, sizeof(ipv6_buf));
  ASSERT_TRUE(res);
  ASSERT_EQ(addr.type(), net::AT_INET6);
  ASSERT_EQ(addr, net::socket_address{"::1"});
}



TEST(IP, parse_dest_address_ipv4)
{
  net::socket_address addr;
  auto res = net::parse_dest_address(addr, ipv4_buf, sizeof(ipv4_buf));
  ASSERT_TRUE(res);
  ASSERT_EQ(addr.type(), net::AT_INET4);
  ASSERT_EQ(addr, net::socket_address{"127.0.0.1"});
}



TEST(IP, parse_dest_address_ipv6)
{
  net::socket_address addr;
  auto res = net::parse_dest_address(addr, ipv6_buf, sizeof(ipv6_buf));
  ASSERT_TRUE(res);
  ASSERT_EQ(addr.type(), net::AT_INET6);
  ASSERT_EQ(addr, net::socket_address{"::1"});
}



TEST(IP, parse_addresses_ipv4)
{
  net::socket_address source;
  net::socket_address dest;
  auto res = net::parse_addresses(source, dest, ipv4_buf, sizeof(ipv4_buf));
  ASSERT_TRUE(res);
  ASSERT_EQ(source.type(), net::AT_INET4);
  ASSERT_EQ(source, net::socket_address{"127.0.0.1"});
  ASSERT_EQ(dest.type(), net::AT_INET4);
  ASSERT_EQ(dest, net::socket_address{"127.0.0.1"});
}


TEST(IP, parse_addresses_ipv6)
{
  net::socket_address source;
  net::socket_address dest;
  auto res = net::parse_addresses(source, dest, ipv6_buf, sizeof(ipv6_buf));
  ASSERT_TRUE(res);
  ASSERT_EQ(source.type(), net::AT_INET6);
  ASSERT_EQ(source, net::socket_address{"::1"});
  ASSERT_EQ(dest.type(), net::AT_INET6);
  ASSERT_EQ(dest, net::socket_address{"::1"});
}


TEST(IP, parse_bad_packet)
{
  net::socket_address addr;
  auto res = net::parse_source_address(addr,
      reinterpret_cast<std::byte const *>("Hello, world!"), 13);
  ASSERT_FALSE(res);
}
