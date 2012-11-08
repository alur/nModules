/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  TaskSwitcher.hpp
 *  The nModules Project
 *
 *  Manages the alt-tab window.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

#include <dwmapi.h>
#include "../nShared/Drawable.hpp"
#include "../nShared/DrawableWindow.hpp"
#include "../nShared/LayoutSettings.hpp"
#include "TaskThumbnail.hpp"

class TaskSwitcher : public Drawable {
public:
    explicit TaskSwitcher();
    virtual ~TaskSwitcher();

    void HandleAltTab();
    void HandleAltShiftTab();

    LRESULT WINAPI HandleMessage(HWND, UINT, WPARAM, LPARAM, LPVOID);

private:
    static bool IsTaskbarWindow(HWND hWnd);
    static BOOL CALLBACK LoadWindowsCallback(HWND window, LPARAM taskSwitcher);

    int windowsPerRow;

    int taskWidth;
    int taskHeight;

    vector<TaskThumbnail*> shownWindows;
    int selectedWindow;

    void LoadSettings();
    void UpdateActiveWindow();

    void AddWindow(HWND window);

    void Show();
    void Hide();

    LayoutSettings layoutSettings;
};
