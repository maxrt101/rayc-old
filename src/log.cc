#include <rayc/log.h>

#include <cctype>
#include <cstdio>
#include <algorithm>
#include <SDL2/SDL.h>

#define _VLOGF_INTERNAL_F(level, format) \
  va_list args; \
  va_start(args, format); \
  vlogf(level, format, args); \
  va_end(args);

#define _VLOGF_INTERNAL(level) _VLOGF_INTERNAL_F(level, format)

static rayc::logger::LogLevel g_logLevel = rayc::logger::LogLevel::WARNING;


rayc::logger::LogLevel rayc::logger::stringToLogLevel(const std::string& s) {
  std::string str = s;
  std::transform(str.begin(), str.end(), str.begin(), tolower);
  if (str == "debug") {
    return LogLevel::DEBUG;
  } else if (str == "info") {
    return LogLevel::INFO;
  } else if (str == "warning") {
    return LogLevel::WARNING;
  } else if (str == "error") {
    return LogLevel::ERROR;
  } else if (str == "fatal") {
    return LogLevel::FATAL;
  }
  return LogLevel::DEBUG;
}

rayc::logger::LogLevel rayc::logger::getLogLevel() {
  return g_logLevel;
}

void rayc::logger::setLogLevel(LogLevel level) {
  g_logLevel = level;
}

void rayc::logger::debug(const std::string format, ...) {
  _VLOGF_INTERNAL(LogLevel::DEBUG);
}

void rayc::logger::info(const std::string format, ...) {
  _VLOGF_INTERNAL(LogLevel::INFO);
}

void rayc::logger::warning(const std::string format, ...) {
  _VLOGF_INTERNAL(LogLevel::WARNING);
}

void rayc::logger::error(const std::string format, ...) {
  _VLOGF_INTERNAL(LogLevel::ERROR);
}

void rayc::logger::fatal(const std::string format, ...) {
  _VLOGF_INTERNAL(LogLevel::FATAL);
}

void rayc::logger::sdlError(std::string format, ...) {
  format += " (";
  format += SDL_GetError();
  format += ")";
  _VLOGF_INTERNAL(LogLevel::ERROR);
}

void rayc::logger::sdlFatal(std::string format, ...) {
  format += " (";
  format += SDL_GetError();
  format += ")";
  _VLOGF_INTERNAL(LogLevel::FATAL);
}

void rayc::logger::logf(LogLevel level, const std::string format, ...) {
  va_list args;
  va_start(args, format);
  vlogf(level, format, args);
  va_end(args);
}

void rayc::logger::vlogf(LogLevel level, const std::string& format, va_list args) {
  if (level < g_logLevel) return;

  FILE* dest = stdout;
  if (level > LogLevel::INFO) {
    dest = stderr;
  }

  vflogf(dest, level, format, args);
}

void rayc::logger::vflogf(FILE* dest, LogLevel level, const std::string& format, va_list args) {
  switch (level) {
    case LogLevel::FATAL:   fprintf(dest, "\033[41mFATAL");   break;
    case LogLevel::ERROR:   fprintf(dest, "\033[31mERROR");   break;
    case LogLevel::WARNING: fprintf(dest, "\033[33mWARNING"); break;
    case LogLevel::INFO:    fprintf(dest, "\033[36mINFO");    break;
    case LogLevel::DEBUG:   fprintf(dest, "\033[34mDEBUG");   break;
    default:                fprintf(dest, "INVALID_LOG_LEVEL");     break;
  }

  fprintf(dest, "\033[0m: ");
  vfprintf(dest, format.c_str(), args);
  fprintf(dest, "\n");
}
