//-------------------------------------------------------------------------------------------------
// /nIcon/nIcon.cpp
// The nModules Project
//
// LiteStep entry points for nIcon.
//-------------------------------------------------------------------------------------------------
#include "TileGroup.hpp"
#include "Version.h"

#include "../nShared/ErrorHandler.h"
#include "../nShared/LiteStep.h"
#include "../nShared/LSModule.hpp"

#include <string>
#include <unordered_map>

// The messages we want from the core
static const UINT gLSMessages[] = { LM_GETREVID, LM_REFRESH, LM_FULLSCREENACTIVATED,
    LM_FULLSCREENDEACTIVATED, 0 };

// All current icon groups
static std::unordered_map<std::wstring, TileGroup> sIconGroups;

// The LiteStep module class
LSModule gLSModule(MODULE_NAME, MODULE_AUTHOR, MakeVersion(MODULE_VERSION));

// The next window in the clipboard viewer chain
static HWND sNextClipboardViewer;


/// <summary>
/// Creates a new group with the specified name.
/// </summary>
/// <param name="groupName">The name of the group to create.</param>
void CreateGroup(LPCWSTR groupName) {
  if (sIconGroups.find(groupName) == sIconGroups.end()) {
    sIconGroups.emplace(
      std::piecewise_construct,
      std::forward_as_tuple(groupName),
      std::forward_as_tuple(groupName)
      );
  } else {
    ErrorHandler::Error(ErrorHandler::Level::Critical,
      L"Attempt to (re)create the already existing group %s!", groupName);
  }
}


/// <summary>
/// Reads through the .rc files and creates labels.
/// </summary>
static void LoadSettings() {
  LiteStep::IterateOverLineTokens(L"*nIcon", CreateGroup);
}


/// <summary>
/// Unloads everything created by .rc settings or at runtime.
/// </summary>
static void Unload() {
  sIconGroups.clear();
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

  sNextClipboardViewer = SetClipboardViewer(gLSModule.GetMessageWindow());
  OleInitialize(nullptr);
  LoadSettings();
  return 0;
}


/// <summary>
/// Called by the LiteStep core when this module is about to be unloaded.
/// </summary>
EXPORT_CDECL(void) quitModule(HINSTANCE /* instance */) {
  Unload();
  gLSModule.DeInitalize();
  OleUninitialize();
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
    ChangeClipboardChain(window, sNextClipboardViewer);
    return 0;

  case WM_DRAWCLIPBOARD:
    for (auto &item : sIconGroups) {
      item.second.HandleClipboardChange();
    }
    SendMessage(sNextClipboardViewer, message, wParam, lParam);
    return 0;

  case WM_CHANGECBCHAIN:
    if (HWND(wParam) == sNextClipboardViewer) {
      sNextClipboardViewer = (HWND)lParam;
    } else if (sNextClipboardViewer != nullptr) {
      SendMessage(sNextClipboardViewer, message, wParam, lParam);
    }
    return 0;

  case LM_FULLSCREENACTIVATED:
    for (auto &item : sIconGroups) {
      item.second.GetWindow()->FullscreenActivated((HMONITOR)wParam, (HWND)lParam);
    }
    return 0;

  case LM_FULLSCREENDEACTIVATED:
    for (auto &item : sIconGroups) {
      item.second.GetWindow()->FullscreenDeactivated((HMONITOR)wParam);
    }
    return 0;

  case LM_REFRESH:
    Unload();
    LoadSettings();
    return 0;
  }
  return DefWindowProc(window, message, wParam, lParam);
}
