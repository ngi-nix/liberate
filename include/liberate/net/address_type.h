/**
 * This file is part of liberate.
 *
 * Author(s): Jens Finkhaeuser <jens@finkhaeuser.de>
 *
 * Copyright (c) 2019-2021 Jens Finkhaeuser.
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

#ifndef LIBERATE_NET_ADDRESS_TYPE_H
#define LIBERATE_NET_ADDRESS_TYPE_H

// *** Config
#include <liberate.h>

namespace liberate::net {

/**
 * Socket or network address type.
 */
enum LIBERATE_API address_type : int8_t
{
  AT_UNSPEC = -1,
  AT_INET4 = 0,
  AT_INET6,
  AT_LOCAL, // Only works for pipes, and not for networks. 
  AT_UNIX = AT_LOCAL, // For people who prefer that name.
};


} // namespace liberate::net

#endif // guard
