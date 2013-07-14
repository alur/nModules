/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  MonitorInfo.cpp
 *  The nModules Project
 *
 *  Provides information about the current monitor configuration.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "../nShared/LiteStep.h"
#include "MonitorInfo.hpp"
#include "Math.h"

// Callback for EnumDisplayMonitors
BOOL CALLBACK EnumMonitorsCallback(HMONITOR, HDC, LPRECT, LPARAM);


/// <summary>
/// Creates a new instance of the MonitorInfo class.
/// </summary>
MonitorInfo::MonitorInfo() {
    Update();
}


/// <summary>
/// Destroys this instance of the MonitorInfo class.
/// </summary>
MonitorInfo::~MonitorInfo() {
    this->m_monitors.clear();
}


/// <summary>
/// Returns the monitor which contains the biggest area of the specified window.
/// </summary>
UINT MonitorInfo::MonitorFromHWND(HWND hWnd) {
    WINDOWINFO wndInfo;
    WINDOWPLACEMENT wp;
    RECT wndRect;

    // Work out the window RECT
    ZeroMemory(&wp, sizeof(WINDOWPLACEMENT));
    GetWindowPlacement(hWnd, &wp);
    if (wp.showCmd == SW_SHOWMINIMIZED) {
        wndRect = wp.rcNormalPosition;
    }
    else { // rcNormalPosition is only valid if the window is minimized.
        ZeroMemory(&wndInfo, sizeof(WINDOWINFO));
        wndInfo.cbSize = sizeof(wndInfo);
        GetWindowInfo(hWnd, &wndInfo);
        wndRect = wndInfo.rcWindow;
    }

    return MonitorFromRECT(&wndRect);
}


/// <summary>
/// Returns the monitor which contains the biggest area of the specified window.
/// </summary>
UINT MonitorInfo::MonitorFromRECT(LPRECT rect) {
    int maxArea = 0;
    int area = 0;
    UINT monitor = UINT(-1);

    // It happened...
    if (rect->right <= rect->left)
    {
        rect->right = rect->left + 1;
    }
    if (rect->bottom <= rect->top)
    {
        rect->bottom = rect->top + 1;
    }

    // Figure out which monitor contains the bigest part of the RECT.
    for (UINT i = 0; i < m_monitors.size(); i++) {
        area = Math::RectIntersectArea(rect, &m_monitors[i].rect);
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
    this->m_monitors.clear();

    this->m_virtualDesktop.rect.left = GetSystemMetrics(SM_XVIRTUALSCREEN);
    this->m_virtualDesktop.rect.top = GetSystemMetrics(SM_YVIRTUALSCREEN);
    this->m_virtualDesktop.width = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    this->m_virtualDesktop.height = GetSystemMetrics(SM_CYVIRTUALSCREEN);
    this->m_virtualDesktop.rect.right = this->m_virtualDesktop.width + this->m_virtualDesktop.rect.left;
    this->m_virtualDesktop.rect.bottom = this->m_virtualDesktop.height + this->m_virtualDesktop.rect.top;
    ZeroMemory(&this->m_virtualDesktop.workArea, sizeof(RECT));
    this->m_virtualDesktop.workAreaHeight = 0;
    this->m_virtualDesktop.workAreaWidth = 0;

    EnumDisplayMonitors(NULL, NULL, EnumMonitorsCallback, (LPARAM)this);
}


/// <summary>
/// Callback for EnumDisplayMonitors. Adds a monitor to the list of monitors.
/// </summary>
/// <param name="hMonitor">Handle to the monitor to add.</param>
/// <param name="lParam">A pointer to the MonitorInfo class to add this monitor to.</param>
BOOL CALLBACK EnumMonitorsCallback(HMONITOR hMonitor, HDC, LPRECT, LPARAM lParam) {
    MonitorInfo * pmInfo;
    MonitorInfo::Monitor mInfo;
    MONITORINFO mi;

    pmInfo = (MonitorInfo *)lParam;

    mi.cbSize = sizeof(MONITORINFO);
    GetMonitorInfo(hMonitor, &mi);

    mInfo.rect = mi.rcMonitor;
    mInfo.height = mi.rcMonitor.bottom - mi.rcMonitor.top;
    mInfo.width = mi.rcMonitor.right - mi.rcMonitor.left;

    mInfo.workArea = mi.rcWork;
    mInfo.workAreaHeight = mi.rcWork.bottom - mi.rcWork.top;
    mInfo.workAreaWidth = mi.rcWork.right - mi.rcWork.left;

    // The primary monitor goes in position 0
    if ((mi.dwFlags & MONITORINFOF_PRIMARY) == MONITORINFOF_PRIMARY) {
        pmInfo->m_monitors.insert(pmInfo->m_monitors.begin(), mInfo);
    }
    else {
        pmInfo->m_monitors.push_back(mInfo);
    }

    return TRUE;
}
