/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  nMediaInfo.cpp
 *  The nModules Project
 *
 *  Main .cpp file for the nMediaInfo module.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "../nShared/LiteStep.h"
#include "nMediaInfo.h"
#include "../nShared/LSModule.hpp"
#include <map>
#include "TextFunctions.h"
#include "CoverArt.hpp"
#include "Bangs.h"
#include "Version.h"
#include "../nShared/ErrorHandler.h"
#include <atomic>
#include <thread>

using std::map;

// The LSModule class
LSModule gLSModule(TEXT(MODULE_NAME), TEXT(MODULE_AUTHOR), MakeVersion(MODULE_VERSION));

// The messages we want from the core
UINT gLSMessages[] = { LM_GETREVID, LM_REFRESH, LM_FULLSCREENACTIVATED,
    LM_FULLSCREENDEACTIVATED, 0 };

//
UINT WinampSongChangeMsg = 0;

//
map<wstring, CoverArt> gCoverArt;


/// <summary>
/// Called by the LiteStep core when this module is loaded.
/// </summary>
EXPORT_CDECL(int) initModuleW(HWND /* parent */, HINSTANCE instance, LPCWSTR /* path */) {
  if (!gLSModule.Initialize(nullptr, instance)) {
    return 1;
  }

  if (!gLSModule.ConnectToCore(MakeVersion(CORE_VERSION))) {
    return 1;
  }

  WinampSongChangeMsg = RegisterWindowMessageW(L"WinampSongChange");

  // Load settings
  LoadSettings();

  TextFunctions::_Register();
  Bangs::_Register();
  Update();

  return 0;
}


/// <summary>
/// Called by the LiteStep core when this module is about to be unloaded.
/// </summary>
EXPORT_CDECL(void) quitModule(HINSTANCE /* instance */) {
  TextFunctions::_UnRegister();
  Bangs::_Unregister();

  gCoverArt.clear();

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
  if (message == WinampSongChangeMsg) {
    Update();
    return 1;
  }
  switch (message) {
  case WM_CREATE:
    SendMessage(LiteStep::GetLitestepWnd(), LM_REGISTERMESSAGE, (WPARAM)window,
        (LPARAM)gLSMessages);
    return 0;

  case WM_DESTROY:
    SendMessage(LiteStep::GetLitestepWnd(), LM_UNREGISTERMESSAGE, (WPARAM)window,
        (LPARAM)gLSMessages);
    return 0;

  case WindowMessages::WM_TEXTUPDATENOTIFY:
    TextFunctions::_UpdateNotify();
    return 0;

  case WindowMessages::WM_COVERARTUPDATE:
    ((CoverArt*)wParam)->SetSource((IWICBitmapSource*)lParam);
    return 0;

  case LM_FULLSCREENACTIVATED:
    for (auto &coverart : gCoverArt) {
      coverart.second.GetWindow()->FullscreenActivated((HMONITOR)wParam, (HWND)lParam);
    }
    return 0;

  case LM_FULLSCREENDEACTIVATED:
    for (auto &coverart : gCoverArt) {
      coverart.second.GetWindow()->FullscreenDeactivated((HMONITOR)wParam);
    }
    return 0;

  case LM_REFRESH:
    return 0;
  }
  return DefWindowProc(window, message, wParam, lParam);
}


/// <summary>
/// Loads settings.
/// </summary>
void LoadSettings() {
  LiteStep::IterateOverLineTokens(L"*nCoverArt", CreateCoverart);
}


/// <summary>
/// Loads settings.
/// </summary>
void CreateCoverart(LPCTSTR name) {
  if (gCoverArt.find(name) == gCoverArt.end()) {
    gCoverArt.emplace(std::piecewise_construct, std::forward_as_tuple(name),
        std::forward_as_tuple(name));
  } else {
    ErrorHandler::Error(ErrorHandler::Level::Critical,
        L"Attempt to (re)create the already existing CoverArt %s!", name);
  }
}


/// <summary>
/// Updates the cover art.
/// </summary>
void Update() {
  // Set while the updater thread is running.
  static std::atomic_flag closed = ATOMIC_FLAG_INIT;

  if (!closed.test_and_set()) {
    std::thread([] () {
      TextFunctions::_Update();
      SendMessage(gLSModule.GetMessageWindow(), WindowMessages::WM_TEXTUPDATENOTIFY, 0, 0);

      for (auto &coverArt : gCoverArt) {
        coverArt.second.Update();
      }

      closed.clear();
    }).detach();
  }
}
