#include "Error.h"

#include <strsafe.h>


HRESULT DescriptionFromHR(HRESULT hr, LPWSTR buf, size_t cchBuf) {
  if (FACILITY_WINDOWS == HRESULT_FACILITY(hr)) {
    hr = HRESULT_CODE(hr);
  }

  if (FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_FROM_HMODULE,
      GetModuleHandle(NULL), hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), buf, DWORD(cchBuf),
      nullptr) == 0) {
    return StringCchPrintf(buf, cchBuf, L"Unknown error, 0x%.8X", hr);
  }

  return S_OK;
}
