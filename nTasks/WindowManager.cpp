/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*	WindowManager.cpp												July, 2012
*	The nModules Project
*
*	Monitors all existing top-level windows. Forwards notifications to the
*	taskbars.
*      
*													             Erik Welander
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "../headers/lsapi.h"
#include "TaskButton.hpp"
#include "Taskbar.hpp"
#include "WindowManager.h"
#include "../nShared/MonitorInfo.hpp"
#include <map>
#include <vector>
#include <assert.h>

using std::map;
using std::vector;
using std::pair;

// All current taskbars
extern map<LPCSTR, Taskbar*> g_Taskbars;

// Handle to the message handler window
extern HWND g_hWndMsgHandler;

namespace WindowManager {
    // The currently active window
    HWND activeWindow = NULL;

    // Contains all current top-level windows.
    map<HWND, WindowInformation> windowMap;

    // Information about the current monitor layout.
    MonitorInfo* g_pMonitorInfo = NULL;

    // True if the windowmanager is running.
    bool isStarted = false;
}


/// <summary>
/// Starts the window manager.
/// </summary>
void WindowManager::Start() {
    // Check that we aren't already running
    assert(!isStarted);
    isStarted = true;

    // Initalize our monitorinfo class
    g_pMonitorInfo = new MonitorInfo();

    // Grab all existing top level windows.
    EnumDesktopWindows(NULL, AddWindow, 1);

    for (TASKBARCITER iter = g_Taskbars.begin(); iter != g_Taskbars.end(); iter++) {
        iter->second->Relayout();
    }

    // Get the currently active window
    SetActive(GetForegroundWindow());

    SetTimer(g_hWndMsgHandler, 1337, 250, NULL);
}


/// <summary>
/// Stops the window manager. Removes all windows from the taskbars and drops
/// the shellhook.
/// </summary>
void WindowManager::Stop() {
    // Check that we are currently running
    assert(isStarted);

    // Clean up
    KillTimer(g_hWndMsgHandler, 1337);
    delete g_pMonitorInfo;
    activeWindow = NULL;
    windowMap.clear();
    isStarted = false;
}


/// <summary>
/// Adds the specified top level window to the list of windows.
/// </summary>
BOOL CALLBACK WindowManager::AddWindow(HWND hWnd, LPARAM lParam) {
    WindowInformation wndInfo;
    WCHAR szTitle[MAX_LINE_LENGTH];

    // Check that we are currently running
    assert(isStarted);

    // If we should add this window to the taskbars
    if (IsTaskbarWindow(hWnd)) {

        // Get information about the window
        GetWindowTextW(hWnd, szTitle, sizeof(szTitle)/sizeof(WCHAR));
        wndInfo.uMonitor = g_pMonitorInfo->MonitorFromHWND(hWnd);

        // Add it to any taskbar that wants it
        for (TASKBARCITER iter = g_Taskbars.begin(); iter != g_Taskbars.end(); iter++) {
            TaskButton* taskButton = iter->second->AddTask(hWnd, wndInfo.uMonitor, lParam == 1);

            // If the taskbar created a button for this window
            if (taskButton != NULL) {
                // Add it to our list of buttons
                wndInfo.buttons.push_back(taskButton);

                // Set the icon and text of the window.
                taskButton->SetText(szTitle);
            }
        }

        windowMap.insert(pair<HWND, WindowInformation>(hWnd, wndInfo));
        UpdateIcon(hWnd);
    }

    return TRUE;
}


/// <summary>
/// Updates the currently active window.
/// </summary>
void WindowManager::SetActive(HWND hWnd) {
    // Check that we are currently running
    assert(isStarted);

    // Remove the active flag from the previously active buttons
    WNDMAPCITER iter = windowMap.find(activeWindow);
    if (iter != windowMap.end()) {
        for (vector<TaskButton*>::const_iterator iter2 = iter->second.buttons.begin(); iter2 != iter->second.buttons.end(); iter2++) {
            (*iter2)->Deactivate();
        }
    }

    // Swap the active window
    activeWindow = hWnd;

    // And set the flag for the now active buttons
    iter = windowMap.find(activeWindow);
    if (iter != windowMap.end()) {
        for (vector<TaskButton*>::const_iterator iter2 = iter->second.buttons.begin(); iter2 != iter->second.buttons.end(); iter2++) {
            (*iter2)->Activate();
        }
    }
}


