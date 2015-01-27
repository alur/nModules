#include "DesktopPane.hpp"
#include "Workarea.h"

#include "../nCoreApi/Core.h"

#include "../nShared/LsModule.hpp"

#include "../nUtilities/lsapi.h"
#include "../nUtilities/Macros.h"
#include "../nUtilities/Windows.h"

static const UINT sLsMessages[] = { LM_GETREVID, LM_REFRESH, 0 };
LsModule gLsModule(L"nDesk", MakeVersion(0, 9, 0, 0));
static DesktopPane *sDesktopPane = nullptr;


BOOL APIENTRY DllMain(HANDLE module, DWORD reasonForCall, LPVOID /* reserved */) {
  if (reasonForCall == DLL_PROCESS_ATTACH) {
#ifdef _DLL
    DisableThreadLibraryCalls((HINSTANCE)module);
#endif
  }
  return TRUE;
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
    return gLsModule.HandleGetRevId(lParam);

  case LM_REFRESH:
    ReloadWorkareas();
    return 0;
  }

  return DefWindowProc(window, message, wParam, lParam);
}


EXPORT_CDECL(int) initModuleW(HWND /* parent */, HINSTANCE /* instance */, LPCWSTR /* path */) {
  if (FAILED(nCore::Connect(MakeVersion(0, 9, 0, 0)))) {
    gLsModule.DestroyMessageHandler();
    return 1;
  }

  if (FAILED(gLsModule.CreateMessageHandler(nCore::GetInstance(), MessageHandler))) {
    return 1;
  }

  sDesktopPane = new DesktopPane();
  LoadWorkareas();

  return 0;
}


EXPORT_CDECL(void) quitModule(HINSTANCE /* instance */) {
  SAFEDELETE(sDesktopPane);
  ClearWorkareas();
  nCore::Disconnect();
  gLsModule.DestroyMessageHandler();
}
