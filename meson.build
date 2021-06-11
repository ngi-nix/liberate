##############################################################################
# Project
project('liberate', 'cpp',
  version: '0.1.0',
  license: 'GPLv3',
  meson_version: '>=0.53.0',
  default_options: [
    'cpp_std=c++17',
    'default_library=both',
  ])


##############################################################################
# Versioning, project and libraries

# There are so many versioning schemes, each of which is incompatible with
# others. We'll manage best by keeping things simple:
#
# - The package version follows semver
# - The library version is the package version
# - The ABI version, i.e. the compatibility we expect, is the major
#   component of the package
_splitver = meson.project_version().split('.')
PACKAGE_MAJOR = _splitver[0]
PACKAGE_MINOR = _splitver[1]
PACKAGE_PATCH = _splitver[2]

ABI_VERSION = PACKAGE_MAJOR
LIB_VERSION = meson.project_version()
PACKAGE_VERSION = meson.project_version()

##############################################################################
# Configuration

conf_data = configuration_data()
compiler = meson.get_compiler('cpp')

host_type = ''
if host_machine.system() in [ 'cygwin', 'darwin', 'dragonfly', 'freebsd', 'gnu', 'linux', 'netbsd' ]
  host_type = 'posix'
elif host_machine.system() == 'windows'
  host_type = 'win32'
elif host_machine.system().startswith('android')
  host_type = 'android'
endif
summary('Host platform', host_type, section: 'Platform')

# For Windows, try to determine the SDK version.
winsdk_ver = -1
if compiler.has_header('ntverp.h')
  code = '''#include <iostream>
#include <ntverp.h>

int main()
{
  std::cout << VER_PRODUCTMAJORVERSION;
}
'''
  result = compiler.run(code, name: 'Windows SDK version check.')
  winsdk_ver = result.stdout().to_int()
  summary('Windows SDK major version', winsdk_ver, section: 'Platform')
endif


### Compiler flags
compiler_id = compiler.get_id()

cpp_lib_is_building = []
cpp_args = []

posix_common_args = [
  '-Wall', '-Wextra', '-Wpedantic', '-Wshadow', '-Wstrict-aliasing',
  '-Wstrict-overflow=5', '-Wcast-align',
  '-Wpointer-arith', '-Wcast-qual', '-Wold-style-cast',
]
if compiler_id == 'clang'
  cpp_args += posix_common_args + [
    '-Wabi',
  ]
  cpp_lib_is_building = ['-DLIBERATE_IS_BUILDING=1']
elif compiler_id == 'gcc'
  cpp_args += posix_common_args + [
    '-Wstrict-null-sentinel',
  ]
  cpp_lib_is_building = ['-DLIBERATE_IS_BUILDING=1']
elif compiler_id == 'msvc'
  cpp_args += [
    '/wd4250', '/wd4251', '/wd4275',
  ]
  cpp_lib_is_building = ['/DLIBERATE_IS_BUILDING=1']
endif

bt = get_option('buildtype')
if bt in ['debug']
  if compiler_id == 'clang'
    cpp_args += ['-ggdb', '-DDEBUG']
  elif compiler_id == 'gcc'
    cpp_args += ['-g3', '-DDEBUG']
  elif compiler_id == 'msvc'
    cpp_args += ['/FS', '/DDEBUG=1']
  endif
elif bt in ['debugoptimized']
  if compiler_id == 'clang'
    cpp_args += ['-ggdb', '-DNDEBUG']
  elif compiler_id == 'gcc'
    cpp_args += ['-g3', '-DNDEBUG']
  elif compiler_id == 'msvc'
    cpp_args += ['/FS', '/DNDEBUG=1']
  endif
else
  posix_release_args = [
    '-fvisibility=hidden', '-fvisibility-inlines-hidden',
    '-DNDEBUG',
  ]
  if compiler_id == 'clang'
    cpp_args += posix_release_args
  elif compiler_id == 'gcc'
    cpp_args += posix_release_args
  elif compiler_id == 'msvc'
    cpp_args += ['/Oi', '/DNDEBUG=1']
  endif
endif

link_args = []
if host_type == 'android'
  # Only posix compilers supported (?)
  cpp_args += [
    '-DANDROID_STL=c++_shared',
    '-fexceptions', '-frtti',
  ]
  link_args = [
    '-lstdc++', '-lm',
  ]
endif


