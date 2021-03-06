/**
 * This file is part of liberate.
 *
 * Author(s): Jens Finkhaeuser <jens@finkhaeuser.de>
 *
 * Copyright (c) 2017-2021 Jens Finkhaeuser.
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
#ifndef LIBERATE_CPP_OPERATORS_COMPARISON_H
#define LIBERATE_CPP_OPERATORS_COMPARISON_H

#ifndef __cplusplus
#error You are trying to include a C++ only header file
#endif

#include <liberate.h>

namespace liberate::cpp {

/**
 * Supplement comparison operators when is_equal_to and is_less_than are
 * defined. Make sure to provide them as protected functions to allow class
 * hierarchies where every member uses this template.
 *
 * Use with curiously recurring inheritance.
 **/
template <typename T>
struct comparison_operators
{
  inline bool operator==(T const & other) const
  {
    return static_cast<T const *>(this)->T::is_equal_to(other);
  }

  inline bool operator<(T const & other) const
  {
    return static_cast<T const *>(this)->T::is_less_than(other);
  }

  inline bool operator!=(T const & other) const
  {
    return !(*static_cast<T const *>(this) == other);
  }

  inline bool operator>(T const & other) const
  {
    return other < *static_cast<T const *>(this);
  }

  inline bool operator>=(T const & other) const
  {
    return !(*static_cast<T const *>(this) < other);
  }

  inline bool operator<=(T const & other) const
  {
    return !(other < *static_cast<T const *>(this));
  }
};

} // namespace liberate::cpp

/**
 * Supplement comparison operators as free functions for types that already
 * define == and <.
 */
#define LIBERATE_MAKE_COMPARABLE(thetype) \
    inline bool operator!=(thetype const & first, thetype const & second) \
    { \
      return !(first == second); \
    } \
    inline bool operator>(thetype const & first, thetype const & second) \
    { \
      return second < first; \
    } \
    inline bool operator>=(thetype const & first, thetype const & second) \
    { \
      return !(first < second); \
    } \
    inline bool operator<=(thetype const & first, thetype const & second) \
    { \
      return !(second < first); \
    }


#endif // guard
