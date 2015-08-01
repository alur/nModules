#include "Messages.h"
#include "TaskbarManager.hpp"

#include "../nModuleBase/nModule.hpp"

#include "../nCoreApi/Core.h"
#include "../nCoreApi/Messages.h"

#include "../Headers/lsapi.h"

#include <thread>
#include <VersionHelpers.h>

static const UINT sLSMessages[] = { LM_GETMINRECT, LM_REDRAW, LM_WINDOWACTIVATED, LM_WINDOWCREATED,
  LM_WINDOWDESTROYED, LM_WINDOWREPLACED, LM_WINDOWREPLACING, LM_MONITORCHANGED,
  LM_TASK_SETPROGRESSSTATE, LM_TASK_SETPROGRESSVALUE, LM_TASK_MARKASACTIVE, LM_TASK_REGISTERTAB,
  LM_TASK_UNREGISTERTAB, LM_TASK_SETACTIVETAB, LM_TASK_SETTABORDER, LM_TASK_SETTABPROPERTIES,
  LM_TASK_SETOVERLAYICON, LM_TASK_SETOVERLAYICONDESC, LM_TASK_SETTHUMBNAILTOOLTIP,
  LM_TASK_SETTHUMBNAILCLIP, LM_TASK_THUMBBARADDBUTTONS, LM_TASK_THUMBBARUPDATEBUTTONS,
  LM_TASK_THUMBBARSETIMAGELIST, 0 };

static const UINT sCoreMessages[] = { NCORE_WINDOW_ICON_CHANGED, NCORE_DISPLAYS_CHANGED, 0 };

extern NModule gModule;

HWND gActiveWindow;
HWND gPreviouslyActiveWindow;

// How often windows are allowed to be updated, in milliseconds.
#define MAX_UPDATE_FREQUENCY 100

// How often to perform window maintenance.
#define MAINTENANCE_FREQUENCY 250


TaskbarManager::TaskbarManager(HWND messageWindow)
  : mMessageWindow(messageWindow)
  , mInitialized(false)
{
  mUpdateMonitorDuringMaintanance = !IsWindows8OrGreater();
  gActiveWindow = GetActiveWindow();
  gPreviouslyActiveWindow = gActiveWindow;
  SendMessage(GetLitestepWnd(), LM_REGISTERMESSAGE, (WPARAM)mMessageWindow, (LPARAM)sLSMessages);
  nCore::RegisterForMessages(mMessageWindow, sCoreMessages);
  mInitThread = std::thread([] (HWND messageWindow) -> void {
    EnumDesktopWindows(nullptr, (WNDENUMPROC) [] (HWND window, LPARAM messageWindow) -> BOOL {
      if (nCore::IsTaskbarWindow(window)) {
        PostMessage((HWND)messageWindow, LM_WINDOWCREATED, (WPARAM)window, 0);
      }
      return TRUE;
    }, (LPARAM)messageWindow);
    PostMessage(messageWindow, NTASK_INITIALIZED, 0, 0);
  }, messageWindow);
  SetTimer(messageWindow, NTASK_TIMER_MAINTENANCE, MAINTENANCE_FREQUENCY, nullptr);
}


TaskbarManager::~TaskbarManager() {
  KillTimer(mMessageWindow, NTASK_TIMER_MAINTENANCE);
  nCore::UnregisterForMessages(mMessageWindow, sCoreMessages);
  SendMessage(GetLitestepWnd(), LM_UNREGISTERMESSAGE, (WPARAM)mMessageWindow, (LPARAM)sLSMessages);
  mInitThread.join();
}


void TaskbarManager::Create(LPCWSTR name) {
  auto result = mTaskbars.emplace(
    std::piecewise_construct,
    std::forward_as_tuple(std::wstring(name)),
    std::forward_as_tuple(name));
  if (result.second) {
    Taskbar &taskbar = result.first->second;
    for (auto task : mTasks) {
      taskbar.AddTask(task.first, task.second.data, false);
    }
    if (mInitialized) {
      taskbar.Initialized();
    }
  } else {
    gModule.log->Warning(L"Failed to create taskbar %s, it already exists!");
  }
}


void TaskbarManager::DestroyAll() {
  mTaskbars.clear();
}


void TaskbarManager::AddWindow(HWND window, bool isReplacement) {
  if (mTasks.find(window) != mTasks.end()) {
    return;
  }

  Task &task = mTasks[window];
  task.data.monitor = nCore::GetDisplays()->DisplayFromHWND(window);
  task.data.progress = 0;
  task.data.progressState = TBPF_NOPROGRESS;
  task.data.flashing = false;
  task.data.minimized = IsIconic(window) != FALSE;
  task.lastUpdateTime = GetTickCount64();
  task.updateDuringMaintenance = false;

  for (auto &taskbar : mTaskbars) {
    taskbar.second.AddTask(window, task.data, isReplacement);
  }
}


void TaskbarManager::DestroyWindow(HWND window, bool isBeingReplaced) {
  if (mTasks.find(window) == mTasks.end()) {
    return;
  }
  mTasks.erase(window);

  for (auto &taskbar : mTaskbars) {
    taskbar.second.RemoveTask(window, isBeingReplaced);
  }
}


LRESULT TaskbarManager::GetMinRect(HWND window, LPPOINTS points) {
  D2D1_RECT_F rect = D2D1::RectF(0, 0, 20, 20);
  for (auto &taskbar : mTaskbars) {
    if (taskbar.second.GetButtonScreenRect(window, &rect)) {
      break;
    }
  }
  points[0].x = (short)rect.left;
  points[0].y = (short)rect.top;
  points[1].x = (short)rect.right;
  points[1].y = (short)rect.bottom;
  return 1;
}


