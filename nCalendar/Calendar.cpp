/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  Calendar.cpp
 *  The nModules Project
 *
 *  A calendar.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "Calendar.hpp"


/// <summary>
/// Constructor
/// </summary>
Calendar::Calendar(LPCWSTR calendarName)
    : Drawable(calendarName)
{
    // Load state settings
    StateRender<State>::InitData initData;
    mStateRender.Load(initData, mSettings);
}


/// <summary>
/// Destructor
/// </summary>
Calendar::~Calendar()
{
}


/// <summary>
/// Handles window messages
/// </summary>
LRESULT WINAPI Calendar::HandleMessage(HWND window, UINT msg, WPARAM wParam, LPARAM lParam, LPVOID)
{
    return DefWindowProc(window, msg, wParam, lParam);
}
