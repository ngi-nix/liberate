// Define before including spdlog.h - compile time cutoff
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE

#include <liberate/logging.h>

#if LIBERATE_LOG_BACKEND == LIBERATE_LOG_BACKEND_PLOG
#  include <plog/Formatters/TxtFormatter.h>
#  include <plog/Appenders/ConsoleAppender.h>

#elif LIBERATE_LOG_BACKEND == LIBERATE_LOG_BACKEND_SPDLOG

#elif LIBERATE_LOG_BACKEND == LIBERATE_LOG_BACKEND_LOGURU
#undef LIBLOG_LEVEL_FATAL
#define LIBLOG_LEVEL_FATAL ERROR
#include <iostream>

#endif



int main(int, char **)
{
#if LIBERATE_LOG_BACKEND == LIBERATE_LOG_BACKEND_PLOG
  static plog::ConsoleAppender<plog::TxtFormatter> consoleAppender;
  plog::init(plog::verbose, &consoleAppender);

#elif LIBERATE_LOG_BACKEND == LIBERATE_LOG_BACKEND_SPDLOG
  spdlog::set_level(spdlog::level::trace);

#elif LIBERATE_LOG_BACKEND == LIBERATE_LOG_BACKEND_LOGURU
  loguru::g_stderr_verbosity = loguru::Verbosity_MAX;
  std::cerr << "WARNING: FATAL logs are redirected to ERROR for this example." << std::endl;

#endif

  LIBLOG(LIBLOG_LEVEL_TRACE, "This is a verbose trace message.");
  LIBLOG(LIBLOG_LEVEL_DEBUG, "This is a verbose debug message.");
  LIBLOG(LIBLOG_LEVEL_INFO, "This is a verbose info message.");
  LIBLOG(LIBLOG_LEVEL_WARN, "This is a verbose warning message.");
  LIBLOG(LIBLOG_LEVEL_ERROR, "This is a verbose error message.");
  LIBLOG(LIBLOG_LEVEL_FATAL, "This is a verbose fatal message.");

  LIBLOG_TRACE("This is a trace message.");
  LIBLOG_DEBUG("This is a debug message.");
  LIBLOG_INFO("This is an info message.");
  LIBLOG_WARN("This is a warning message.");
  LIBLOG_ERROR("This is an error message.");
  LIBLOG_FATAL("This is a fatal message.");

  LLOG_T("This is a short trace message.");
  LLOG_D("This is a short debug message.");
  LLOG_I("This is a short info message.");
  LLOG_W("This is a short warning message.");
  LLOG_E("This is a short error message.");
  LLOG_F("This is a short fatal message.");

#if defined(LIBERATE_WIN32)
  LIBLOG_ERR(ERROR_INVALID_PARAMETER, "System error code.");
#else
  LIBLOG_ERR(EINVAL, "System error code.");
#endif

  LIBLOG_ERRNO("Errno log message");

  LIBLOG_EXC(std::runtime_error{"runtime error message"}, "Exception logging");
}
