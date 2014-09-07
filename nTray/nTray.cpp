//-------------------------------------------------------------------------------------------------
// /nTray/nTray.cpp
// The nModules Project
//
// nTray entry points.
//-------------------------------------------------------------------------------------------------
#include "Tray.hpp"
#include "TrayManager.h"
#include "Types.h"
#include "Version.h"

#include "../nShared/LiteStep.h"
#include "../nShared/LSModule.hpp"

#include <string>
#include <unordered_map>

#define WM_GOT_INITIAL_TRAY_ICONS WM_USER

// The messages we want from the core
const UINT gLSMessages[] = { LM_GETREVID, LM_REFRESH, LM_SYSTRAY, LM_SYSTRAYINFOEVENT,
    LM_FULLSCREENACTIVATED, LM_FULLSCREENDEACTIVATED, 0 };

// Handle to the tray notify window
HWND ghWndTrayNotify;

// All the trays we currently have loaded
TrayMap gTrays;

// The LiteStep module class
LSModule gLSModule(TEXT(MODULE_NAME), TEXT(MODULE_AUTHOR), MakeVersion(MODULE_VERSION));

// True for the first 100ms of nTrays life. Speeds up loading.
bool gInitPhase;


/// <summary>
/// Reads through the .rc files and creates trays.
/// </summary>
static void LoadSettings() {
  using std::forward_as_tuple;
  using std::piecewise_construct;

  LiteStep::IterateOverLineTokens(L"*nTray", [] (LPCWSTR name) -> void {
    gTrays.emplace(piecewise_construct, forward_as_tuple(name), forward_as_tuple(name));
  });
}


/// <summary>
/// Called by the LiteStep core when this module is loaded.
/// </summary>
/// <param name="parent">The module's parent window.</param>
/// <param name="instance">Handle to this module's instance.</param>
/// <param name="path">Path to the LiteStep directory.</param>
/// <returns>0 on success, non-zero on error.</returns>
/// <remarks>
/// If this function returns non-zero, the module will be unloaded immediately, without going
/// through quitModule.
/// </remarks>
EXPORT_CDECL(int) initModuleW(HWND parent, HINSTANCE instance, LPCWSTR /* path */) {
  gInitPhase = true;

  if (!gLSModule.Initialize(parent, instance)) {
    return 1;
  }

  if (!gLSModule.ConnectToCore(MakeVersion(CORE_VERSION))) {
    return 1;
  }

  LoadSettings();

  // Let the core know that we want the system tray icons
  ghWndTrayNotify = (HWND)SendMessage(LiteStep::GetLitestepWnd(), LM_SYSTRAYREADY, 0, 0);
  PostMessage(gLSModule.GetMessageWindow(), WM_GOT_INITIAL_TRAY_ICONS, 0, 0);

  // Register a bang for printing tray info
  LiteStep::AddBangCommand(L"!nTrayListIconIDs", [] (HWND, LPCTSTR) -> void {
    TrayManager::ListIconIDS();
  });

  return 0;
}


/// <summary>
/// Called by the core when this module is about to be unloaded.
/// </summary>
EXPORT_CDECL(void) quitModule(HINSTANCE /* instance */) {
  LiteStep::RemoveBangCommand(L"!nTrayListIconIDs");
  gTrays.clear();
  TrayManager::Stop();
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
    SendMessage(GetLitestepWnd(), LM_REGISTERMESSAGE, (WPARAM)window, (LPARAM)gLSMessages);
    return 0;

  case WM_DESTROY:
    SendMessage(GetLitestepWnd(), LM_UNREGISTERMESSAGE, (WPARAM)window, (LPARAM)gLSMessages);
    return 0;

  case LM_REFRESH:
    return 0;

  case LM_FULLSCREENACTIVATED:
    for (auto &tray : gTrays) {
      tray.second.GetWindow()->FullscreenActivated((HMONITOR)wParam, (HWND)lParam);
    }
    return 0;

  case LM_FULLSCREENDEACTIVATED:
    for (auto &tray : gTrays) {
      tray.second.GetWindow()->FullscreenDeactivated((HMONITOR)wParam);
    }
    return 0;

  case WM_GOT_INITIAL_TRAY_ICONS:
    gInitPhase = false;
    TrayManager::InitCompleted();
    return 0;

  case LM_SYSTRAY:
  case LM_SYSTRAYINFOEVENT:
    return TrayManager::ShellMessage(window, message, wParam, lParam);
  }
  return DefWindowProc(window, message, wParam, lParam);
}
