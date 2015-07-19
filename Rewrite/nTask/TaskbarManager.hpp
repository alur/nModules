#pragma once

#include "Taskbar.hpp"
#include "TaskData.h"

#include <ShlObj.h>
#include <string>
#include <thread>
#include <unordered_map>

/// <summary>
/// Manages the taskbars.
/// </summary>
class TaskbarManager {
public:
  explicit TaskbarManager(HWND messageWindow);
  ~TaskbarManager();

public:
  /// <summary>
  /// Creates a taskbar with the given name.
  /// </summary>
  void Create(LPCWSTR name);

  /// <summary>
  /// Destroys all the taskbars.
  /// </summary>
  void DestroyAll();

  LRESULT HandleMessage(HWND, UINT message, WPARAM, LPARAM);

private:
  void AddWindow(HWND window, bool isReplacement);
  void DestroyWindow(HWND window, bool isBeingReplaced);
  LRESULT GetMinRect(HWND window, LPPOINTS points);
  void MonitorChanged(HWND window, UINT newMonitor);
  void RedrawWindow(HWND window, LPARAM lParam);
  void RedrawWindowIcon(HWND window);
  void SetOverlayIcon(HWND window, HICON icon);
  void WindowMaintenance();

private:
  struct Task {
    TaskData data;
    ULONGLONG lastUpdateTime;
    bool updateDuringMaintenance;
  };

private:
  const HWND mMessageWindow;
  std::thread mInitThread;
  std::unordered_map<HWND, Task> mTasks;
  std::unordered_map<std::wstring, Taskbar> mTaskbars;
  bool mInitialized;

  // True if we need to update the monitor during task maintenance.
  bool mUpdateMonitorDuringMaintanance;
};
