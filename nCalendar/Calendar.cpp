//-------------------------------------------------------------------------------------------------
// /nCalendar/Calendar.cpp
// The nModules Project
//
// A calendar.
//-------------------------------------------------------------------------------------------------
#include "Calendar.hpp"


/// <summary>
/// Constructor
/// </summary>
Calendar::Calendar(LPCWSTR calendarName)
    : Drawable(calendarName)
{
  WindowSettings windowSettings;
  windowSettings.Load(mSettings);
  mStateRender.Load(mSettings);

  mWindow->Initialize(windowSettings, &mStateRender);
}


/// <summary>
/// Handles window messages
/// </summary>
LRESULT WINAPI Calendar::HandleMessage(HWND window, UINT msg, WPARAM wParam, LPARAM lParam, LPVOID) {
  return DefWindowProc(window, msg, wParam, lParam);
}
