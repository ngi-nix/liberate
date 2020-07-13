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
1. `liberate/logging.h` contains logging macros.

## Logging

Liberate itself does not provide genuine logging facilities. It does, however,
provide macros which delegate to one of several logging backends, or to a simple
stderr-based logging mechanism. The purpose of these macros is to provide the same
logging interface for liberate and it's dependent project, but leave the project
user in control of where log data is sent.

You choose the logging backend from `meson_options.txt`, or by passing the
`-DLIBERATE_LOG_BACKEND` compiler flag. Note that depending on how you use the
macros, either option may be more appropriate for you. If all your projects
are built with meson, in the top-level project do:

```bash
$ cd <build-dir>
$ meson <source-dir> -Dliberate:log_backend=plog
```

Everything should build with the selected backend. Note that depending on the
backend you choose, you may need to promote the backend's subproject, but meson
will provide instructions for how to do that.

Possible backends are:

1. [loguru](https://github.com/emilk/loguru)
1. [plog](https://github.com/SergiusTheBest/plog)
1. [spdlog](https://github.com/gabime/spdlog)
1. Builtin `stderr` log.

### Log Levels

In order of descending verbosity:

| Liberate              | loguru  | plog      | spdlog     |
|-----------------------|---------|-----------|------------|
| `LIBLOG_LEVEL_TRACE`  | `9`     | `verbose` | `TRACE`    |
| `LIBLOG_LEVEL_DEBUG`  | `1`     | `debug`   | `DEBUG`    |
| `LIBLOG_LEVEL_INFO`   | `INFO`  | `info`    | `INFO`     |
| `LIBLOG_LEVEL_WARN`   | `WARN`  | `warning` | `WARN`     |
| `LIBLOG_LEVEL_ERROR`  | `ERROR` | `error`   | `ERROR`    |
| `LIBLOG_LEVEL_FATAL`  | `FATAL` | `fatal`   | `CRITICAL` |


### Log Macros

- `LIBLOG(LEVEL, message)` log message at the given log level.
- `LIBLOG_<LEVEL_WITHOUT_PREFIX>(message)` shorthand for the above.
- Short versions of the above: `LLOG_<FIRST_LEVEL_LETTER>(message)`.
- `LIBLOG_ERR(code, message)` logs the given message. The code is considered
  a platform error code, as returned by e.g. `errno` or `GetLastError()`.
- `LIBLOG_ERRNO(message)` same as above, but uses `errno`/`WSAGetLastError()`
  internally to retrieve the platform error code.
- `LIBLOG_EXC(exception, message)` logs the message, followed by the
  exception's `.what()`.

All of the shorthand macros above for logging error information log
at `LIBLOG_ERROR` level. The below, therefore, are all equivalent:

```c++
char buf[200] = {};
snprintf("%s // %s", msg, strerror(errno));

LIBLOG(LIBLOG_LEVEL_ERROR, buf);
LIBLOG_ERROR(buf);
LLOG_E(buf);
LIBLOG_ERR(errno, msg);
LIBLOG_ERRNO(msg);

auto exc = hypothetical_make_exception(errno);
LIBLOG_EXC(exc, msg);
```

### Builtin logger

The builtin `stderr` log is very simple: it prefixes each log entry with the
file and line number, and the severity. Additionally, it only produces output
if the `-DDEBUG` flag is set and the `-DNDEBUG` flag is unset, so that you can
produce entirely log-free release binaries.

### Other Backends

Liberate does not initialize other logging backends. It includes the selected
backend headers, but does nothing else. You can use the logging header to write
log messages, but where they end up is up to your project's initialization
code.

- `loguru`:
  - In the examples, `FATAL` error messages are redirected to `ERROR`, to
    demonstrate all `LIBLOG*` macros appropriately.
  - The library cannot be used on Android.
