/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  Taskbar.hpp
 *  The nModules Project
 *
 *  Represents a taskbar. Contains TaskButtons.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

#include <map>
#include "TaskButton.hpp"
#include "../nShared/DrawableWindow.hpp"
#include "../nShared/MessageHandler.hpp"
#include "../nShared/LayoutSettings.hpp"
#include "../nShared/Drawable.hpp"

using std::map;

class Taskbar: public Drawable {
public:
    // Ways to sort the tasks.
    enum ORDERING {
        BYAPPLICATION,
        BYTITLE,
        BYTIMEADDED
    };

    explicit Taskbar(LPCSTR);
    virtual ~Taskbar();

    void LoadSettings(bool = false);
    TaskButton* AddTask(HWND, UINT, bool);
    bool MonitorChanged(HWND hWnd, UINT monitor, TaskButton** out);
    void RemoveTask(HWND);
    void Relayout();
    LRESULT WINAPI HandleMessage(HWND window, UINT msg, WPARAM wParam, LPARAM lParam);

private:
    // Removes a task from this taskbar
    void RemoveTask(map<HWND, TaskButton*>::iterator iter);

    // The name of this taskbar
    LPCSTR name;

    // Settings which define how to organize the buttons
    LayoutSettings* layoutSettings;

    // The taskbar buttons
    map<HWND, TaskButton*> buttons;

    // The maximum width of a taskbar button
    UINT buttonMaxWidth, buttonMaxHeight, buttonWidth, buttonHeight;

    // The monitor to display tasks for
    UINT monitor;

    //
    ORDERING ordering;
};