# Make things work with MSVC and Windows SDK <10
if compiler_id == 'msvc' and winsdk_ver < 10
  cpp_args += ['/permissive']
endif

add_project_arguments(cpp_args, language: 'cpp')


### Version and package information
conf_data.set_quoted('LIBERATE_PACKAGE_MAJOR', PACKAGE_MAJOR)
conf_data.set_quoted('LIBERATE_PACKAGE_MINOR', PACKAGE_MINOR)
conf_data.set_quoted('LIBERATE_PACKAGE_PATCH', PACKAGE_PATCH)
conf_data.set_quoted('LIBERATE_PACKAGE_VERSION', PACKAGE_VERSION)
conf_data.set_quoted('LIBERATE_ABI_VERSION', ABI_VERSION)
conf_data.set_quoted('LIBERATE_LIB_VERSION', LIB_VERSION)

conf_data.set_quoted('LIBERATE_PACKAGE_NAME', meson.project_name())
conf_data.set_quoted('LIBERATE_PACKAGE_URL', 'https://gitlab.com/interpeer/liberate')

### Host platform details
conf_data.set('LIBERATE_BIGENDIAN', host_machine.endian() == 'big')

### Headers
conf_data.set('LIBERATE_HAVE_ARPA_INET_H',
  compiler.has_header('arpa' / 'inet.h'))
conf_data.set('LIBERATE_HAVE_NETINET_IN_H',
  compiler.has_header('netinet' / 'in.h'))
conf_data.set('LIBERATE_HAVE_LINUX_UN_H',
  compiler.has_header('linux' / 'un.h'))
conf_data.set('LIBERATE_HAVE_SYS_UN_H',
  compiler.has_header('sys' / 'un.h'))
conf_data.set('LIBERATE_HAVE_SYS_SOCKET_H',
  compiler.has_header('sys' / 'socket.h'))
conf_data.set('LIBERATE_HAVE_WINSOCK2_H',
  compiler.has_header('winsock2.h'))
conf_data.set('LIBERATE_HAVE_WS2TCPIP_H',
  compiler.has_header('ws2tcpip.h'))
have_afunix = compiler.has_header('afunix.h')
conf_data.set('LIBERATE_HAVE_AFUNIX_H', have_afunix)


### Types

compiler.sizeof('int32_t', prefix: '#include <stdint.h>')
compiler.sizeof('uint32_t', prefix: '#include <stdint.h>')
compiler.sizeof('int64_t', prefix: '#include <stdint.h>')
compiler.sizeof('uint64_t', prefix: '#include <stdint.h>')
compiler.sizeof('size_t', prefix: '#include <stdint.h>')
compiler.sizeof('ssize_t', prefix: '#include <stdint.h>')
char8_size = compiler.sizeof('char8_t')
if char8_size > 0
  conf_data.set('LIBERATE_HAVE_CHAR8_T', true)
endif
wchar_size = compiler.sizeof('wchar_t')
if wchar_size > 0
  conf_data.set('LIBERATE_HAVE_WCHAR_T', true)
endif
stdbyte_size = compiler.sizeof('std::byte', prefix: '#include <cstddef>')
if stdbyte_size > 0
  conf_data.set('LIBERATE_HAVE_STD_BYTE', true)
endif

have_strerror_s = compiler.compiles('''
#include <string.h>

int main(int, char**)
{
  char buf[1];
  auto e = strerror_s(buf, sizeof(buf), 0);
}
''', name: 'strerror_s()')
conf_data.set('LIBERATE_HAVE_STRERROR_S', have_strerror_s)

have_strerror_r = compiler.compiles('''
#include <string.h>

#if (_POSIX_C_SOURCE >= 200112L) && !_GNU_SOURCE
#  error "No XSI compliant version!"
#endif

int main(int, char**)
{
  char buf[1];
  int e = strerror_r(0, buf, sizeof(buf));
}
''', name: 'strerror_r()')
conf_data.set('LIBERATE_HAVE_STRERROR_R', have_strerror_r)



have_getaddrinfo = compiler.compiles('''
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

int main(int, char**)
{
  auto res = getaddrinfo(nullptr, nullptr, nullptr, nullptr);
}
''', name: 'getaddrinfo()')
conf_data.set('LIBERATE_HAVE_GETADDRINFO', have_getaddrinfo)

