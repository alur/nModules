/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  Taskbar.hpp
 *  The nModules Project
 *
 *  Declaration of the Taskbar class.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

#include <map>
#include "TaskButton.hpp"
#include "../nShared/DrawableWindow.hpp"
#include "../nShared/IDrawableMessageHandler.hpp"

using std::map;

class Taskbar: public IDrawableMessageHandler {
public:
    explicit Taskbar(LPCSTR);
    virtual ~Taskbar();

    void LoadSettings(bool = false);
    TaskButton* AddTask(HWND, UINT, bool);
    bool MonitorChanged(HWND hWnd, UINT monitor, TaskButton** pOut);
    void RemoveTask(HWND);
    LRESULT WINAPI HandleMessage(HWND, UINT, WPARAM, LPARAM);
    void Relayout();

private:

    void RemoveTask(map<HWND, TaskButton*>::iterator iter);

    // The name of this taskbar
    LPCSTR m_pszName;

    // The taskbar's window
    DrawableWindow* m_pWindow;

    // Settings for painting the taskbar background
    Settings* settings;

    // The taskbar buttons
    map<HWND, TaskButton*> m_buttons;

    // The maximum width of a taskbar button
    UINT m_uMaxButtonWidth;

    // The monitor to display tasks for
    UINT m_uMonitor;
};
