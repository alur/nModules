/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  TaskSwitcher.hpp
 *  The nModules Project
 *
 *  Manages the alt-tab window.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

#include <dwmapi.h>
#include "../nShared/DrawableWindow.hpp"

class TaskSwitcher : public MessageHandler {
public:
    explicit TaskSwitcher();
    virtual ~TaskSwitcher();

    void HandleAltTab();
    void HandleAltShiftTab();

    LRESULT WINAPI HandleMessage(HWND, UINT, WPARAM, LPARAM, LPVOID);

private:
    typedef struct {
        HTHUMBNAIL thumbnail;
        HWND window;
    } ShownWindow;

    static bool IsTaskbarWindow(HWND hWnd);
    static BOOL CALLBACK LoadWindowsCallback(HWND window, LPARAM taskSwitcher);

    vector<ShownWindow> shownWindows;
    int selectedWindow;

    void LoadSettings();
    void UpdateActiveWindow();

    void AddWindow(HWND window);
    void LoadWindows();
    void ClearWindows();

    Settings* settings;
    DrawableWindow* window;
};
