#include "Messages.h"
#include "TaskbarManager.hpp"

#include "../nCoreApi/Core.h"

#include "../nUtilities/lsapi.h"

#include <thread>

static const UINT sMessages[] = { LM_GETMINRECT, LM_REDRAW, LM_WINDOWACTIVATED, LM_WINDOWCREATED,
  LM_WINDOWDESTROYED, LM_WINDOWREPLACED, LM_WINDOWREPLACING, LM_MONITORCHANGED,
  LM_TASK_SETPROGRESSSTATE, LM_TASK_SETPROGRESSVALUE, LM_TASK_MARKASACTIVE, LM_TASK_REGISTERTAB,
  LM_TASK_UNREGISTERTAB, LM_TASK_SETACTIVETAB, LM_TASK_SETTABORDER, LM_TASK_SETTABPROPERTIES,
  LM_TASK_SETOVERLAYICON, LM_TASK_SETOVERLAYICONDESC, LM_TASK_SETTHUMBNAILTOOLTIP,
  LM_TASK_SETTHUMBNAILCLIP, LM_TASK_THUMBBARADDBUTTONS, LM_TASK_THUMBBARUPDATEBUTTONS,
  LM_TASK_THUMBBARSETIMAGELIST, 0 };

// How often windows are allowed to be updated, in milliseconds.
#define MAX_UPDATE_FREQUENCY 100

TaskbarManager::TaskbarManager(HWND messageWindow)
  : mMessageWindow(messageWindow)
  , mInitialized(false)
{
  SendMessage(GetLitestepWnd(), LM_REGISTERMESSAGE, (WPARAM)mMessageWindow, (LPARAM)sMessages);
  mInitThread = std::thread([messageWindow] () -> void {
    EnumDesktopWindows(nullptr, [] (HWND window, LPARAM messageWindow) -> BOOL {
      if (nCore::IsTaskbarWindow(window)) {
        PostMessage((HWND)messageWindow, LM_WINDOWCREATED, (WPARAM)window, 0);
      }
      return TRUE;
    }, (LPARAM)messageWindow);
    PostMessage(messageWindow, NTASK_INITIALIZED, 0, 0);
  });
}


TaskbarManager::~TaskbarManager() {
  SendMessage(GetLitestepWnd(), LM_UNREGISTERMESSAGE, (WPARAM)mMessageWindow, (LPARAM)sMessages);
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
      taskbar.AddTask(task.first, false);
    }
    if (mInitialized) {
      taskbar.Initialized();
    }
  } else {
    // TODO::Log
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
  task.lastUpdateTime = GetTickCount64();
  task.updateDuringMaintenance = false;

  for (auto &taskbar : mTaskbars) {
    taskbar.second.AddTask(window, isReplacement);
  }
}


void TaskbarManager::DestroyWindow(HWND window, bool isBeingReplaced) {
  if (mTasks.find(window) == mTasks.end()) {
    return;
  }

  for (auto &taskbar : mTaskbars) {
    taskbar.second.RemoveTask(window, isBeingReplaced);
  }
  mTasks.erase(window);
}


void TaskbarManager::RedrawWindow(HWND window, LPARAM lParam) {
  if (mTasks.find(window) == mTasks.end()) {
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
    taskbar.second.RedrawTask(window);
  }
}


LRESULT TaskbarManager::HandleMessage(HWND window, UINT message, WPARAM wParam, LPARAM lParam) {
  switch (message) {
  case LM_GETMINRECT:
    // TODO(Erik): Handle this
    return 0;

  case LM_REDRAW:
    RedrawWindow((HWND)wParam, lParam);
    return 0;

  case LM_WINDOWACTIVATED:
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

  case NTASK_INITIALIZED:
    mInitialized = true;
    for (auto &taskbar : mTaskbars) {
      taskbar.second.Initialized();
    }
    return 0;
  }

  return DefWindowProc(window, message, wParam, lParam);
}
