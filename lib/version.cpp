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
#include <build-config.h>

#include <liberate/version.h>

namespace liberate {


std::pair<std::string, std::string>
version()
{
  return std::make_pair<std::string, std::string>(LIBERATE_PACKAGE_MAJOR,
      LIBERATE_PACKAGE_MINOR);
}



char const * copyright_string()
{
  return
    LIBERATE_PACKAGE_NAME " " LIBERATE_PACKAGE_VERSION " "
      LIBERATE_PACKAGE_URL "\n"
    "Copyright (c) 2020 Jens Finkhaeuser.\n"
    "Licensed under the the GPLv3 for personal, educational or non-profit "
    "use.\n"
    "Other licensing options available; please contact the copyright holder "
    "for information."
    ;
}


char const * license_string()
{
  return
    "This software is licensed under the terms of the GNU GPLv3 for personal,\n"
    "educational and non-profit use. For all other uses, alternative license\n"
    "options are available. Please contact the copyright holder for\n"
    "additional information, stating your intended usage.\n"
    "\n"
    "You can find the full text of the GPLv3 in the COPYING file in this code\n"
    "distribution.\n"
    "\n"
    "This software is distributed on an \"AS IS\" BASIS, WITHOUT ANY\n"
    "WARRANTY; without even the implied warranty of MERCHANTABILITY or\n"
    "FITNESS FOR A PARTICULAR PURPOSE."
    ;
}

} // namespace liberate
