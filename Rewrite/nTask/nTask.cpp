#include "Messages.h"
#include "TaskbarManager.hpp"

#include "../nCoreApi/Core.h"
#include "../nCoreApi/Messages.h"

#include "../nShared/LiteStep.h"

#include "../nUtilities/lsapi.h"
#include "../nUtilities/Macros.h"
#include "../nUtilities/Windows.h"

#include <map>
#include <string>
#include <unordered_map>

static const UINT sLsMessages[] = { LM_GETREVID, LM_REFRESH, 0 };
static const wchar_t sName[] = L"nTask";
static const VERSION sVersion = MakeVersion(0, 9, 0, 0);
static const VERSION sCoreVersion = MakeVersion(0, 9, 0, 0);

static TaskbarManager *sTaskbarManager = nullptr;
static HWND sWindow = nullptr;


BOOL APIENTRY DllMain(HANDLE module, DWORD reasonForCall, LPVOID /* reserved */) {
  if (reasonForCall == DLL_PROCESS_ATTACH) {
#ifdef _DLL
    DisableThreadLibraryCalls((HINSTANCE)module);
#endif
  }
  return TRUE;
}


static void APICALL CreateTaskbar(LPCWSTR name, LPARAM) {
  sTaskbarManager->Create(name);
}


static void LoadSettings() {
  nCore::EnumRCLineTokens(L"*nTaskbar", CreateTaskbar, 0);
}


static LRESULT WINAPI MessageHandler(HWND window, UINT message, WPARAM wParam, LPARAM lParam) {
  switch (message) {
  case WM_CREATE:
    SendMessage(GetLitestepWnd(), LM_REGISTERMESSAGE, WPARAM(window), LPARAM(sLsMessages));
    sTaskbarManager = new TaskbarManager(window);
    return 0;

  case WM_DESTROY:
    SendMessage(GetLitestepWnd(), LM_UNREGISTERMESSAGE, WPARAM(window), LPARAM(sLsMessages));
    delete sTaskbarManager;
    sTaskbarManager = nullptr;
    return 0;

  case LM_GETREVID:
    return HandleGetRevId(sName, sVersion, lParam);

  case LM_REFRESH:
    sTaskbarManager->DestroyAll();
    LoadSettings();
    return 0;

  case LM_GETMINRECT:
  case LM_REDRAW:
  case LM_WINDOWACTIVATED:
  case LM_WINDOWCREATED:
  case LM_WINDOWDESTROYED:
  case LM_WINDOWREPLACED:
  case LM_WINDOWREPLACING:
  case LM_MONITORCHANGED:
  case LM_TASK_SETPROGRESSSTATE:
  case LM_TASK_SETPROGRESSVALUE:
  case LM_TASK_MARKASACTIVE:
  case LM_TASK_REGISTERTAB:
  case LM_TASK_UNREGISTERTAB:
  case LM_TASK_SETACTIVETAB:
  case LM_TASK_SETTABORDER:
  case LM_TASK_SETTABPROPERTIES:
  case LM_TASK_SETOVERLAYICON:
  case LM_TASK_SETOVERLAYICONDESC:
  case LM_TASK_SETTHUMBNAILTOOLTIP:
  case LM_TASK_SETTHUMBNAILCLIP:
  case LM_TASK_THUMBBARADDBUTTONS:
  case LM_TASK_THUMBBARUPDATEBUTTONS:
  case LM_TASK_THUMBBARSETIMAGELIST:
  case NCORE_DISPLAYS_CHANGED:
  case NCORE_WINDOW_ICON_CHANGED:
  case NTASK_INITIALIZED:
    return sTaskbarManager->HandleMessage(window, message, wParam, lParam);
  }

  return DefWindowProc(window, message, wParam, lParam);
}


EXPORT_CDECL(int) initModuleW(HWND /* parent */, HINSTANCE /* instance */, LPCWSTR /* path */) {
  if (FAILED(nCore::Connect(sCoreVersion))) {
    return 1;
  }

  if (FAILED(CreateMessageHandler(nCore::GetInstance(), sName, MessageHandler, sWindow))) {
    return 1;
  }

  LoadSettings();

  return 0;
}


EXPORT_CDECL(void) quitModule(HINSTANCE /* instance */) {
  nCore::Disconnect();
  DestroyWindow(sWindow);
  sWindow = nullptr;
}
