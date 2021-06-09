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
#include <liberate.h>

#include <stdlib.h>

#include "net/netincludes.h"

#include <liberate/logging.h>

namespace liberate {

namespace {

/**
 * The api_global struct contains global state - which really shouldn't be
 * state at all! Really, don't store any state here. It's just a struct so we
 * have RAII here. And we use RAII so we can make as many shared_ptr copies
 * as we want, the last one to be destroyed will clear this global struct.
 *
 * Use the constructor/destructor for initializing/deinitializing things
 * *once*, but keep all API state in the api_impl struct.
 */
struct api_global
{
  api_global()
  {
#if defined(LIBERATE_WIN32)
    WSADATA data{};
    // Request Winsock 2.2
    int res = WSAStartup(MAKEWORD(2, 2), &data);
    LIBLOG_INFO("WSA Description: " << data.szDescription);
    if (res != 0) {
      LIBLOG_ERROR("WSA System Status: " << data.szSystemStatus);
      // FIXME system error here
      throw std::runtime_error{"WSAStartup failed!"};
    }
#endif // win32
  }

  ~api_global()
  {
#if defined(LIBERATE_WIN32)
    WSACleanup();
    LIBLOG_INFO("WSA cleanup finished.");
#endif // win32
  }
};

std::shared_ptr<api_global> global_create()
{
  static std::shared_ptr<api_global> global = std::make_shared<api_global>();
  return global;
}

} // anonymous namespace


/**
 * Holder for all hidden API state.
 */
struct api::api_impl
{
  std::shared_ptr<api_global> global = global_create();

  // TODO If we need global API state, but it here. Make accessors in the
  //      API class itself.
};



api::api()
  : m_impl{std::make_unique<api_impl>()}
{
}



api::~api()
{
}

} // namespace packeteer
