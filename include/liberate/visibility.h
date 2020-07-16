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
#ifndef LIBERATE_VISIBILITY_H
#define LIBERATE_VISIBILITY_H

#if defined(_WIN32) || defined(__CYGWIN__) || defined(__MINGW32__)
  #if defined(LIBERATE_IS_BUILDING) && LIBERATE_IS_BUILDING > 0
    #define LIBERATE_API __declspec(dllexport)
  #else
    #define LIBERATE_API __declspec(dllimport)
  #endif
  #define LIBERATE_API_FRIEND LIBERATE_API
#else // Windows
  #if __GNUC__ >= 4
    #define LIBERATE_API  [[gnu::visibility("default")]]
  #else
    #define LIBERATE_API
  #endif // GNU C
  #define LIBERATE_API_FRIEND
#endif // POSIX

// Private symbols may be exported in debug builds for testing purposes.
#if defined(DEBUG)
  #define LIBERATE_PRIVATE LIBERATE_API
#else
  #define LIBERATE_PRIVATE
#endif

#endif // guard