have_ai_idn = compiler.compiles('''
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

int main(int, char**)
{
  int x = AI_IDN;
}
''', name: 'AI_IDN define')
conf_data.set('LIBERATE_HAVE_AI_IDN', have_ai_idn)

have_eai_addrfamily = compiler.compiles('''
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

int main(int, char**)
{
  int x = EAI_ADDRFAMILY;
}
''', name: 'EAI_ADDRFAMILY define')
conf_data.set('LIBERATE_HAVE_EAI_ADDRFAMILY', have_eai_addrfamily)

have_eai_nodata = compiler.compiles('''
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

int main(int, char**)
{
  int x = EAI_NODATA;
}
''', name: 'EAI_NODATA define')
conf_data.set('LIBERATE_HAVE_EAI_NODATA', have_eai_nodata)

have_eai_system = compiler.compiles('''
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

int main(int, char**)
{
  int x = EAI_SYSTEM;
}
''', name: 'EAI_SYSTEM define')
conf_data.set('LIBERATE_HAVE_EAI_SYSTEM', have_eai_system)


### Set values from options
log_backend = 'LIBERATE_LOG_BACKEND_' + get_option('log_backend').to_upper()
summary('Log backend', log_backend, section: 'Build options')
conf_data.set('LIBERATE_LOG_BACKEND', log_backend)

configure_file(
  input: 'build-config.h.in',
  output: 'build-config.h',
  configuration: conf_data
)

main_build_dir = declare_dependency(
  include_directories: include_directories('.')
)

##############################################################################
# Dependencies
threads = dependency('threads', required: true)

deps = [threads]

##############################################################################
# Meson Wrap Dependencies
define = '-D'
if compiler_id == 'msvc'
  define = '/D'
endif

log_deps = []
log_cpp_args = []
if log_backend == 'LIBERATE_LOG_BACKEND_PLOG'
  plog = subproject('sergiusthebest-plog')
  log_deps = [plog.get_variable('plog_dep')]
  log_cpp_args = [define + 'LIBERATE_LOG_BACKEND=LIBERATE_LOG_BACKEND_PLOG']
elif log_backend == 'LIBERATE_LOG_BACKEND_LOGURU' and not host_machine.system().endswith('bsd')
  loguru = subproject('emilk-loguru')
  log_deps = [loguru.get_variable('emilk_loguru_dep')]
  log_cpp_args = [define + 'LIBERATE_LOG_BACKEND=LIBERATE_LOG_BACKEND_LOGURU']
elif log_backend == 'LIBERATE_LOG_BACKEND_SPDLOG'
  spdlog = subproject('spdlog')
  log_deps = [spdlog.get_variable('spdlog_dep')]
  log_cpp_args = [define + 'LIBERATE_LOG_BACKEND=LIBERATE_LOG_BACKEND_SPDLOG']
endif

##############################################################################
# Library

includes = include_directories(
  'include',
)

install_headers(
  'include' / 'liberate.h',
)

install_headers(
  'include' / 'liberate' / 'version.h',
  'include' / 'liberate' / 'visibility.h',
  'include' / 'liberate' / 'types.h',
  'include' / 'liberate' / 'logging.h',

  subdir: 'liberate',
)

install_headers(
  'include' / 'liberate' / 'types' / 'varint.h',
  'include' / 'liberate' / 'types' / 'type_traits.h',
  'include' / 'liberate' / 'types' / 'byte.h',

  subdir: 'liberate' / 'types',
)

install_headers(
  'include' / 'liberate' / 'string' / 'util.h',
  'include' / 'liberate' / 'string' / 'utf8.h',
  'include' / 'liberate' / 'string' / 'urlencode.h',
  'include' / 'liberate' / 'string' / 'hexencode.h',

  subdir: 'liberate' / 'string',
)

install_headers(
  'include' / 'liberate' / 'fs' / 'path.h',
  'include' / 'liberate' / 'fs' / 'tmp.h',

  subdir: 'liberate' / 'fs',
)

install_headers(
  'include' / 'liberate' / 'sys' / 'error.h',

  subdir: 'liberate' / 'sys',
)

install_headers(
  'include' / 'liberate' / 'net' / 'address_type.h',
  'include' / 'liberate' / 'net' / 'socket_address.h',
  'include' / 'liberate' / 'net' / 'network.h',
  'include' / 'liberate' / 'net' / 'url.h',
  'include' / 'liberate' / 'net' / 'ip.h',
  'include' / 'liberate' / 'net' / 'resolve.h',

  subdir: 'liberate' / 'net',
)

