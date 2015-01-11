//-------------------------------------------------------------------------------------------------
// /Utilities/Error.cpp
// The nModules Project
//
// Functions for dealing with errors.
//-------------------------------------------------------------------------------------------------
#include "Error.h"

#include <strsafe.h>


HRESULT DescriptionFromHR(HRESULT hr, LPTSTR buf, size_t cchBuf) {
  if (FACILITY_WINDOWS == HRESULT_FACILITY(hr)) {
    hr = HRESULT_CODE(hr);
  }

  if (FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, nullptr, hr,
    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), buf, DWORD(cchBuf), nullptr) == 0) {
    return StringCchPrintf(buf, cchBuf, L"Unknown error, 0x%.8X", hr);
  }

  return S_OK;
}
