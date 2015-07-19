#include "Label.hpp"

#include "../nModuleBase/nModule.hpp"

#include "../nShared/LiteStep.h"

#include "../nCoreApi/Core.h"

#include "../Headers/lsapi.h"
#include "../Headers/Macros.h"

#include <unordered_map>

NModule gModule(L"nLabel", MakeVersion(1, 0, 0, 0), MakeVersion(1, 0, 0, 0));
std::unordered_map<std::wstring, Label> gLabels;

static const UINT sLsMessages[] = { LM_GETREVID, LM_REFRESH, 0 };


static void APICALL CreateLabel(LPCWSTR name, LPARAM) {
  gLabels.emplace(
    std::piecewise_construct, std::forward_as_tuple(name), std::forward_as_tuple(name));
}


static void LoadSettings() {
  nCore::EnumRCLineTokens(L"*nLabel", CreateLabel, 0);
}


LRESULT WINAPI MessageHandlerProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam) {
  switch (message) {
  case WM_CREATE:
    SendMessage(GetLitestepWnd(), LM_REGISTERMESSAGE, WPARAM(window), LPARAM(sLsMessages));
    LoadSettings();
    return 0;

  case WM_DESTROY:
    gLabels.clear();
    SendMessage(GetLitestepWnd(), LM_UNREGISTERMESSAGE, WPARAM(window), LPARAM(sLsMessages));
    return 0;

  case LM_GETREVID:
    return HandleGetRevId(gModule.name, gModule.version, lParam);

  case LM_REFRESH:
    gLabels.clear();
    LoadSettings();
    return 0;
  }

  return DefWindowProc(window, message, wParam, lParam);
}
