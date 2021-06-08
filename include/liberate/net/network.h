/**
 * This file is part of liberate.
 *
 * Author(s): Jens Finkhaeuser <jens@finkhaeuser.de>
 *
 * Copyright (c) 2014 Unwesen Ltd.
 * Copyright (c) 2015-2020 Jens Finkhaeuser.
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

#ifndef LIBERATE_NET_NETWORK_H
#define LIBERATE_NET_NETWORK_H

// *** Config
#include <liberate.h>

// *** C++ includes
#include <string>
#include <memory>
#include <stdexcept>

// *** Own includes
#include <liberate/cpp/operators/comparison.h>
#include <liberate/net/address_type.h>
#include <liberate/net/socket_address.h>

namespace liberate::net {

/*****************************************************************************
 * Forward declarations
 **/
class socket_address;


/*****************************************************************************
 * Network
 **/
/**
 * Offers operations on networks, including allocation of available addresses
 * within a network.
 **/
class LIBERATE_API network
  : public ::liberate::cpp::comparison_operators<network>
{
public:
  /**
   * Constructor. The netspec is expected to be an IP network specification in
   * CIDR notation. The constructor will throw an exception if the netspec
   * cannot be parsed.
   *
   * Note that due to memory limitations, it is all but impossible to manage
   * all possible IPv6 in a large network. As such, we artificially limit the
   * number of addresses you can manage with this class to 2^64.
   **/
  explicit network(std::string const & netspec);
  virtual ~network();

  /**
   * The copy constructor creates a *new* network with none of the state used
   * to manage reserved addresses. It is functionally equivalent to writing
   *   network n1{spec};
   *   network n2{n1.netspec()};
   */
  network(network const & other);


  /**
   * Throws away all state in the network class and reinitializes it with the
   * given new netspec.
   **/
  void reset(std::string const & netspec);


  /**
   * Verifies the given netspec string would create a valid network.
   **/
  static bool verify_netspec(std::string const & netspec);


  /**
   * Returns the network mask size.
   **/
  size_t mask_size() const;


  /**
   * Returns the maximum amount of allocatable addresses.
   **/
  uint64_t max_size() const;


  /**
   * Returns the network family as a socket_address_type. This is one of
   * AT_INET4 or AT_INET6. May return AT_UNSPEC for badly constructed
   * networks.
   **/
  address_type family() const;


  /**
   * Returns true if the given address is part of the network, false otherwise.
   **/
  bool in_network(socket_address const & address) const;


  /**
   * Return the network, default gateway and broadcast addresses of this
   * network respectively.
   **/
  socket_address network_address() const;
  socket_address gateway_address() const;
  socket_address broadcast_address() const;

  /**
   * Return a netspec that can be used to create an equivalent network
   * instance.
   */
  std::string netspec() const;


  /**
   * Returns a new socket_address (with port set to 0) that is part of this
   * network, or nullptr if there are no available addresses.
   **/
  socket_address reserve_address();

  /**
   * Return a socket address for an identifier. Note that this does *not*
   * reserve this address, it just performs the mapping. Use reserve_address()
   * to actually reserve the address, or is_reserved() to query whether it is
   * already reserved.
   */
  socket_address mapped_address(std::string const & identifier) const;
  socket_address mapped_address(void const * identifier, size_t const & length);

  /**
   * Returns a new socket_address (with port set to 0) that is part of this
   * network, or nullptr if there are no available addresses.
   *
   * Providing the same identifier string will always yield the same address.
   * Two or more identifier strings may yield the same address (i.e. cause a
   * collision). The chances of collisions are much lower when larger networks
   * are used.
   **/
  socket_address reserve_address(std::string const & identifier);
  socket_address reserve_address(void const * identifier, size_t const & length);

  /**
   * Finally, allow reserving an address directly. We don't return the address
   * here, just a boolean flag to determine whether it was successful.
   **/
  bool reserve_address(socket_address const & addr);

  /**
   * Releases an address that is part of this network back into the pool. If
   * the address is not in the network, false is returned. Otherwise, true is
   * returned, and the next call to reserve_address() could return the same
   * address again.
   **/
  bool release_address(socket_address const & addr);

  /**
   * Return true if an address is reserved already with the given identifier,
   * false otherwise.
   */
  bool is_reserved(std::string const & identifier) const;
  bool is_reserved(void const * identifier, size_t const & length) const;
  bool is_reserved(socket_address const & addr) const;

protected:
  /**
   * Used by cpp::comparison_operators
   **/
  friend struct liberate::cpp::comparison_operators<network>;

  virtual bool is_equal_to(network const & other) const;
  virtual bool is_less_than(network const & other) const;

private:
  // Pointer to implementation
  struct network_impl;
  std::unique_ptr<network_impl> m_impl;

  // Creates a version of the given input address with the netmask applied.
  socket_address make_masked(socket_address const & input) const;

  friend LIBERATE_API_FRIEND std::ostream & operator<<(std::ostream & os, network const & addr);
};


/**
 * Formats a network into human-readable form.
 **/
LIBERATE_API std::ostream & operator<<(std::ostream & os, network const & addr);


} // namespace liberate::net

#endif // guard
