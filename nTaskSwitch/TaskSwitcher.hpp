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

class TaskSwitcher : public Drawable
{
public:
    explicit TaskSwitcher();
    virtual ~TaskSwitcher();
    
    void Hide();

    void HandleAltTab();
    void HandleAltShiftTab();

    void HoveringOverTask(TaskThumbnail* task);

    LRESULT WINAPI HandleMessage(HWND, UINT, WPARAM, LPARAM, LPVOID);

private:
    static bool IsTaskbarWindow(HWND hWnd);
    static BOOL CALLBACK LoadWindowsCallback(HWND window, LPARAM taskSwitcher);

    void LoadSettings();
    void UpdateActiveWindow(int delta);

    void AddWindow(HWND window);
    void Preview(HWND window);

    void Show(int delta);

    //
    // Settings
    int windowsPerRow; // How many windows there should be per row.
    int peekDelay; // When the taskswitcher has been inactive for this many ms; put it in peek mode.

    int taskWidth;
    int taskHeight;

    bool peeking;
    UINT_PTR peekTimer;

    vector<TaskThumbnail*> shownWindows;
    int selectedWindow;
    TaskThumbnail* hoveredThumbnail;

    LayoutSettings layoutSettings;
};
