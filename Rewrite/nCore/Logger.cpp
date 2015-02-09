#include "Logger.hpp"

#include "../nUtilities/Macros.h"

#include <stdlib.h>


EXPORT_CDECL(ILogger*) CreateLogger(LPCWSTR name) {
  return new Logger(name);
}

Logger::Logger(LPCWSTR name) : mName(_wcsdup(name)) {}


Logger::~Logger() {
  free(mName);
}


void Logger::Destroy() {
  delete this;
}


void Logger::Debug(LPCWSTR, ...) {
}


void Logger::DebugHR(HRESULT, LPCWSTR, ...) {
}


void Logger::Info(LPCWSTR, ...) {
}


void Logger::InfoHR(HRESULT, LPCWSTR, ...) {
}


void Logger::Warning(LPCWSTR, ...) {
}


void Logger::WarningHR(HRESULT, LPCWSTR, ...) {
}


void Logger::Error(LPCWSTR, ...) {
}


void Logger::ErrorHR(HRESULT, LPCWSTR, ...) {
}