void TaskbarManager::MonitorChanged(HWND window, UINT newMonitor) {
  if (mTasks.find(window) == mTasks.end()) {
    if (nCore::IsTaskbarWindow(window)) {
      AddWindow(window, false);
    }
    return;
  }

  Task &task = mTasks[window];
  task.data.monitor = newMonitor;
  for (auto &taskbar : mTaskbars) {
    taskbar.second.MonitorChanged(window, task.data);
  }
}


void TaskbarManager::RedrawWindow(HWND window, LPARAM lParam) {
  if (mTasks.find(window) == mTasks.end()) {
    if (nCore::IsTaskbarWindow(window)) {
      AddWindow(window, false);
    }
    return;
  }

  ULONGLONG time = GetTickCount64();
  Task &task = mTasks[window];

  if (lParam == HSHELL_HIGHBIT) {
    task.data.flashing = true;
  }

  // There are a few windows which update 100s of times/second (such as the cygwin installer). We
  // can't render that fast, and end up locking up the shell with our backlog of HSHELL_REDRAW
  // messages. Therefore, if we get called too frequently, we just defer the update until the next
  // maintenance cycle.
  if (time - task.lastUpdateTime < MAX_UPDATE_FREQUENCY) {
    task.updateDuringMaintenance = true;
    return;
  }
  task.lastUpdateTime = time;
  task.updateDuringMaintenance = false;

  for (auto &taskbar : mTaskbars) {
    taskbar.second.RedrawTask(window, TaskButton::Text, lParam == HSHELL_HIGHBIT);
  }
}


void TaskbarManager::RedrawWindowIcon(HWND window) {
  for (auto &taskbar : mTaskbars) {
    taskbar.second.RedrawTask(window, TaskButton::Icon, false);
  }
}


void TaskbarManager::SetOverlayIcon(HWND window, HICON icon) {
  if (mTasks.find(window) == mTasks.end()) {
    return;
  }
  mTasks[window].data.overlayIcon = icon;

  for (auto &taskbar : mTaskbars) {
    taskbar.second.RedrawTask(window, TaskButton::OverlayIcon, false);
  }
}


void TaskbarManager::WindowMaintenance() {
  for (auto &taskbar : mTaskbars) {
    taskbar.second.Lock();
  }

  // Remove invalid windows
  for (auto iter = mTasks.begin(); iter != mTasks.end(); ) {
    if (!IsWindow(iter->first)) {
      for (auto &taskbar : mTaskbars) {
        taskbar.second.RemoveTask(iter->first, false);
      }
      iter = mTasks.erase(iter);
      continue;
    }

    if (mUpdateMonitorDuringMaintanance) {
      UINT monitor = nCore::GetDisplays()->DisplayFromHWND(iter->first);
      if (monitor != iter->second.data.monitor) {
        MonitorChanged(iter->first, monitor);
      }
    }

    if (iter->second.updateDuringMaintenance) {
      RedrawWindow(iter->first, 0);
    }

    if ((IsIconic(iter->first) != FALSE) != iter->second.data.minimized) {
      iter->second.data.minimized = !iter->second.data.minimized;
      for (auto &taskbar : mTaskbars) {
        taskbar.second.UpdateButtonState(iter->first);
      }
    }
    ++iter;
  }

  for (auto &taskbar : mTaskbars) {
    taskbar.second.Unlock();
  }
}


LRESULT TaskbarManager::HandleMessage(HWND window, UINT message, WPARAM wParam, LPARAM lParam) {
  switch (message) {
  case LM_GETMINRECT:
    return GetMinRect((HWND)wParam, (LPPOINTS)lParam);

  case LM_MONITORCHANGED:
    MonitorChanged((HWND)wParam, nCore::GetDisplays()->DisplayFromHWND((HWND)wParam));
    return 0;

  case LM_TASK_SETOVERLAYICON:
    SetOverlayIcon((HWND)wParam, (HICON)lParam);
    return 0;

  case LM_REDRAW:
    RedrawWindow((HWND)wParam, lParam);
    return 0;

  case LM_WINDOWACTIVATED:
    gPreviouslyActiveWindow = gActiveWindow;
    gActiveWindow = (HWND)wParam;
    for (auto &taskbar : mTaskbars) {
      taskbar.second.ActiveWindowChanged(gPreviouslyActiveWindow, gActiveWindow);
    }
    if (nCore::IsTaskbarWindow((HWND)wParam)) {
      AddWindow((HWND)wParam, false); // Steam...
    }
    return 0;

  case LM_WINDOWCREATED:
    AddWindow((HWND)wParam, false);
    return 0;

  case LM_WINDOWDESTROYED:
    DestroyWindow((HWND)wParam, false);
    return 0;

  case LM_WINDOWREPLACED:
    AddWindow((HWND)wParam, true);
    return 0;

  case LM_WINDOWREPLACING:
    DestroyWindow((HWND)wParam, true);
    return 0;

  case NCORE_DISPLAYS_CHANGED:
    return 0;

  case NCORE_WINDOW_ICON_CHANGED:
    RedrawWindowIcon((HWND)wParam);
    return 0;

  case NTASK_INITIALIZED:
    mInitialized = true;
    for (auto &taskbar : mTaskbars) {
      taskbar.second.Initialized();
    }
    return 0;

  case WM_TIMER:
    switch (wParam) {
    case NTASK_TIMER_MAINTENANCE:
      WindowMaintenance();
      break;
    }
    return 0;
  }

  return DefWindowProc(window, message, wParam, lParam);
}
