#include "DesktopPane.hpp"
#include "Workarea.h"

#include "../nModuleBase/nModule.hpp"

#include "../nShared/LiteStep.h"

#include "../nCoreApi/Core.h"
#include "../nCoreApi/Messages.h"

#include "../nUtilities/lsapi.h"
#include "../nUtilities/Macros.h"
#include "../nUtilities/Windows.h"

NModule gModule(L"nDesk", MakeVersion(0, 9, 0, 0), MakeVersion(0, 9, 0, 0));

static const UINT sLsMessages[] = { LM_GETREVID, LM_REFRESH, 0 };
static const UINT sCoreMessages[] = { NCORE_DISPLAYS_CHANGED, 0 };

static DesktopPane *sDesktopPane = nullptr;


int nModuleInit(NModule&) {
  sDesktopPane = new DesktopPane();
  LoadWorkareas();
  return 0;
}


void nModuleQuit(NModule&) {
  SAFEDELETE(sDesktopPane);
  ClearWorkareas();
}


LRESULT WINAPI MessageHandlerProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam) {
  switch (message) {
  case LM_GETREVID:
    return HandleGetRevId(gModule.name, gModule.version, lParam);

  case LM_REFRESH:
    LoadWorkareas();
    return 0;

  case NCORE_DISPLAYS_CHANGED:
    LoadWorkareas();
    return 0;

  case WM_CREATE:
    SendMessage(GetLitestepWnd(), LM_REGISTERMESSAGE, WPARAM(window), LPARAM(sLsMessages));
    nCore::RegisterForMessages(window, sCoreMessages);
    return 0;

  case WM_DESTROY:
    nCore::UnregisterForMessages(window, sCoreMessages);
    SendMessage(GetLitestepWnd(), LM_UNREGISTERMESSAGE, WPARAM(window), LPARAM(sLsMessages));
    return 0;
  }

  return DefWindowProc(window, message, wParam, lParam);
}
