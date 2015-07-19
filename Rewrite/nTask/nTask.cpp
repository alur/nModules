#include "Messages.h"
#include "TaskbarManager.hpp"

#include "../nModuleBase/nModule.hpp"

#include "../nShared/LiteStep.h"

#include "../nCoreApi/Core.h"
#include "../nCoreApi/Messages.h"

#include "../Headers/lsapi.h"
#include "../Headers/Macros.h"

NModule gModule(L"nTask", MakeVersion(1, 0, 0, 0), MakeVersion(1, 0, 0, 0));

// Set to whether or not windows are activated by hovering over them. In this mode we should
// automatically move the mouse cursor to the center of the window when activated.
BOOL gActiveWindowTracking = FALSE;

static const UINT sLsMessages[] = { LM_GETREVID, LM_REFRESH, 0 };
static TaskbarManager *sTaskbarManager = nullptr;


static void APICALL CreateTaskbar(LPCWSTR name, LPARAM) {
  sTaskbarManager->Create(name);
}


static void LoadSettings() {
  nCore::EnumRCLineTokens(L"*nTaskbar", CreateTaskbar, 0);
}


LRESULT WINAPI MessageHandlerProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam) {
  switch (message) {
  case WM_CREATE:
    SystemParametersInfo(SPI_GETACTIVEWINDOWTRACKING, 0, &gActiveWindowTracking, 0);
    sTaskbarManager = new TaskbarManager(window);
    SendMessage(GetLitestepWnd(), LM_REGISTERMESSAGE, WPARAM(window), LPARAM(sLsMessages));
    LoadSettings();
    return 0;

  case WM_DESTROY:
    SAFEDELETE(sTaskbarManager);
    SendMessage(GetLitestepWnd(), LM_UNREGISTERMESSAGE, WPARAM(window), LPARAM(sLsMessages));
    return 0;

  case WM_SETTINGCHANGE:
    if (wParam == SPI_SETACTIVEWINDOWTRACKING) {
      SystemParametersInfo(SPI_GETACTIVEWINDOWTRACKING, 0, &gActiveWindowTracking, 0);
    }
    return 0;

  case LM_GETREVID:
    return HandleGetRevId(gModule.name, gModule.version, lParam);

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
  case WM_TIMER:
    return sTaskbarManager->HandleMessage(window, message, wParam, lParam);
  }

  return DefWindowProc(window, message, wParam, lParam);
}