/// <summary>
/// Removes the specified window from all taskbars.
/// </summary>
void WindowManager::RemoveWindow(HWND hWnd) {
    // Check that we are currently running
    assert(isStarted);

    WNDMAPITER iter = windowMap.find(hWnd);
    if (iter != windowMap.end()) {
        // Remove all buttons
        for (TASKBARCITER iter2 = g_Taskbars.begin(); iter2 != g_Taskbars.end(); iter2++) {
            iter2->second->RemoveTask(hWnd);
        }

        windowMap.erase(iter);
    }

    if (activeWindow == hWnd)
        activeWindow = NULL;
}


/// <summary>
/// Removes the specified window from all taskbars.
/// </summary>
void WindowManager::UpdateWindow(HWND hWnd, LPARAM lParam) {
    // Check that we are currently running
    assert(isStarted);

    WNDMAPITER iter = windowMap.find(hWnd);
    if (iter != windowMap.end()) {
        // Update the text
        WCHAR szTitle[MAX_LINE_LENGTH];
        GetWindowTextW(hWnd, szTitle, sizeof(szTitle)/sizeof(WCHAR));
        for (vector<TaskButton*>::const_iterator iter2 = iter->second.buttons.begin(); iter2 != iter->second.buttons.end(); iter2++) {
            (*iter2)->SetText(szTitle);
        }

        // Update the icon
        UpdateIcon(hWnd);

        // Check if we should be flashing
        if (lParam == TRUE) {
            for (vector<TaskButton*>::const_iterator iter2 = iter->second.buttons.begin(); iter2 != iter->second.buttons.end(); iter2++) {
                (*iter2)->Flash();
            }
        }
    }
}


/// <summary>
/// Retrives the rectangle of the first taskbutton for the specified HWND.
/// </summary>
LRESULT WindowManager::GetMinRect(HWND hWnd, LPPOINTS lpPoints) {
    // Check that we are currently running
    assert(isStarted);

    WNDMAPCITER iter = windowMap.find(hWnd);
    if (iter != windowMap.end() && !iter->second.buttons.empty()) {
        iter->second.buttons[0]->GetMinRect(lpPoints);
        return 1;
    }
    return 0;
}


/// <summary>
/// Updates the monitor information for each window.
/// </summary>
void WindowManager::UpdateMonitor(WNDMAPITER iter) {
    if (iter != windowMap.end()) {
        int monitor = g_pMonitorInfo->MonitorFromHWND(iter->first);
        if ((UINT)monitor != iter->second.uMonitor) {
            HWND hWnd = iter->first;
            RemoveWindow(hWnd);
            AddWindow(hWnd, NULL);
        }
    }
}


/// <summary>
/// Updates the monitor information for each window.
/// </summary>
void WindowManager::UpdateWindowMonitors() {
    vector<HWND> mods;
    for (WNDMAPITER iter = windowMap.begin(); iter != windowMap.end(); iter++) {
        int monitor = g_pMonitorInfo->MonitorFromHWND(iter->first);
        if ((UINT)monitor != iter->second.uMonitor) {
            mods.push_back(iter->first);
        }
    }
    for (vector<HWND>::iterator iter = mods.begin(); iter != mods.end(); iter++) {
        RemoveWindow(*iter);
        AddWindow(*iter, NULL);
    }
}


