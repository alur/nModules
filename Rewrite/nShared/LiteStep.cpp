#include "Error.h"
#include "LiteStep.h"

#include "../Headers/hresults.h"
#include "../Headers/lsapi.h"

#include <strsafe.h>


HRESULT CreateMessageHandler(HINSTANCE coreInstance, LPCWSTR name, WNDPROC wndProc, HWND &window) {
  window = CreateWindowEx(WS_EX_TOOLWINDOW | WS_EX_NOREDIRECTIONBITMAP, L"LSnModuleMsgHandler",
    name, WS_POPUP, 0, 0, 0, 0, nullptr, nullptr, coreInstance, wndProc);
  if (window == nullptr) {
    return HrGetLastError();
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


void HandleCoreConnectionError(LPCWSTR module, VERSION version, HRESULT hr) {
  wchar_t title[128], message[4096];

  switch (hr) {
  default:
    DescriptionFromHR(hr, message, _countof(message));
    break;
  case E_API_CORE_NOT_FOUND:
    StringCchCopy(message, _countof(message), L"The core was not found. Is it loaded?");
    break;
  case E_API_FUNC_NOT_FOUND:
    StringCchCopy(message, _countof(message), L"An expected nCore API was missing!");
    break;
  }
  StringCchPrintf(title, _countof(title), L"%s: nCore connection failed", module);

  MessageBox(nullptr, message, title, MB_OK | MB_ICONERROR);
}
