//-------------------------------------------------------------------------------------------------
// /nShared/MonitorInfo.cpp
// The nModules Project
//
// Provides information about the current monitor configuration.
//-------------------------------------------------------------------------------------------------
#include "MonitorInfo.hpp"

#include "../nShared/LiteStep.h"

#include "../Utilities/Math.h"

#include <assert.h>


/// <summary>
/// Creates a new instance of the MonitorInfo class.
/// </summary>
MonitorInfo::MonitorInfo() {
  Update();
}


/// <summary>
/// Returns the monitor which contains the biggest area of the specified window.
/// </summary>
UINT MonitorInfo::MonitorFromHWND(HWND hWnd) const {
  WINDOWINFO wndInfo;
  WINDOWPLACEMENT wp;
  RECT wndRect;

  // Work out the window RECT
  ZeroMemory(&wp, sizeof(WINDOWPLACEMENT));
  GetWindowPlacement(hWnd, &wp);
  if (wp.showCmd == SW_SHOWMINIMIZED) {
    wndRect = wp.rcNormalPosition;
  } else { // rcNormalPosition is only valid if the window is minimized.
    ZeroMemory(&wndInfo, sizeof(WINDOWINFO));
    wndInfo.cbSize = sizeof(wndInfo);
    GetWindowInfo(hWnd, &wndInfo);
    wndRect = wndInfo.rcWindow;
  }

  return MonitorFromRECT(wndRect);
}


/// <summary>
/// Returns the monitor which contains the biggest area of the specified window.
/// </summary>
UINT MonitorInfo::MonitorFromRECT(RECT rect) const {
  int maxArea = 0;
  int area = 0;
  UINT monitor = UINT(-1);

  // It happened...
  if (rect.right <= rect.left) {
    rect.right = rect.left + 1;
  }
  if (rect.bottom <= rect.top) {
    rect.bottom = rect.top + 1;
  }

  // Figure out which monitor contains the bigest part of the RECT.
  for (UINT i = 0; i < mMonitors.size(); ++i) {
    area = RectIntersectArea(&rect, &mMonitors[i].rect);
    if (area > maxArea) {
      maxArea = area;
      monitor = i;
    }
  }

  return monitor;
}


/// <summary>
/// Updates the list of monitors. Should be called when ...
/// </summary>
void MonitorInfo::Update() {
  mMonitors.clear();

  mVirtualDesktop.rect.left = GetSystemMetrics(SM_XVIRTUALSCREEN);
  mVirtualDesktop.rect.top = GetSystemMetrics(SM_YVIRTUALSCREEN);
  mVirtualDesktop.width = GetSystemMetrics(SM_CXVIRTUALSCREEN);
  mVirtualDesktop.height = GetSystemMetrics(SM_CYVIRTUALSCREEN);
  mVirtualDesktop.rect.right = mVirtualDesktop.width + mVirtualDesktop.rect.left;
  mVirtualDesktop.rect.bottom = mVirtualDesktop.height + mVirtualDesktop.rect.top;
  ZeroMemory(&mVirtualDesktop.workArea, sizeof(RECT));
  mVirtualDesktop.workAreaHeight = 0;
  mVirtualDesktop.workAreaWidth = 0;

  mMonitors.reserve(GetSystemMetrics(SM_CMONITORS));
  mMonitors.emplace_back();
  EnumDisplayMonitors(nullptr, nullptr, EnumMonitorsCallback, (LPARAM)this);
}


/// <summary>
/// Gets the monitor with the given id.
/// </summary>
/// <param name="id">The id of the monitor to get.</param>
const MonitorInfo::Monitor &MonitorInfo::GetMonitor(UINT id) const {
  assert(id < mMonitors.size());
  return mMonitors[id];
}


/// <summary>
/// Gets the virtual desktop.
/// </summary>
const MonitorInfo::Monitor &MonitorInfo::GetVirtualDesktop() const {
  return mVirtualDesktop;
}


/// <summary>
/// Gets the number of monitors.
/// </summary>
UINT MonitorInfo::GetMonitorCount() const {
  return UINT(mMonitors.size());
}


/// <summary>
/// Returns a vector of monitors.
/// </summary>
const std::vector<MonitorInfo::Monitor> &MonitorInfo::GetMonitors() const {
  return mMonitors;
}


/// <summary>
/// Callback for EnumDisplayMonitors. Adds a monitor to the list of monitors.
/// </summary>
/// <param name="hMonitor">Handle to the monitor to add.</param>
/// <param name="lParam">A pointer to the MonitorInfo class to add this monitor to.</param>
BOOL CALLBACK MonitorInfo::EnumMonitorsCallback(HMONITOR hMonitor, HDC, LPRECT, LPARAM lParam) {
  MonitorInfo *self = (MonitorInfo*)lParam;

  MONITORINFO mi;
  mi.cbSize = sizeof(MONITORINFO);
  GetMonitorInfo(hMonitor, &mi);

  bool isPrimary = (mi.dwFlags & MONITORINFOF_PRIMARY) == MONITORINFOF_PRIMARY;
  if (!isPrimary) {
    self->mMonitors.emplace_back();
  }
  MonitorInfo::Monitor &monitor = isPrimary ? self->mMonitors[0] : self->mMonitors.back();

  monitor.rect = mi.rcMonitor;
  monitor.height = mi.rcMonitor.bottom - mi.rcMonitor.top;
  monitor.width = mi.rcMonitor.right - mi.rcMonitor.left;

  monitor.workArea = mi.rcWork;
  monitor.workAreaHeight = mi.rcWork.bottom - mi.rcWork.top;
  monitor.workAreaWidth = mi.rcWork.right - mi.rcWork.left;

  return TRUE;
}
