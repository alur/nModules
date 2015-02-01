#pragma once

#include "../nCoreApi/ILogger.hpp"

class Logger : public ILogger {
public:
  explicit Logger(LPCWSTR name);
  ~Logger();

  // ILogger
public:
  void APICALL Destroy();
  void APICALL Debug(LPCWSTR, ...) override;
  void APICALL Info(LPCWSTR, ...) override;
  void APICALL Warning(LPCWSTR, ...) override;
  void APICALL Error(LPCWSTR, ...) override;

private:
  LPWSTR mName;
};
