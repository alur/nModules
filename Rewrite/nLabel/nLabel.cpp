#include "Label.hpp"

#include "../nCoreApi/Core.h"

#include "../nShared/LiteStep.h"

#include "../nUtilities/lsapi.h"
#include "../nUtilities/Macros.h"
#include "../nUtilities/Windows.h"

#include <unordered_map>

static const UINT sLsMessages[] = { LM_GETREVID, LM_REFRESH, 0 };
static const wchar_t sName[] = L"nLabel";
static const VERSION sVersion = MakeVersion(0, 9, 0, 0);
static const VERSION sCoreVersion = MakeVersion(0, 9, 0, 0);

HWND gWindow = nullptr;
std::unordered_map<std::wstring, Label> gLabels;


BOOL APIENTRY DllMain(HANDLE module, DWORD reasonForCall, LPVOID /* reserved */) {
  if (reasonForCall == DLL_PROCESS_ATTACH) {
#ifdef _DLL
    DisableThreadLibraryCalls((HINSTANCE)module);
#endif
  }
  return TRUE;
}


static void APICALL CreateLabel(LPCWSTR name, LPARAM) {
  gLabels.emplace(
    std::piecewise_construct, std::forward_as_tuple(name), std::forward_as_tuple(name));
}


static void LoadSettings() {
  nCore::EnumRCLineTokens(L"*nLabel", CreateLabel, 0);
}


static LRESULT WINAPI MessageHandler(HWND window, UINT message, WPARAM wParam, LPARAM lParam) {
  switch (message) {
  case WM_CREATE:
    SendMessage(GetLitestepWnd(), LM_REGISTERMESSAGE, WPARAM(window), LPARAM(sLsMessages));
    return 0;

  case WM_DESTROY:
    SendMessage(GetLitestepWnd(), LM_UNREGISTERMESSAGE, WPARAM(window), LPARAM(sLsMessages));
    return 0;

  case LM_GETREVID:
    return HandleGetRevId(sName, sVersion, lParam);

  case LM_REFRESH:
    gLabels.clear();
    LoadSettings();
    return 0;
  }

  return DefWindowProc(window, message, wParam, lParam);
}


EXPORT_CDECL(int) initModuleW(HWND /* parent */, HINSTANCE /* instance */, LPCWSTR /* path */) {
  if (FAILED(nCore::Connect(sCoreVersion))) {
    return 1;
  }

  if (FAILED(CreateMessageHandler(nCore::GetInstance(), sName, MessageHandler, gWindow))) {
    return 1;
  }

  LoadSettings();

  return 0;
}


EXPORT_CDECL(void) quitModule(HINSTANCE /* instance */) {
  gLabels.clear();
  nCore::Disconnect();
  DestroyWindow(gWindow);
  gWindow = nullptr;
}
