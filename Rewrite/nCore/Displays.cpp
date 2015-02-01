#include "Displays.hpp"

#include "../nShared/Math.h"

#include <ShellScalingAPI.h>


Displays::Displays() {
  Update();
}


UINT Displays::DisplayFromHWND(HWND hwnd) const {
  WINDOWINFO wndInfo;
  WINDOWPLACEMENT wp;
  RECT wndRect;

  // Work out the window RECT
  ZeroMemory(&wp, sizeof(WINDOWPLACEMENT));
  GetWindowPlacement(hwnd, &wp);
  if (wp.showCmd == SW_SHOWMINIMIZED) {
    wndRect = wp.rcNormalPosition;
  } else { // rcNormalPosition is only valid if the window is minimized.
    ZeroMemory(&wndInfo, sizeof(WINDOWINFO));
    wndInfo.cbSize = sizeof(wndInfo);
    GetWindowInfo(hwnd, &wndInfo);
    wndRect = wndInfo.rcWindow;
  }

  return DisplayFromRECT(wndRect);
}


UINT Displays::DisplayFromRECT(RECT rect) const {
  int maxArea = 0;
  int area = 0;
  UINT display = UINT(-1);

  // It happened...
  if (rect.right <= rect.left) {
    rect.right = rect.left + 1;
  }
  if (rect.bottom <= rect.top) {
    rect.bottom = rect.top + 1;
  }

  // Figure out which display contains the bigest part of the RECT.
  for (UINT i = 0; i < mDisplays.size(); ++i) {
    area = RectIntersectArea(&rect, &mDisplays[i].rect);
    if (area > maxArea) {
      maxArea = area;
      display = i;
    }
  }

  return display;
}


UINT Displays::Count() const {
  return (UINT)mDisplays.size();
}


const Display &Displays::GetDisplay(UINT id) const {
  return mDisplays[id];
}


const Display &Displays::GetDesktop() const {
  return mDesktop;
}


void Displays::Update() {
  mDisplays.clear();

  mDesktop.rect.left = GetSystemMetrics(SM_XVIRTUALSCREEN);
  mDesktop.rect.top = GetSystemMetrics(SM_YVIRTUALSCREEN);
  mDesktop.width = GetSystemMetrics(SM_CXVIRTUALSCREEN);
  mDesktop.height = GetSystemMetrics(SM_CYVIRTUALSCREEN);
  mDesktop.rect.right = mDesktop.width + mDesktop.rect.left;
  mDesktop.rect.bottom = mDesktop.height + mDesktop.rect.top;
  ZeroMemory(&mDesktop.workArea, sizeof(RECT));
  mDesktop.workAreaHeight = 0;
  mDesktop.workAreaWidth = 0;

  mDisplays.reserve(GetSystemMetrics(SM_CMONITORS));
  mDisplays.emplace_back();
  EnumDisplayMonitors(nullptr, nullptr, EnumMonitorsCallback, (LPARAM)this);
}


BOOL CALLBACK Displays::EnumMonitorsCallback(HMONITOR monitor, HDC, LPRECT, LPARAM lParam) {
  Displays *self = (Displays*)lParam;

  MONITORINFOEX mi;
  mi.cbSize = sizeof(MONITORINFOEX);
  GetMonitorInfo(monitor, &mi);

  bool isPrimary = (mi.dwFlags & MONITORINFOF_PRIMARY) == MONITORINFOF_PRIMARY;
  if (!isPrimary) {
    self->mDisplays.emplace_back();
  }
  Display &display = isPrimary ? self->mDisplays[0] : self->mDisplays.back();

  display.rect = mi.rcMonitor;
  display.height = mi.rcMonitor.bottom - mi.rcMonitor.top;
  display.width = mi.rcMonitor.right - mi.rcMonitor.left;

  display.workArea = mi.rcWork;
  display.workAreaHeight = mi.rcWork.bottom - mi.rcWork.top;
  display.workAreaWidth = mi.rcWork.right - mi.rcWork.left;

  display.monitor = monitor;
  UINT dpiY, dpiX;
  GetDpiForMonitor(monitor, MDT_EFFECTIVE_DPI, &dpiX, &dpiY);
  display.dpi = D2D1::Point2F((float)dpiY, (float)dpiX);

  return TRUE;
}
