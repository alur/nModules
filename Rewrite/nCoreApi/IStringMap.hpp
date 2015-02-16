#pragma once

#include "ApiDefs.h"

#include "../nUtilities/Windows.h"

class IStringMap {
public:
  virtual bool APICALL Get(LPCWSTR key, LPWSTR buffer, size_t cchBuffer) const = 0;
};
