/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*  WindowManager.h                                                  July, 2012
*  The nModules Project
*
*  Monitors all existing top-level windows.
*      
*  Erik Welander
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#ifndef WINDOWMANAGER_H
#define WINDOWMANAGER_H

namespace WindowManager {
    /// <summary>
    /// Holds all information about a particular top level window.
    /// </summary>
    typedef struct WindowInformation {
        UINT uMonitor;
        vector<TaskButton*> buttons;
        HICON hIcon;
    } WindowInformation;

    // Some helpful typedefs
    typedef map<LPCSTR, Taskbar*>::const_iterator TASKBARCITER;
    typedef map<HWND, WindowInformation>::const_iterator WNDMAPCITER;
    typedef map<HWND, WindowInformation>::iterator WNDMAPITER;

    void Start();
    void Stop();
    bool IsTaskbarWindow(HWND hWnd);
    void SetActive(HWND hWnd);
    LRESULT ShellMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    BOOL CALLBACK AddWindow(HWND hWnd, LPARAM lParam);
    void RemoveWindow(HWND hWnd);
    LRESULT GetMinRect(HWND, LPPOINTS);
    void UpdateIcon(HWND hWnd);
    void CALLBACK UpdateIconCallback(HWND hWnd, UINT uMsg, ULONG_PTR dwData, LRESULT lResult);
    void SetIcon(HWND, HICON);
    void UpdateWindow(HWND hWnd, LPARAM data);
    void UpdateWindowMonitors();
    void UpdateMonitor(WNDMAPITER iter);
}

#endif /* WINDOWMANAGER_H */
