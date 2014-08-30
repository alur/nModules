//-------------------------------------------------------------------------------------------------
// /nTray/TrayManager.h
// The nModules Project
//
// Keeps track of the system tray icons and notifies the trays of changes.
//-------------------------------------------------------------------------------------------------
#pragma once

#include "TrayIcon.hpp"
#include "Types.h"

#include <map>
#include <vector>

namespace TrayManager {
  struct Icon {
    std::map<Tray*, TrayIcon*> trayIcons;
    IconData data;
  };

  typedef std::vector<Icon>::iterator IconIter;

  void Stop();
  void GetScreenRect(IconIter, LPRECT);

  IconIter FindIcon(GUID);
  IconIter FindIcon(HWND, UINT id);
  IconIter FindIcon(LiteStep::LPLSNOTIFYICONDATA);
  IconIter FindIcon(LiteStep::LPSYSTRAYINFOEVENT);

  void AddIcon(LiteStep::LPLSNOTIFYICONDATA);
  void DeleteIcon(LiteStep::LPLSNOTIFYICONDATA);
  void ModifyIcon(LiteStep::LPLSNOTIFYICONDATA);
  void SetFocus(LiteStep::LPLSNOTIFYICONDATA);
  void SetVersion(LiteStep::LPLSNOTIFYICONDATA);

  void ListIconIDS();
  void InitCompleted();

  LRESULT ShellMessage(HWND, UINT message, WPARAM, LPARAM);
}
