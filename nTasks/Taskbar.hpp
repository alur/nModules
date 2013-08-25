/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  Taskbar.hpp
 *  The nModules Project
 *
 *  Represents a taskbar. Contains TaskButtons.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

#include <map>
#include "ButtonSettings.hpp"
#include "TaskButton.hpp"
#include "../nShared/Window.hpp"
#include "../nShared/MessageHandler.hpp"
#include "../nShared/LayoutSettings.hpp"
#include "../nShared/Drawable.hpp"
#include "../nShared/WindowThumbnail.hpp"

class Taskbar: public Drawable
{
    // Public Typedefs
public:
    // Ways to sort the tasks.
    enum class SortingType
    {
        Application,
        Title,
        TimeAdded,
        Position
    };

    // Private Typedefs
private:
    typedef std::list<TaskButton*> ButtonList;
    typedef std::map<HWND, ButtonList::const_iterator> ButtonMap;

    enum class States
    {
        Base = 0,
        Count
    };

    // Constructors and destructors
public:
    explicit Taskbar(LPCTSTR);
    virtual ~Taskbar();

    // MessageHandler
public:
    LRESULT WINAPI HandleMessage(HWND window, UINT msg, WPARAM wParam, LPARAM lParam, LPVOID Window) override;

    // Public methods
public:
    void ShowThumbnail(HWND hwnd, LPRECT position);
    void HideThumbnail();

    void LoadSettings(bool = false);
    TaskButton* AddTask(HWND, UINT, bool);
    bool MonitorChanged(HWND hWnd, UINT monitor, TaskButton** out);
    void RemoveTask(HWND);
    void Relayout();
    void Repaint();

    // Private methods
private:
    // Removes a task from this taskbar
    void RemoveTask(ButtonMap::iterator iter);

    // 
private:
    // Settings which define how to organize the buttons
    LayoutSettings mLayoutSettings;

    // The taskbar buttons
    ButtonSettings mButtonSettings;
    ButtonMap mButtonMap;
    ButtonList mButtonList;

    // The maximum width of a taskbar button
    int mButtonMaxWidth;
    int mButtonMaxHeight;
    int mButtonWidth;
    int mButtonHeight;

    //
    StateRender<States> mStateRender;

    // The monitor to display tasks for
    UINT mMonitor;

    //
    SortingType mSortingType;

    //
    WindowThumbnail* mThumbnail;

    //
    bool mNoThumbnails;
};
