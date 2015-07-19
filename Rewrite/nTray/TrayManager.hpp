#pragma once

#include "IconData.h"
#include "Tray.hpp"
#include "TrayIcon.hpp"

#include "../Headers/lsapi.h"

#include <list>
#include <map>

class TrayManager {
private:
  struct NotifyIcon {
    IconData data;
    std::list<std::pair<Tray*, std::list<TrayIcon>::iterator>> instances;
  };

public:
  TrayManager();
  ~TrayManager();

public:
  void CreateTray(LPCWSTR name);
  void DestroyAllTrays();
  LRESULT HandleMessage(HWND, UINT, WPARAM, LPARAM);

private:
  void UpdateIconData(IconData&, LPLSNOTIFYICONDATA);

  void AddIcon(LPLSNOTIFYICONDATA nid);
  void DeleteIcon(LPLSNOTIFYICONDATA nid);
  void ModifyIcon(LPLSNOTIFYICONDATA nid);
  void SetFocus(LPLSNOTIFYICONDATA nid);
  void SetVersion(LPLSNOTIFYICONDATA nid);

  std::list<NotifyIcon>::iterator FindIcon(GUID);
  std::list<NotifyIcon>::iterator FindIcon(HWND, UINT);
  std::list<NotifyIcon>::iterator FindIcon(LPLSNOTIFYICONDATA);
  std::list<NotifyIcon>::iterator FindIcon(LPSYSTRAYINFOEVENT);

private:
  bool mInitialized;
  std::list<NotifyIcon> mIcons;
  std::map<std::wstring, Tray> mTrays;
};
