/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  TaskSwitcher.hpp
 *  The nModules Project
 *
 *  Manages the alt-tab window.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

#include "TaskThumbnail.hpp"
#include "ThumbnailSettings.hpp"

#include "../Utilities/Common.h"

#include "../nShared/Drawable.hpp"
#include "../nShared/LayoutSettings.hpp"
#include "../nShared/StateRender.hpp"
#include "../nShared/Window.hpp"

#include <dwmapi.h>

class TaskSwitcher : public Drawable
{
public:
    enum class State
    {
        Base = 0,
        Count
    };

public:
    explicit TaskSwitcher();
    virtual ~TaskSwitcher();
    
public:
    void Hide();

    void HandleAltTab();
    void HandleAltShiftTab();

    void HoveringOverTask(TaskThumbnail* task);

public:
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
private:
    int mWindowsPerRow; // How many windows there should be per row.
    int mPeekDelay; // When the taskswitcher has been inactive for this many ms; put it in peek mode.

private:
    // The size of each task thumbnail.
    D2D1_SIZE_F mTaskSize;

    // True if we have entered peek mode.
    bool mPeeking;
    UINT_PTR mPeekTimer;

    vector<TaskThumbnail*> mShownWindows;
    int mSelectedWindow;
    TaskThumbnail* mHoveredThumbnail;

    LayoutSettings mLayoutSettings;

    StateRender<State> mStateRender;
    ThumbnailSettings mThumbnailSettings;
};
