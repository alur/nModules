#include "../nModuleBase/nModule.hpp"

#include "../nShared/LiteStep.h"

#include "../nCoreApi/Core.h"

#include "../nUtilities/lsapi.h"
#include "../nUtilities/Macros.h"

static const UINT sLsMessages[] = { LM_GETREVID, LM_REFRESH, LM_SYSTRAY, LM_SYSTRAYINFOEVENT, 0 };
//static TrayManager *sTrayManager = nullptr;

NModule gModule(L"nTray", MakeVersion(0, 9, 0, 0), MakeVersion(0, 9, 0, 0));


static void APICALL CreateTray(LPCWSTR, LPARAM) {
  //sTrayManager->Create(name);
}


static void LoadSettings() {
  nCore::EnumRCLineTokens(L"*nTray", CreateTray, 0);
}


int nModuleInit(NModule&) {
  LoadSettings();
  return 0;
}


void nModuleQuit(NModule&) {
}


LRESULT WINAPI MessageHandlerProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam) {
  switch (message) {
  case WM_CREATE:
    SendMessage(GetLitestepWnd(), LM_REGISTERMESSAGE, WPARAM(window), LPARAM(sLsMessages));
    //sTrayManager = new TrayManager(window);
    return 0;

  case WM_DESTROY:
    SendMessage(GetLitestepWnd(), LM_UNREGISTERMESSAGE, WPARAM(window), LPARAM(sLsMessages));
    //delete sTrayManager;
    //sTrayManager = nullptr;
    return 0;

  case LM_GETREVID:
    return HandleGetRevId(gModule.name, gModule.version, lParam);

  case LM_REFRESH:
    //sTrayManager->DestroyAll();
    LoadSettings();
    return 0;

  case LM_SYSTRAY:
  case LM_SYSTRAYINFOEVENT:
    //return sTrayManager->HandleMessage(window, message, wParam, lParam);
    break;
  }

  return DefWindowProc(window, message, wParam, lParam);
}
