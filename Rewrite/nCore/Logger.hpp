#pragma once

#include "../nCoreApi/ILogger.hpp"

#include <string>

class Logger : public ILogger {
public:
  explicit Logger(LPCWSTR name);
  ~Logger();

  // IDiscardable
public:
  void APICALL Discard() override;

  // ILogger
public:
  void APICALL Debug(LPCWSTR, ...) override;
  void APICALL DebugHR(HRESULT, LPCWSTR, ...) override;
  void APICALL Info(LPCWSTR, ...) override;
  void APICALL InfoHR(HRESULT, LPCWSTR, ...) override;
  void APICALL Warning(LPCWSTR, ...) override;
  void APICALL WarningHR(HRESULT, LPCWSTR, ...) override;
  void APICALL Error(LPCWSTR, ...) override;
  void APICALL ErrorHR(HRESULT, LPCWSTR, ...) override;

private:
  std::wstring mName;
};
