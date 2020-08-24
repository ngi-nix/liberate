##############################################################################
# Tests

# Tests in a subproject are complicicated. You need all the compile and
# link flags from the enclosing project, etc.
if not meson.is_subproject()
  gtest = subproject('gtest')

  # See https://github.com/google/googletest/issues/813
  test_args = []
  if host_machine.system() == 'cygwin'
    test_args += ['-D_POSIX_C_SOURCE=200809L']
  endif

  # Google test issues this warning; disable it in *test* code only.
  if compiler_id == 'msvc'
    test_args = [
      '/wd4389',
    ]
  endif

  test_src = [
    'cpp' / 'hash.cpp',
    'cpp' / 'operators' / 'comparison.cpp',
    'string' / 'util.cpp',
    'string' / 'utf8.cpp',
    'string' / 'urlencode.cpp',
    'fs' / 'path.cpp',
    'fs' / 'tmp.cpp',
    'net' / 'socket_address.cpp',
    'net' / 'network.cpp',
    'net' / 'url.cpp',
    'net' / 'ip.cpp',
    'types' / 'varint.cpp',
    'types' / 'type_traits.cpp',
    'serialization' / 'integer.cpp',
    'serialization' / 'varint.cpp',
    'runner.cpp',
  ]

  # Additional tests only if -DDEBUG is given
  if bt in ['debug', 'debugoptimized']
    test_src += [
      'net' / 'cidr.cpp',
    ]
  endif

  unittests = executable('unittests', test_src,
      dependencies: [
        main_build_dir,
        liberate_dep,
        gtest.get_variable('gtest_dep'),
      ],
      cpp_args: test_args + cpp_lib_is_building,
  )
  test('unittests', unittests)
endif
