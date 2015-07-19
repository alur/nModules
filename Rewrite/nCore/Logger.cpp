#include "Logger.hpp"

#include "../Headers/Macros.h"

#include <stdlib.h>


EXPORT_CDECL(ILogger*) CreateLogger(LPCWSTR name) {
  return new Logger(name);
}


Logger::Logger(LPCWSTR name) : mName(name) {}


Logger::~Logger() {
}


void Logger::Discard() {
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
