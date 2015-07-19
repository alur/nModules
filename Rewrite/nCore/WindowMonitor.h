#pragma once

#include "../Headers/Windows.h"

/// <summary>
/// Keeps track of the existing taskbar windows and their icons. This removes that responsibility
/// from nTask, nTaskSwitch, and nPopup; allowing quick hwnd -> hicon lookups.
/// </summary>
namespace WindowMonitor {
  void Start();
  void Stop();
  LRESULT HandleMessage(UINT, WPARAM, LPARAM);

  /// <summary>
  /// Removes non-existing HWNDs, and checks if a window has moved to another monitor.
  /// </summary>
  void RunWindowMaintenance();
};
