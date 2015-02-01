#include "LiteStep.h"

#include "../nUtilities/lsapi.h"

#include <strsafe.h>


HRESULT CreateMessageHandler(HINSTANCE coreInstance, LPCWSTR name, WNDPROC wndProc, HWND &window) {
  window = CreateWindowEx(WS_EX_TOOLWINDOW | WS_EX_NOREDIRECTIONBITMAP, L"LSnModuleMsgHandler",
    name, WS_POPUP, 0, 0, 0, 0, nullptr, nullptr, coreInstance, wndProc);
  if (window == nullptr) {
    HRESULT hr = HRESULT_FROM_WIN32(GetLastError());
    return hr;
  }

  SetWindowLongPtr(window, GWLP_USERDATA, MAGIC_DWORD);
  return S_OK;
}


LRESULT HandleGetRevId(LPCWSTR name, VERSION version, LPARAM lParam) {
  size_t length;
  LPCWSTR format = GetBuildVersion(version) ? L"%s %u.%u.%u.%u" :
    GetPatchVersion(version) ? L"%s %u.%u.%u" : L"%s %u.%u";
  if (SUCCEEDED(StringCchPrintf((LPWSTR)lParam, MAX_GETREVID, format, name,
    GetMajorVersion(version), GetMinorVersion(version), GetPatchVersion(version),
    GetBuildVersion(version)))) {
    if (SUCCEEDED(StringCchLength((LPWSTR)lParam, MAX_GETREVID, &length))) {
      return length;
    }
  }
  return 0;
}
