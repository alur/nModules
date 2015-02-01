#pragma once

#include "ApiDefs.h"

#include "../nUtilities/Windows.h"

class ILogger {
public:
  virtual void Destroy() = 0;
  virtual void APICALL Debug(LPCWSTR, ...) = 0;
  virtual void APICALL Info(LPCWSTR, ...) = 0;
  virtual void APICALL Warning(LPCWSTR, ...) = 0;
  virtual void APICALL Error(LPCWSTR, ...) = 0;
};
