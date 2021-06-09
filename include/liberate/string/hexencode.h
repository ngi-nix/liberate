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
#include <sstream>

#include <iostream> // FIXME

#include <liberate/types/byte.h>
#include <liberate/serialization/integer.h>

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


/**
 * Encode an entire memory region in a hexdump style format.
 *
 * The usage is as follows: instantiate a hexdumper struct that is parametrized
 * with some format flags. Then pass a pointer and size to the dumper's
 * () operator. This allows the hexdump to be integrated into any kind of
 * output stream, e.g.:
 *
 * canonical_hexdump hd;
 * std::cerr << "Something weird in this memory region: "
 *    << hd(buf, bufsize) << std::endl;
 */
template <
  // Size of an offset; this can be used to calculate the number of Bytes
  // required to display it.
  size_t OFFSET_SIZE = sizeof(intptr_t),

  // Display this many Bytes per line
  size_t BYTES_PER_LINE = 16,
  // Split each line into columns of this size
  size_t BYTES_PER_COLUMN = 1,
  // Group this many columns into a group
  size_t GROUPS = 2,

  // Padding after a group/column
  size_t COLUMN_PAD = 1,
  size_t GROUP_PAD = 2,

  // Format characters; if \0 they are unused.
  char FRAME = '|',
  char PLACEHOLDER = '\0'
>
struct hexdump
{
  // Number of columns in total, and columns per group.
  static constexpr size_t COLUMNS = BYTES_PER_LINE / BYTES_PER_COLUMN;
  static constexpr size_t COLUMNS_PER_GROUP = COLUMNS / GROUPS;

  static constexpr size_t BYTES_PER_GROUP = COLUMNS_PER_GROUP * BYTES_PER_COLUMN;

  // Output widths with and without trailing end padding. Internal padding is
  // always applied.
  static constexpr size_t O_OFFSET_WIDTH_UNPADDED = (OFFSET_SIZE * 2);
  static constexpr size_t O_OFFSET_WIDTH = O_OFFSET_WIDTH_UNPADDED + GROUP_PAD;

  static constexpr size_t O_COLUMN_WIDTH_UNPADDED = (BYTES_PER_COLUMN * 2);
  static constexpr size_t O_COLUMN_WIDTH = O_COLUMN_WIDTH_UNPADDED + COLUMN_PAD;

  static constexpr size_t O_GROUP_WIDTH_UNPADDED = (
      ((COLUMNS_PER_GROUP - 1) * O_COLUMN_WIDTH)
      + O_COLUMN_WIDTH_UNPADDED
  );
  static constexpr size_t O_GROUP_WIDTH = O_GROUP_WIDTH_UNPADDED + GROUP_PAD;

  static constexpr size_t O_PLAIN_WIDTH = (
      (FRAME == '\0' ? 0 : 2)
      + BYTES_PER_LINE
  ); // Always unpadded, end of the line

  static constexpr size_t O_LINE_WIDTH = (
    O_OFFSET_WIDTH
    + (GROUPS * O_GROUP_WIDTH)
    + O_PLAIN_WIDTH
  );



  /**
   * Take an input pointer and input size in Bytes. Optionally take a flag that
   * determines whether the printed offsets should start from zero or from the
   * input pointer.
   *
   * The result is a string containing the hexump.
   *
   * There is an overload for std::string for convenience.
   */
  template <typename T>
  inline std::string operator()(
      T const * input,
      size_t input_size,
      bool from_zero = true) const
  {
    return format(
        reinterpret_cast<::liberate::types::byte const *>(input),
        input_size,
        from_zero
          ?  nullptr
          : reinterpret_cast<::liberate::types::byte const *>(input)
      );
  }

  inline std::string operator()(std::string const & str, bool from_zero = true) const
  {
    return operator()(str.c_str(), str.size(), from_zero);
  }


private:
  static inline std::string format(
      ::liberate::types::byte const * input,
      size_t input_size,
      ::liberate::types::byte const * display_base
    ) const
  {
    if (!input || !input_size) {
      return {};
    }

    // Fixed sized line buffer (plus EOL)
    ::liberate::types::byte linebuf[O_LINE_WIDTH + 1];

    // Number of lines
    auto num_lines = (input_size + BYTES_PER_LINE) / BYTES_PER_LINE;

    // Display offset base may be different from the actual input base,
    // e.g. for displaying Bytes in a file vs memory.
    auto display_offset = display_base;
    auto offset = input;
    auto remaining = input_size;

    std::stringstream result;

    // Handle all but the last line the same.
    for (size_t line = 0 ; line < num_lines - 1 ; ++line) {
      auto line_offset = linebuf;
      size_t line_remaining = sizeof(linebuf);

      // First output the display offset. We'll treat the pointer as Bytes
      // here.
      add_offset(line_offset, line_remaining, display_offset);
      add_pad(line_offset, line_remaining, GROUP_PAD);

      // Add groups
      add_groups(line_offset, line_remaining, offset, BYTES_PER_LINE);

      // Add plaintext
      add_plaintext(line_offset, line_remaining, offset, BYTES_PER_LINE);

      *line_offset = '\0';
      result << linebuf << std::endl;

      // End of the line, increment display offset and offset
      display_offset += BYTES_PER_LINE;
      offset += BYTES_PER_LINE;
      remaining -= BYTES_PER_LINE;
    }

    // The last line is similar, but we have to take care of padding incomplete
    // columns.
    auto line_offset = linebuf;
    size_t line_remaining = sizeof(linebuf);

    // First output the display offset. We'll treat the pointer as Bytes
    // here.
    add_offset(line_offset, line_remaining, display_offset);
    add_pad(line_offset, line_remaining, GROUP_PAD);

    // Add groups
    add_groups(line_offset, line_remaining, offset, remaining);

    // Add plaintext
    add_plaintext(line_offset, line_remaining, offset, remaining);

    *line_offset = '\0';
    result << linebuf << std::endl;

    // As the very last line, we'll add the end offset
    display_offset += remaining;
    line_offset = linebuf;
    line_remaining = sizeof(linebuf);

    add_offset(line_offset, line_remaining, display_offset);

    *line_offset = '\0';
    result << linebuf;

    return result.str();
  }


