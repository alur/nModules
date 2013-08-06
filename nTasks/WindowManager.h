/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*  WindowManager.h
*  The nModules Project
*
*  Monitors all existing top-level windows.
*  
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

#include <string>

namespace WindowManager {
    /// <summary>
    /// Holds all information about a particular top level window.
    /// </summary>
    typedef struct WindowInformation {
        // The monitor this window is currently on
        UINT uMonitor;

        // Pointers to the buttons that represent this window
        vector<TaskButton*> buttons;

        // The main icon of the window
        HICON hIcon;

        // Any overlay icon
        HICON hOverlayIcon;

        // The progress state of the window
        TBPFLAG progressState;

        // If the progress is determinate, how far along we are
        USHORT progress;
    } WindowInformation;

    // Some helpful typedefs
    typedef map<std::tstring, Taskbar*>::const_iterator TASKBARCITER;
    typedef map<HWND, WindowInformation>::const_iterator WNDMAPCITER;
    typedef map<HWND, WindowInformation>::iterator WNDMAPITER;

    void Start();
    void Stop();
    bool IsTaskbarWindow(HWND hWnd);
    void SetActive(HWND hWnd);
    void MarkAsMinimized(HWND hWnd);
    LRESULT ShellMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    void AddWindow(HWND hWnd);
    void RemoveWindow(HWND hWnd);
    void MonitorChanged(HWND hWnd, UINT monitor);
    LRESULT GetMinRect(HWND, LPPOINTS);
    void UpdateIcon(HWND hWnd);
    void CALLBACK UpdateIconCallback(HWND hWnd, UINT uMsg, ULONG_PTR dwData, LRESULT lResult);
    void SetIcon(HWND, HICON);
    void UpdateWindow(HWND hWnd, LPARAM data);
    void UpdateWindowMonitors();
    void AddExisting();
    void SetOverlayIcon(HWND hWnd,  HICON hIcon);
}
