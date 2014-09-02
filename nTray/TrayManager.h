//-------------------------------------------------------------------------------------------------
// /nTray/TrayManager.h
// The nModules Project
//
// Keeps track of the system tray icons and notifies the trays of changes.
//-------------------------------------------------------------------------------------------------
#pragma once

namespace TrayManager {
  void Stop();
  void ListIconIDS();
  void InitCompleted();

  LRESULT ShellMessage(HWND, UINT message, WPARAM, LPARAM);
}