install_headers(
  'include' / 'liberate' / 'cpp' / 'hash.h',
  'include' / 'liberate' / 'cpp' / 'operators.h',

  subdir: 'liberate' / 'cpp',
)

install_headers(
  'include' / 'liberate' / 'cpp' / 'operators' / 'comparison.h',

  subdir: 'liberate' / 'cpp' / 'operators',
)

install_headers(
  'include' / 'liberate' / 'serialization' / 'integer.h',
  'include' / 'liberate' / 'serialization' / 'varint.h',

  subdir: 'liberate' / 'serialization',
)

install_headers(
  'include' / 'liberate' / 'concurrency' / 'concurrent_queue.h',
  'include' / 'liberate' / 'concurrency' / 'tasklet.h',
  'include' / 'liberate' / 'concurrency' / 'lock_policy.h',

  subdir: 'liberate' / 'concurrency',
)

install_headers(
  'include' / 'liberate' / 'checksum' / 'crc32.h',

  subdir: 'liberate' / 'checksum',
)

install_headers(
  'include' / 'liberate' / 'random' / 'unsafe_bits.h',

  subdir: 'liberate' / 'random',
)

install_headers(
  'include' / 'liberate' / 'timeout' / 'exponential_backoff.h',

  subdir: 'liberate' / 'timeout',
)



libsrc = [
  'lib' / 'version.cpp',
  'lib' / 'liberate.cpp',
  'lib' / 'string' / 'util.cpp',
  'lib' / 'string' / 'utf8.cpp',
  'lib' / 'string' / 'urlencode.cpp',
  'lib' / 'string' / 'hexencode.cpp',
  'lib' / 'fs' / 'path.cpp',
  'lib' / 'fs' / 'tmp.cpp',
  'lib' / 'sys' / 'error.cpp',
  'lib' / 'net' / 'cidr.cpp',
  'lib' / 'net' / 'socket_address.cpp',
  'lib' / 'net' / 'network.cpp',
  'lib' / 'net' / 'url.cpp',
  'lib' / 'net' / 'ip.cpp',
  'lib' / 'net' / 'resolve.cpp',
  'lib' / 'concurrency' / 'tasklet.cpp',
]


lib = library('erate', libsrc,
    include_directories: [includes],
    dependencies: deps + log_deps,
    link_args: link_args,
    cpp_args: cpp_lib_is_building + log_cpp_args,
    version: LIB_VERSION,
    soversion: ABI_VERSION,
    install: true)

liberate_dep = declare_dependency(
    include_directories: [includes],
    dependencies: deps + log_deps,
    compile_args: log_cpp_args,
    link_with: [lib],
    link_args: link_args,
    version: LIB_VERSION,
)


##############################################################################
# Subdirs
subdir('test')
subdir('examples')

##############################################################################
# Linter, etc.
cppcheck = find_program('cppcheck', required: false)

if cppcheck.found()
  run_target('cppcheck', command: [
    cppcheck.path(),
    '--cppcheck-build-dir=@0@/cppcheck'.format(meson.current_build_dir()),
    '--enable=all', '-v',
    '--suppress=missingIncludeSystem',
    '--inline-suppr',
    '-I', '@0@/include'.format(meson.current_source_dir()),
    '-I', '@0@/lib'.format(meson.current_source_dir()),
    '-I', meson.current_build_dir(),
    '--std=c++17',
    cpp_lib_is_building,
    '--quiet',
    '@0@/lib'.format(meson.current_source_dir()),
  ])
endif

oclint = find_program('oclint', required: false)

if oclint.found()
  oclint_config = custom_target('oclint_config',
      output: '.oclint',
      input: '.oclint',
      command: ['cp', '@INPUT@', '@OUTPUT@'],
      install: false,
  )

  run_target('oclint', command: [
      oclint.path(),
      libsrc,
      '-o', '@0@/oclint.log'.format(meson.current_build_dir()),
      '--',
      '-I', '@0@/include'.format(meson.current_source_dir()),
      '-I', '@0@/lib'.format(meson.current_source_dir()),
      '-I', meson.current_build_dir(),
      '--std=@0@'.format(get_option('cpp_std')),
      cpp_args,
      cpp_lib_is_building,
      '-DOCLINT_IS_RUNNING',
    ],
    depends: oclint_config,
  )
endif
