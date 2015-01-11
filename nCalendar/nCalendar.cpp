//-------------------------------------------------------------------------------------------------
// /nCalendar/nCalendar.cpp
// The nModules Project
//
// LiteStep entry points for nCalendar.
//-------------------------------------------------------------------------------------------------
#include "Calendar.hpp"
#include "Version.h"

#include "../nShared/ErrorHandler.h"
#include "../nShared/LiteStep.h"
#include "../nShared/LSModule.hpp"

#include "../Utilities/Common.h"

#include <map>

// The LSModule class
LSModule gLSModule(TEXT(MODULE_NAME), TEXT(MODULE_AUTHOR), MakeVersion(MODULE_VERSION));

// The messages we want from the core
const UINT gLSMessages[] = { LM_GETREVID, LM_REFRESH, LM_FULLSCREENACTIVATED,
  LM_FULLSCREENDEACTIVATED, 0 };

// All current calendars
std::map<std::wstring, Calendar> gCalendars;



/// <summary>
/// Reads through the .rc files and creates calendars.
/// </summary>
static void Unload() {
  gCalendars.clear();
}


/// <summary>
/// Creates a new calendar with the specified name.
/// </summary>
/// <param name="calendarName">The name of the calendar to create.</param>
static void CreateCalendar(LPCTSTR calendarName) {
  if (gCalendars.find(calendarName) == gCalendars.end()) {
    gCalendars.emplace(
      std::piecewise_construct,
      std::forward_as_tuple(calendarName),
      std::forward_as_tuple(calendarName)
      );
  } else {
    ErrorHandler::Error(ErrorHandler::Level::Critical,
      L"Attempt to (re)create the already existing calendar %s!", calendarName);
  }
}


/// <summary>
/// Reads through the .rc files and creates calendars.
/// </summary>
static void LoadSettings() {
  LiteStep::IterateOverLineTokens(L"*nCalendar", CreateCalendar);
}


/// <summary>
/// Called by the LiteStep core when this module is loaded.
/// </summary>
EXPORT_CDECL(int) initModuleW(HWND parent, HINSTANCE instance, LPCWSTR /* path */) {
  if (!gLSModule.Initialize(parent, instance)) {
    return 1;
  }
  if (!gLSModule.ConnectToCore(MakeVersion(CORE_VERSION))) {
    return 1;
  }
  LoadSettings();

  return 0;
}


/// <summary>
/// Called by the LiteStep core when this module is about to be unloaded.
/// </summary>
void quitModule(HINSTANCE /* instance */) {
  Unload();
  gLSModule.DeInitalize();
}


/// <summary>
/// Handles the main window's messages.
/// </summary>
/// <param name="window">The window the message is for.</param>
/// <param name="message">The type of message.</param>
/// <param name="wParam">wParam</param>
/// <param name="lParam">lParam</param>
LRESULT WINAPI LSMessageHandler(HWND window, UINT message, WPARAM wParam, LPARAM lParam) {
  switch(message) {
  case WM_CREATE:
    SendMessage(LiteStep::GetLitestepWnd(), LM_REGISTERMESSAGE, (WPARAM)window, (LPARAM)gLSMessages);
    return 0;

  case WM_DESTROY:
    SendMessage(LiteStep::GetLitestepWnd(), LM_UNREGISTERMESSAGE, (WPARAM)window, (LPARAM)gLSMessages);
    return 0;

  case LM_REFRESH:
    Unload();
    LoadSettings();
    return 0;

  case LM_FULLSCREENACTIVATED:
    for (auto &item : gCalendars) {
      item.second.GetWindow()->FullscreenActivated((HMONITOR) wParam, (HWND) lParam);
    }
    return 0;

  case LM_FULLSCREENDEACTIVATED:
    for (auto &item : gCalendars) {
      item.second.GetWindow()->FullscreenDeactivated((HMONITOR) wParam);
    }
    return 0;
  }
  return DefWindowProc(window, message, wParam, lParam);
}
