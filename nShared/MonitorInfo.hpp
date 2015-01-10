//-------------------------------------------------------------------------------------------------
// /nShared/MonitorInfo.hpp
// The nModules Project
//
// Provides information about the current monitor configuration.
//-------------------------------------------------------------------------------------------------
#pragma once

#include "../Utilities/Common.h"

#include <vector>

class MonitorInfo {
public:
  struct Monitor {
    RECT rect;
    RECT workArea;
    int width;
    int height;
    int workAreaWidth;
    int workAreaHeight;
  };

public:
  MonitorInfo();

public:
  void Update();
  UINT MonitorFromHWND(HWND hWnd) const;
  UINT MonitorFromRECT(RECT rect) const;

  UINT GetMonitorCount() const;
  const Monitor &GetMonitor(UINT id) const;
  const std::vector<Monitor> &GetMonitors() const;
  const Monitor &GetVirtualDesktop() const;

private:
  static BOOL CALLBACK EnumMonitorsCallback(HMONITOR, HDC, LPRECT, LPARAM);

private:
  // Every monitor
  std::vector<Monitor> mMonitors;

  // The virtual desktop
  Monitor mVirtualDesktop;
};