/// <summary>
/// Processes shell hook messages.
/// </summary>
LRESULT WindowManager::ShellMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    // Check that we are currently running
    assert(isStarted);

    switch (uMsg) {

        // A window is being minimized or restored, the system needs the coordinates of
        // the taskbutton for the window animation.
    case LM_GETMINRECT:
        return GetMinRect((HWND)wParam, (LPPOINTS)lParam);

        // The active input language has changed
    case LM_LANGUAGE:
        return 0;

        // Text/Icon/Blinking change
    case LM_REDRAW:
        UpdateWindow((HWND)wParam, lParam);
        return 0;

        // The active window has changed
    case LM_WINDOWACTIVATED:
        SetActive((HWND)wParam);
        return 0;

        // A new top level window has been created
    case LM_WINDOWCREATED:
        AddWindow((HWND)wParam, NULL);
        return 0;

        // A top level window has been destroyed
    case LM_WINDOWDESTROYED:
        RemoveWindow((HWND)wParam);
        return 0;

        // A top level window has been replaced
    case LM_WINDOWREPLACED:
        AddWindow((HWND)lParam, NULL);
        return 0;

        // A window is about to be replaced
    case LM_WINDOWREPLACING:
        RemoveWindow((HWND)wParam);
        return 0;

        // Windows 8+ A window has moved to a different monitor
    case LM_MONITORCHANGED:
        return 0;

        // The display layout has changed.
    case WM_DISPLAYCHANGE:
        g_pMonitorInfo->Update();
        UpdateWindowMonitors();
        return 0;

        //
    case WM_TIMER:
        switch(wParam) {
        case 1337:
            UpdateWindowMonitors();
            return 0;
        }
    }
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}


/// <summary>
/// Processes callwndproc hook messages.
/// </summary>
//LRESULT WindowManager::ShellMessageHook() {
//}


/// <summary>
/// Determines if a window should be shown on the taskbar.
/// </summary>
bool WindowManager::IsTaskbarWindow(HWND hWnd) {
    // Make sure it's actually a window.
    if (!IsWindow(hWnd))
        return false;

    // And that it's visible
    if (!IsWindowVisible(hWnd))
        return false;

    LONG_PTR exStyle = GetWindowLongPtr(hWnd, GWL_EXSTYLE);

    // Windows with the WS_EX_APPWINDOW style should always be shown
    if ((exStyle & WS_EX_APPWINDOW) == WS_EX_APPWINDOW)
        return true;
    else if (GetParent(hWnd) != NULL) // Windows with parents should not be shown
        return false;
    else if (GetWindow(hWnd, GW_OWNER) != NULL) // Windows with owner should not be shown
        return false;
    else if ((exStyle & WS_EX_TOOLWINDOW) == WS_EX_TOOLWINDOW) // Toolwindows should not be shown on the taskbar
        return false;

    return true;
}


/// <summary>
/// Updates the icon of a particular HWND
/// </summary>
void WindowManager::SetIcon(HWND hWnd, HICON hIcon) {
    WNDMAPITER window = windowMap.find(hWnd);
    if (window != windowMap.end()) {
        if (hIcon != window->second.hIcon) {
            for (vector<TaskButton*>::const_iterator iter = window->second.buttons.begin(); iter != window->second.buttons.end(); iter++) {
                (*iter)->SetIcon(hIcon);
            }
            window->second.hIcon = hIcon;
        }
    }
}


/// <summary>
/// Updates the icon of a particular HWND
/// </summary>
void CALLBACK WindowManager::UpdateIconCallback(HWND hWnd, UINT uMsg, ULONG_PTR dwData, LRESULT lResult) {
    // We really only expect WM_GETICON messages.
    if (uMsg == WM_GETICON) {
        // If we got an icon back, use it.
        if (lResult != NULL) {
            SetIcon(hWnd, (HICON)lResult);
        }
        else switch (dwData) {
        case ICON_BIG:
            {
                SendMessageCallback(hWnd, WM_GETICON, ICON_SMALL, NULL, UpdateIconCallback, ICON_SMALL);
            }
            break;

        case ICON_SMALL:
            {
                SendMessageCallback(hWnd, WM_GETICON, ICON_SMALL2, NULL, UpdateIconCallback, ICON_SMALL2);
            }
            break;

        case ICON_SMALL2:
            {
                HICON hIcon;
                hIcon = (HICON)GetClassLongPtr(hWnd, GCLP_HICON);
                if (!hIcon)
                    hIcon = (HICON)GetClassLongPtr(hWnd, GCLP_HICONSM);
                SetIcon(hWnd, hIcon);
            }
            break;
        }
    }
}


/// <summary>
/// Updates the icon of a particular HWND
/// </summary>
void WindowManager::UpdateIcon(HWND hWnd) {
    SendMessageCallback(hWnd, WM_GETICON, ICON_BIG, NULL, UpdateIconCallback, ICON_BIG);
}
