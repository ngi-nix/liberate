
##############################################################################
# Examples

# Only build if it's the main project
if not meson.is_subproject()

  define = '-D'
  if compiler_id == 'msvc'
    define = '/D'
  endif

  # Stderr
  example_args = [define + 'LIBERATE_LOG_BACKEND=LIBERATE_LOG_BACKEND_STDERR']
  executable('log_stderr', ['logging' / 'main.cpp'],
    dependencies: [liberate_dep],
    cpp_args: example_args,
    link_args: link_args,
  )

  # Plog
  if host_type != 'win32'
    plog = subproject('plog')
    example_args = [define + 'LIBERATE_LOG_BACKEND=LIBERATE_LOG_BACKEND_PLOG']
    executable('log_plog', ['logging' / 'main.cpp'],
      dependencies: [
        liberate_dep,
        plog.get_variable('plog_dep'),
      ],
      cpp_args: example_args,
      link_args: link_args,
    )
  endif

  # Loguru
  if host_type != 'android' and host_type != 'win32'
    loguru = subproject('loguru',
      default_options: [
        'loguru_with_streams=true',
      ],
    )
    example_args = [define + 'LIBERATE_LOG_BACKEND=LIBERATE_LOG_BACKEND_LOGURU']
    executable('log_loguru', ['logging' / 'main.cpp'],
      dependencies: [
        liberate_dep,
        loguru.get_variable('emilk_loguru_dep'),
      ],
      cpp_args: example_args,
      link_args: link_args,
    )
  endif

  # spdlog
  spdlog = subproject('spdlog',
    default_options: [
      'enable_examples=false',
      'enable_benchmarks=false',
      'enable_tests=false',
  ])

  example_args = [define + 'LIBERATE_LOG_BACKEND=LIBERATE_LOG_BACKEND_SPDLOG']
  executable('log_spdlog', ['logging' / 'main.cpp'],
    dependencies: [
      liberate_dep,
      spdlog.get_variable('spdlog_dep'),
    ],
    cpp_args: example_args,
    link_args: link_args,
  )

endif
