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
#include <liberate/fs/tmp.h>

#include <fstream>

#include <gtest/gtest.h>


TEST(FsTmp, has_value)
{
  auto tmp = liberate::fs::temp_name();
  // std::cout << "TMP: "<< tmp << std::endl;

  // Ask for a few characters at least
  ASSERT_GT(tmp.size(), 3);
}



TEST(FsTmp, contains_prefix)
{
  auto tmp = liberate::fs::temp_name("foo");
  // std::cout << "TMP: "<< tmp << std::endl;
  EXPECT_GT(tmp.size(), 3);

  auto pos = tmp.find("foo");
  ASSERT_NE(pos, std::string::npos);
}


TEST(FsTmp, open_file)
{
  auto tmp = liberate::fs::temp_name("foo");
  std::ofstream os(tmp.c_str());
  os << "test";
  os.close();
}
