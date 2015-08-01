#include "Api.h"
#include "Displays.hpp"
#include "Messages.h"
#include "WindowMonitor.h"

#include "../Headers/lsapi.h"
#include "../Headers/Macros.h"

#include <algorithm>
#include <assert.h>
#include <thread>
#include <unordered_map>

extern HWND gWindow;

// How often icons are allowed to be updated, in milliseconds.
#define MAX_UPDATE_FREQUENCY 100

// The messages that the Window Manager wants from the LS core
static const UINT sWMMessages[] = {
  LM_WINDOWCREATED, LM_WINDOWDESTROYED, LM_WINDOWREPLACED, LM_WINDOWREPLACING, LM_REDRAW, 0
};

static HICON sDefaultIcon;
static std::thread sInitThread;


struct WindowData {
  WindowData()
    : largeIcon(sDefaultIcon)
    , smallIcon(sDefaultIcon)
    , lastUpdateTime(0)
    , updateDuringMaintenance(false)
  {}
  HICON largeIcon;
  HICON smallIcon;
  ULONGLONG lastUpdateTime;
  bool updateDuringMaintenance;
};

typedef std::unordered_map<HWND, WindowData> WindowMap;

static WindowMap sWindowData;


static void CALLBACK GetIconCallback(HWND window, UINT message, ULONG_PTR data, LRESULT result) {
  assert(message == WM_GETICON);

  switch (data) {
  case ICON_BIG:
    if (result != NULL) {
      sWindowData[window].largeIcon = (HICON)result;
    } else {
      HICON icon = (HICON)GetClassLongPtr(window, GCLP_HICON);
      if (!icon) {
        icon = sDefaultIcon;
      }
      sWindowData[window].largeIcon = icon;
    }
    SendMessageCallback(window, WM_GETICON, ICON_SMALL, NULL, GetIconCallback, ICON_SMALL);
    break;

  case ICON_SMALL:
    if (result != NULL) {
      sWindowData[window].smallIcon = (HICON)result;
      SendMessage(gWindow, NCORE_WINDOW_ICON_CHANGED, (WPARAM)window, NULL);
    } else {
      SendMessageCallback(window, WM_GETICON, ICON_SMALL2, NULL, GetIconCallback, ICON_SMALL2);
    }
    break;

  case ICON_SMALL2:
    if (result != NULL) {
      sWindowData[window].smallIcon = (HICON)result;
    } else {
      HICON icon = (HICON)GetClassLongPtr(window, GCLP_HICONSM);
      if (!icon) {
        icon = sDefaultIcon;
      }
      sWindowData[window].smallIcon = icon;
    }
    SendMessage(gWindow, NCORE_WINDOW_ICON_CHANGED, (WPARAM)window, NULL);
    break;
  }
}


static void UpdateWindowData(HWND window) {
  ULONGLONG time = GetTickCount64();
  WindowData &data = sWindowData[window];
  if (time - data.lastUpdateTime < MAX_UPDATE_FREQUENCY) {
    data.updateDuringMaintenance = true;
    return;
  }
  data.lastUpdateTime = time;
  data.updateDuringMaintenance = false;
  SendMessageCallback(window, WM_GETICON, ICON_BIG, NULL, GetIconCallback, ICON_BIG);
}


LRESULT WindowMonitor::HandleMessage(UINT message, WPARAM wParam, LPARAM) {
  switch (message) {
  case LM_REDRAW:
    UpdateWindowData((HWND)wParam);
    return 0;

  case LM_WINDOWCREATED:
    UpdateWindowData((HWND)wParam);
    return 0;

  case LM_WINDOWDESTROYED:
    sWindowData.erase((HWND)wParam);
    return 0;

  case LM_WINDOWREPLACED:
    UpdateWindowData((HWND)wParam);
    return 0;

  case LM_WINDOWREPLACING:
    sWindowData.erase((HWND)wParam);
    return 0;

  default:
    assert(false); // We should handle every message sent to us.
    return 0;
  }
}


void WindowMonitor::Start() {
  sDefaultIcon = LoadIcon(nullptr, IDI_APPLICATION);
  SendMessage(GetLitestepWnd(), LM_REGISTERMESSAGE, (WPARAM)gWindow, (LPARAM)sWMMessages);
  SetTimer(gWindow, NCORE_TIMER_WINDOW_MAINTENANCE, 250, nullptr);

  sInitThread = std::thread([] () -> void {
    EnumDesktopWindows(nullptr, (WNDENUMPROC) [] (HWND window, LPARAM) -> BOOL {
      if (IsTaskbarWindow(window)) {
        PostMessage(gWindow, LM_WINDOWCREATED, (WPARAM)window, 0);
      }
      return TRUE;
    }, 0);
  });
}


void WindowMonitor::Stop() {
  sInitThread.join();
  SendMessage(GetLitestepWnd(), LM_UNREGISTERMESSAGE, (WPARAM)gWindow, (LPARAM)sWMMessages);
  KillTimer(gWindow, NCORE_TIMER_WINDOW_MAINTENANCE);
  sWindowData.clear();
  DestroyIcon(sDefaultIcon);
  sDefaultIcon = nullptr;
}


void WindowMonitor::RunWindowMaintenance() {
  for (WindowMap::iterator iter = sWindowData.begin(); iter != sWindowData.end();) {
    if (!IsTaskbarWindow(iter->first)) {
      iter = sWindowData.erase(iter);
      continue;
    }

    if (iter->second.updateDuringMaintenance) {
      iter->second.updateDuringMaintenance = false;
      UpdateWindowData(iter->first);
    }

    ++iter;
  }
}


static inline bool IsCoreWindow(HWND window) {
  wchar_t className[512];
  if (GetClassName(window, className, _countof(className)) == 0) {
    return false;
  }
  return wcscmp(L"Windows.UI.Core.CoreWindow", className) == 0 ||
      wcscmp(L"ApplicationFrameWindow", className) == 0;
}


EXPORT_CDECL(bool) IsTaskbarWindow(HWND window) {
  if (!IsWindow(window) || !IsWindowVisible(window)) {
    return false;
  }
  LONG_PTR exStyle = GetWindowLongPtr(window, GWL_EXSTYLE);
  return CHECKFLAG(exStyle, WS_EX_APPWINDOW) ||
      GetParent(window) == nullptr &&
      GetWindow(window, GW_OWNER) == nullptr &&
      !CHECKFLAG(exStyle, WS_EX_TOOLWINDOW) &&
      GetWindowTextLength(window) != 0 &&
      !IsCoreWindow(window); // TODO(Erik): How does explorer do this?
}


EXPORT_CDECL(HICON) GetWindowIcon(HWND window, UINT32 size) {
  WindowMap::iterator iter = sWindowData.find(window);
  if (iter == sWindowData.end()) {
    if (IsTaskbarWindow(window)) {
      UpdateWindowData(window);
    }
    return nullptr;
  }
  if (size > 20 && iter->second.largeIcon != nullptr) {
    return iter->second.largeIcon;
  }
  return iter->second.smallIcon;
}
