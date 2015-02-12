#include "Messages.h"
#include "TrayManager.hpp"

#include "../nModuleBase/nModule.hpp"

#include "../nShared/LiteStep.h"

#include "../nCoreApi/Core.h"

#include "../nUtilities/lsapi.h"
#include "../nUtilities/Macros.h"

static const UINT sLsMessages[] = { LM_GETREVID, LM_REFRESH, LM_SYSTRAY, LM_SYSTRAYINFOEVENT, 0 };
static TrayManager *sTrayManager = nullptr;

NModule gModule(L"nTray", MakeVersion(0, 9, 0, 0), MakeVersion(0, 9, 0, 0));
HWND gTrayNotifyWindow;


static void APICALL CreateTray(LPCWSTR name, LPARAM) {
  sTrayManager->CreateTray(name);
}


static void LoadSettings() {
  nCore::EnumRCLineTokens(L"*nTray", CreateTray, 0);
}


int nModuleInit(NModule&) {
  LoadSettings();
  gTrayNotifyWindow = (HWND)SendMessage(GetLitestepWnd(), LM_SYSTRAYREADY, 0, 0);
  PostMessage(gModule.window, NTRAY_GOT_INITIAL_ICONS, 0, 0);
  return 0;
}


void nModuleQuit(NModule&) {
}


LRESULT WINAPI MessageHandlerProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam) {
  switch (message) {
  case WM_CREATE:
    SendMessage(GetLitestepWnd(), LM_REGISTERMESSAGE, WPARAM(window), LPARAM(sLsMessages));
    sTrayManager = new TrayManager();
    return 0;

  case WM_DESTROY:
    SendMessage(GetLitestepWnd(), LM_UNREGISTERMESSAGE, WPARAM(window), LPARAM(sLsMessages));
    SAFEDELETE(sTrayManager);
    return 0;

  case LM_GETREVID:
    return HandleGetRevId(gModule.name, gModule.version, lParam);

  case LM_REFRESH:
    sTrayManager->DestroyAllTrays();
    LoadSettings();
    return 0;

  case LM_SYSTRAY:
  case LM_SYSTRAYINFOEVENT:
  case NTRAY_GOT_INITIAL_ICONS:
    return sTrayManager->HandleMessage(window, message, wParam, lParam);
    break;
  }

  return DefWindowProc(window, message, wParam, lParam);
}
