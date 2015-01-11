//-------------------------------------------------------------------------------------------------
// /nClock/nClock.cpp
// The nModules Project
//
// nClock entry points.
//-------------------------------------------------------------------------------------------------
#include "Clock.hpp"
#include "Version.h"

#include "../nShared/ErrorHandler.h"
#include "../nShared/LiteStep.h"
#include "../nShared/LSModule.hpp"

#include <string>
#include <unordered_map>

// The LSModule class
LSModule gLSModule(TEXT(MODULE_NAME), TEXT(MODULE_AUTHOR), MakeVersion(MODULE_VERSION));

// The messages we want from the core
static const UINT sLSMessages[] = { LM_GETREVID, LM_REFRESH, LM_FULLSCREENACTIVATED,
  LM_FULLSCREENDEACTIVATED, 0 };

// All current clocks
std::unordered_map<std::wstring, Clock> gClocks;

static void LoadSettings();
static void DestroyClocks();
static void CreateClock(LPCWSTR clockName);


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
EXPORT_CDECL(void) quitModule(HINSTANCE /* instance */) {
  DestroyClocks();
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
  using LiteStep::GetLitestepWnd;

  switch(message) {
  case WM_CREATE:
    SendMessage(GetLitestepWnd(), LM_REGISTERMESSAGE, (WPARAM)window, (LPARAM)sLSMessages);
    return 0;

  case WM_DESTROY:
    SendMessage(GetLitestepWnd(), LM_UNREGISTERMESSAGE, (WPARAM)window, (LPARAM)sLSMessages);
    return 0;

  case LM_REFRESH:
    DestroyClocks();
    LoadSettings();
    return 0;

  case LM_FULLSCREENACTIVATED:
    for (auto &item : gClocks) {
      item.second.GetWindow()->FullscreenActivated((HMONITOR) wParam, (HWND) lParam);
    }
    return 0;

  case LM_FULLSCREENDEACTIVATED:
    for (auto &item : gClocks) {
      item.second.GetWindow()->FullscreenDeactivated((HMONITOR) wParam);
    }
    return 0;
  }
  return DefWindowProc(window, message, wParam, lParam);
}


/// <summary>
/// Destroys all clocks.
/// </summary>
void DestroyClocks() {
  gClocks.clear();
}


/// <summary>
/// Reads through the .rc files and creates clocks.
/// </summary>
void LoadSettings() {
  LiteStep::IterateOverLineTokens(L"*nClock", CreateClock);
}


/// <summary>
/// Creates a new clock with the specified name.
/// </summary>
/// <param name="clockName">The name of the clock to create.</param>
void CreateClock(LPCWSTR clockName) {
  if (gClocks.find(clockName) == gClocks.end()) {
    gClocks.emplace(
      std::piecewise_construct,
      std::forward_as_tuple(clockName),
      std::forward_as_tuple(clockName)
    );
  } else {
    ErrorHandler::Error(ErrorHandler::Level::Critical,
      L"Attempt to (re)create the already existing clock %s!", clockName);
  }
}
