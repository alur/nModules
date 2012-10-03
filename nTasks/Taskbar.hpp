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
#include "../nShared/IDrawableMessageHandler.hpp"
#include "../nShared/LayoutSettings.hpp"

using std::map;

class Taskbar: public IDrawableMessageHandler {
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
    LRESULT WINAPI HandleMessage(HWND, UINT, WPARAM, LPARAM);
    void Relayout();

private:
    // Removes a task from this taskbar
    void RemoveTask(map<HWND, TaskButton*>::iterator iter);

    // The name of this taskbar
    LPCSTR name;

    // The taskbar's window
    DrawableWindow* window;

    // Settings for painting the taskbar background
    Settings* settings;

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
