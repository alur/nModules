#pragma once

#include "ApiDefs.h"
#include "IDiscardable.hpp"

#include "../Headers/Windows.h"

class ILogger : public IDiscardable {
public:
  virtual void APICALL Debug(LPCWSTR, ...) = 0;
  virtual void APICALL Info(LPCWSTR, ...) = 0;
  virtual void APICALL Warning(LPCWSTR, ...) = 0;
  virtual void APICALL Error(LPCWSTR, ...) = 0;

  virtual void APICALL DebugHR(HRESULT, LPCWSTR, ...) = 0;
  virtual void APICALL InfoHR(HRESULT, LPCWSTR, ...) = 0;
  virtual void APICALL WarningHR(HRESULT, LPCWSTR, ...) = 0;
  virtual void APICALL ErrorHR(HRESULT, LPCWSTR, ...) = 0;
};
