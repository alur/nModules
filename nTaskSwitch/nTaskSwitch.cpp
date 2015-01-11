//-------------------------------------------------------------------------------------------------
// /nTaskSwitch/nTaskSwitch.cpp
// The nModules Project
//
// nTaskSwitch entry points.
//-------------------------------------------------------------------------------------------------
#include "TaskSwitcher.hpp"
#include "Version.h"

#include "../nShared/LiteStep.h"
#include "../nShared/LSModule.hpp"

#include <strsafe.h>

// The LSModule class
LSModule gLSModule(TEXT(MODULE_NAME), TEXT(MODULE_AUTHOR), MakeVersion(MODULE_VERSION));

// The messages we want from the core
static const UINT sLSMessages[] = { LM_GETREVID, LM_REFRESH, 0 };
UINT (WINAPI *DwmpActivateLivePreview)(UINT onOff, HWND hWnd, HWND topMost, UINT unknown);
UINT (WINAPI *DwmpActivateLivePreview2)(UINT onOff, HWND hWnd, HWND topMost, UINT unknown);
HWND gDesktopWindow;
static TaskSwitcher *sTaskSwitcher;

#define HOTKEY_ALTTAB 1
#define HOTKEY_SHIFTALTTAB 2


UINT WINAPI FakeLivePreview(UINT onOff, HWND hWnd, HWND topMost, UINT unknown) {
  TRACE("DwmpActivateLivePreview(%x, %x, %x, %x)", onOff, hWnd, topMost, unknown);
  //return DwmpActivateLivePreview2(onOff, hWnd, topMost, unknown);
  return 0;
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

  DwmpActivateLivePreview2 = (decltype(DwmpActivateLivePreview))GetProcAddress(
    GetModuleHandleW(L"DWMAPI.DLL"), (LPCSTR)0x71);
  DwmpActivateLivePreview = FakeLivePreview;
  if (!DwmpActivateLivePreview) {
    return 1;
  }
  gDesktopWindow = FindWindowW(L"DesktopBackgroundClass", nullptr);

  RegisterHotKey(gLSModule.GetMessageWindow(), HOTKEY_ALTTAB, MOD_ALT, VK_TAB);
  RegisterHotKey(gLSModule.GetMessageWindow(), HOTKEY_SHIFTALTTAB, MOD_ALT | MOD_SHIFT, VK_TAB);

  sTaskSwitcher = new TaskSwitcher();

  return 0;
}


/// <summary>
/// Called by the LiteStep core when this module is about to be unloaded.
/// </summary>
EXPORT_CDECL(void) quitModule(HINSTANCE /* instance */) {
  delete sTaskSwitcher;

  UnregisterHotKey(gLSModule.GetMessageWindow(), HOTKEY_ALTTAB);
  UnregisterHotKey(gLSModule.GetMessageWindow(), HOTKEY_SHIFTALTTAB);

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
    SendMessage(LiteStep::GetLitestepWnd(), LM_REGISTERMESSAGE, (WPARAM)window,
      (LPARAM)sLSMessages);
    return 0;

  case WM_DESTROY:
    SendMessage(LiteStep::GetLitestepWnd(), LM_UNREGISTERMESSAGE, (WPARAM)window,
      (LPARAM)sLSMessages);
    return 0;

  case LM_REFRESH:
    return 0;

  case WM_HOTKEY:
    switch (wParam) {
    case HOTKEY_ALTTAB:
      sTaskSwitcher->HandleAltTab();
      break;

    case HOTKEY_SHIFTALTTAB:
      sTaskSwitcher->HandleAltShiftTab();
      break;
    }
    return 0;
  }
  return DefWindowProc(window, message, wParam, lParam);
}
