/**
 * This file is part of liberate.
 *
 * Author(s): Jens Finkhaeuser <jens@finkhaeuser.de>
 *
 * Copyright (c) 2021 Jens Finkhaeuser.
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
#ifndef LIBERATE_STRING_HEXENCODE_H
#define LIBERATE_STRING_HEXENCODE_H

#include <liberate.h>

#include <cstddef>
#include <vector>
#include <string>

#include <liberate/types/byte.h>

namespace liberate::string {

/**
 * Hex encode and decode. Return the size of the output buffer used, or 0 if
 * nothing could be transcoded (e.g. due to a too small output buffer).
 **/
LIBERATE_API
size_t
hexencode(::liberate::types::byte * output, size_t output_size,
    ::liberate::types::byte const * input,
    size_t input_size, bool uppercase = false);

LIBERATE_API
size_t
hexdecode(::liberate::types::byte * output, size_t output_size,
    ::liberate::types::byte const * input,
    size_t input_size);

/**
 * Convenvience wrappers
 **/
LIBERATE_API
std::string
hexencode(char const * input, size_t input_size, bool uppercase = false);

LIBERATE_API
std::string
hexencode(::liberate::types::byte const * input, size_t input_size, bool uppercase = false);


LIBERATE_API
std::vector<::liberate::types::byte>
hexdecode(char const * input, size_t input_size);

LIBERATE_API
std::vector<::liberate::types::byte>
hexdecode(::liberate::types::byte const * input, size_t input_size);



} // namespace liberate::string

#endif // guard