  static inline void add_plaintext(::liberate::types::byte *& buf,
      size_t & remaining, liberate::types::byte const * offset,
      size_t offset_size)
  {
    // Add frame, if we have it.
    if (FRAME) {
      *buf++ = FRAME;
      --remaining;
    }

    // Now display the line, using the replacement character if
    // necessary.
    for (size_t i = 0 ; i < offset_size ; ++i) {
      auto c = *(offset + i);
      *buf++ = std::isprint(c) && c != ' '
        ? c
        : (PLACEHOLDER ? PLACEHOLDER : ' ');
      --remaining;
    }

    // Also add closing frame
    if (FRAME) {
      *buf++ = FRAME;
      --remaining;
    }
  }


  static inline void add_groups(::liberate::types::byte *& buf,
      size_t & remaining, liberate::types::byte const * offset,
      size_t offset_size)
  {
    auto offs = offset;
    auto rem = offset_size;
    for (size_t i = 0 ; i < GROUPS ; ++i) {
      size_t to_process = std::min(BYTES_PER_GROUP, rem);

      add_single_group(buf, remaining, offs, to_process);
      add_pad(buf, remaining, GROUP_PAD);

      offset += to_process;
      rem -= to_process;
    }
  }

  static inline void add_single_group(::liberate::types::byte *& buf,
      size_t & remaining, liberate::types::byte const * offset,
      size_t offset_size)
  {
    auto offs = offset;
    auto rem = offset_size;
    for (size_t i = 0 ; i < COLUMNS_PER_GROUP ; ++i) {
      size_t to_process = std::min(BYTES_PER_COLUMN, rem);

      add_column(buf, remaining, offs, to_process);
      add_pad(buf, remaining, COLUMN_PAD);

      offs += to_process;
      rem -= to_process;
    }
  }

  static inline void add_column(::liberate::types::byte *& buf,
      size_t & remaining, liberate::types::byte const * offset,
      size_t offset_size)
  {
    // Each column is easy to encode.
    auto res = hexencode(buf, remaining, offset, offset_size);
    if (res != offset_size * 2) {
      return;
    }

    // We also add padding empty columns or remaining parts.
    auto rem = (BYTES_PER_COLUMN * 2) - res;
    add_pad(buf, remaining, rem);

    buf += res;
    remaining -= res;
  }


  static inline void add_offset(::liberate::types::byte *& buf,
      size_t & remaining, void const * display_offset)
  {
    ::liberate::types::byte offsbuf[sizeof(intptr_t)];

    intptr_t offs = reinterpret_cast<intptr_t>(display_offset);
    auto res = ::liberate::serialization::serialize_int(offsbuf,
        sizeof(offsbuf), offs);
    if (res != sizeof(offs)) {
      return;
    }

    // The offset buffer now holds the entire offset, but we want to
    // truncate it to the offset size. That means we're only serializing
    // the last N bytes.
    auto ptr = offsbuf + sizeof(offsbuf) - OFFSET_SIZE;
    res = hexencode(
        buf, remaining,
        ptr, OFFSET_SIZE);
    if (res != OFFSET_SIZE * 2) {
      return;
    }

    buf += res;
    remaining -= res;
  }


  static inline void add_pad(::liberate::types::byte *& buf,
      size_t & remaining, size_t amount)
  {
    for (size_t i = 0 ; i < amount ; ++i) {
      *buf++ = ' ';
      --remaining;
    }
  }

};

// The default parameters are almost as with he canonical -C parameter from
// hexdump. The main difference is the fixed 32 bit/4 byte offset.
using canonical_hexdump = hexdump<4>;

// Another format uses wider columns and the full pointer width, no frame
// but a replacement character instead.
using wide_hexdump = hexdump<
  sizeof(intptr_t),
  32, 4, 2,
  1, 2,
  '\0', '.'
>;



} // namespace liberate::string

#endif // guard
