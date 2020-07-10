# liberate

liberate is a small platform liberation library for the Interpeer Project.

The scope of the project is bound to the needs of the overall proejct. It may
change over time. For now, the focus is on the subset of platform abstractions
that are shared amongst several other projects:

- [packeteer](https://gitlab.com/interpeer/packeteer)
- [channeler](https://gitlab.com/interpeer/channeler)

## Contents

1. `liberate/cpp/` contains headers that make some C++ language features
  a little more accessible.
1. `liberate/net/` contains classes for socket and network addresses, and
  a simple URL parser for socket-URLs.
1. `liberate/fs/` contains filesystem functionality.
1. `liberate/string/` contains some string utilities.
1. `liberate/visibility.h` contains library symbol visibility macros that
  can be re-used in other libraries.
