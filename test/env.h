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
#ifndef LIBERATE_TEST_ENV_H
#define LIBERATE_TEST_ENV_H

#include <gtest/gtest.h>

#include <liberate.h>

class TestEnvironment : public testing::Environment
{
public:
  liberate::api api;
};

extern TestEnvironment * test_env;

#endif // guard
