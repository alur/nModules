#include "LsModule.hpp"

#include "../nUtilities/hresults.h"
#include "../nUtilities/lsapi.h"

#include <strsafe.h>


LsModule::LsModule(const LPCWSTR name, VERSION version)
  : mMessageWindow(nullptr)
  , mName(name)
  , mVersion(version) {}


HRESULT LsModule::CreateMessageHandler(HINSTANCE coreInstance, WNDPROC wndProc) {
  mMessageWindow = CreateWindowEx(WS_EX_TOOLWINDOW | WS_EX_NOREDIRECTIONBITMAP,
    L"LSnModuleMsgHandler", mName, WS_POPUP, 0, 0, 0, 0, nullptr, nullptr, coreInstance, wndProc);
  if (mMessageWindow == nullptr) {
    HRESULT hr = HRESULT_FROM_WIN32(GetLastError());
    return hr;
  }

  SetWindowLongPtr(mMessageWindow, GWLP_USERDATA, MAGIC_DWORD);
  return S_OK;
}


void LsModule::DestroyMessageHandler() {
  if (mMessageWindow != nullptr) {
    DestroyWindow(mMessageWindow);
    mMessageWindow = nullptr;
  }
}


HWND LsModule::GetMessageWindow() const {
  return mMessageWindow;
}


LRESULT LsModule::HandleGetRevId(LPARAM lParam) const {
  size_t length;
  LPCWSTR format = GetBuildVersion(mVersion) ? L"%s %u.%u.%u.%u" :
    GetPatchVersion(mVersion) ? L"%s %u.%u.%u" : L"%s %u.%u";
  if (SUCCEEDED(StringCchPrintf((LPWSTR)lParam, MAX_GETREVID, format, mName,
    GetMajorVersion(mVersion), GetMinorVersion(mVersion), GetPatchVersion(mVersion),
    GetBuildVersion(mVersion)))) {
    if (SUCCEEDED(StringCchLength((LPWSTR)lParam, MAX_GETREVID, &length))) {
      return length;
    }
  }
  return 0;
}
