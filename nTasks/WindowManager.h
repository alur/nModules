/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*  WindowManager.h
*  The nModules Project
*
*  Monitors all existing top-level windows.
*  
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

#include <string>

//
typedef std::map<std::tstring, Taskbar*> TaskbarMap;

namespace WindowManager
{
    /// <summary>
    /// Holds all information about a particular top level window.
    /// </summary>
    struct WindowInformation
    {
        ~WindowInformation()
        {
            if (hIcon != nullptr)
            {
                DestroyIcon(hIcon);
            }
            if (hOverlayIcon != nullptr)
            {
                DestroyIcon(hOverlayIcon);
            }
        }

        // The monitor this window is currently on
        UINT uMonitor;

        // Pointers to the buttons that represent this window
        std::vector<TaskButton*> buttons;

        // The main icon of the window
        HICON hIcon;

        // Any overlay icon
        HICON hOverlayIcon;

        // The progress state of the window
        TBPFLAG progressState;

        // If the progress is determinate, how far along we are
        USHORT progress;
    };

    // Some helpful typedefs
    typedef std::map<HWND, WindowInformation> WindowMap;

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
    void SetOverlayIcon(HWND hWnd, HICON hIcon);
    void SetProgressState(HWND hWnd, TBPFLAG state);
    void SetProgressValue(HWND hWnd, USHORT progress);
    void UpdateWindow(HWND hWnd, LPARAM data);
    void UpdateWindowMonitors();
    void AddExisting();
}
