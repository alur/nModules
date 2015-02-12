#pragma once

#include "IconData.h"
#include "Tray.hpp"
#include "TrayIcon.hpp"

#include "../nUtilities/lsapi.h"

#include <list>
#include <map>

class TrayManager {
private:
  struct NotifyIcon {
    IconData data;
    std::list<std::pair<Tray*, TrayIcon*>> instances;
  };

public:
  TrayManager();
  ~TrayManager();

public:
  void CreateTray(LPCWSTR name);
  void DestroyAllTrays();
  LRESULT HandleMessage(HWND, UINT, WPARAM, LPARAM);

private:
  void AddIcon(LPLSNOTIFYICONDATA iconData);
  void DeleteIcon(LPLSNOTIFYICONDATA iconData);
  void ModifyIcon(LPLSNOTIFYICONDATA iconData);
  void SetFocus(LPLSNOTIFYICONDATA iconData);
  void SetVersion(LPLSNOTIFYICONDATA iconData);

  std::list<NotifyIcon>::iterator FindIcon(GUID);
  std::list<NotifyIcon>::iterator FindIcon(HWND, UINT);
  std::list<NotifyIcon>::iterator FindIcon(LPLSNOTIFYICONDATA);
  std::list<NotifyIcon>::iterator FindIcon(LPSYSTRAYINFOEVENT);

private:
  bool mInitialized;
  std::list<NotifyIcon> mIcons;
  std::map<std::wstring, Tray> mTrays;
};
