/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  Taskbar.hpp                                                     July, 2012
 *  The nModules Project
 *
 *  Declaration of the Taskbar class.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#ifndef TASKBAR_HPP
#define TASKBAR_HPP

#include <map>
#include "TaskButton.hpp"
#include "../nShared/DrawableWindow.hpp"

using std::map;

class Taskbar {
public:
    explicit Taskbar(LPCSTR);
    virtual ~Taskbar();

    void LoadSettings(bool = false);
    TaskButton* AddTask(HWND, UINT, bool);
    void RemoveTask(HWND);
    LRESULT WINAPI HandleMessage(UINT, WPARAM, LPARAM);
    void Relayout();

private:
    // The name of this taskbar
    LPCSTR m_pszName;

    // The taskbar's window
    DrawableWindow* m_pWindow;

    // Settings for painting the taskbar background
    PaintSettings* m_pPaintSettings;

    // The taskbar buttons
    map<HWND, TaskButton*> m_buttons;

    // The maximum width of a taskbar button
    UINT m_uMaxButtonWidth;

    // The monitor to display tasks for
    UINT m_uMonitor;
};

#endif /* TASKBAR_HPP */
