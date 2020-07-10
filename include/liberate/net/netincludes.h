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

#ifndef LIBERATE_NETINCLUDES_H
#define LIBERATE_NETINCLUDES_H

#include <liberate/config.h>

#if defined(LIBERATE_HAVE_ARPA_INET_H)
#include <arpa/inet.h>
#endif

#if defined(LIBERATE_HAVE_NETINET_IN_H)
#include <netinet/in.h>
#endif

#if defined(LIBERATE_HAVE_LINUX_UN_H)
#  include <linux/un.h>
#  define LIBERATE_HAVE_SOCKADDR_UN
#else
#  if defined(LIBERATE_HAVE_SYS_UN_H)
#    include <sys/un.h>
#    define UNIX_PATH_MAX 108
#    define LIBERATE_HAVE_SOCKADDR_UN
#  endif
#endif

#if defined(LIBERATE_HAVE_SYS_SOCKET_H)
#  include <sys/socket.h>
#endif

#if defined(LIBERATE_HAVE_WINSOCK2_H)
#  define FD_SETSIZE 32767
#  include <winsock2.h>
#  pragma comment(lib, "Ws2_32.lib")
#endif

#if defined(LIBERATE_HAVE_WS2TCPIP_H)
#  include <ws2tcpip.h>
#endif

#if defined(LIBERATE_HAVE_AFUNIX_H)
#  include <afunix.h>
#  define LIBERATE_HAVE_SOCKADDR_UN
#endif

#if defined(LIBERATE_WIN32)
using sa_family_t = ADDRESS_FAMILY;
#endif

#endif // guard
