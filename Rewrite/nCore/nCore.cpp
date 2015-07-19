#include "Displays.hpp"
#include "Logger.hpp"
#include "Factories.h"
#include "MessageRegistrar.h"
#include "Messages.h"
#include "Timers.h"
#include "Pane.hpp"
#include "WindowMonitor.h"

#include "../nShared/LiteStep.h"

#include "../Headers/lsapi.h"
#include "../Headers/Macros.h"
#include "../Headers/Windows.h"

static const UINT sLsMessages[] = { LM_GETREVID, LM_REFRESH, LM_FULLSCREENACTIVATED,
  LM_FULLSCREENDEACTIVATED, 0 };
static const wchar_t sName[] = L"nCore";
static const VERSION sVersion = MakeVersion(1, 0, 0, 0);

Displays gDisplays;
Logger *gLogger = nullptr;
HINSTANCE gInstance = nullptr;
HWND gWindow = nullptr;


BOOL APIENTRY DllMain(HANDLE module, DWORD reasonForCall, LPVOID /* reserved */) {
  if (reasonForCall == DLL_PROCESS_ATTACH) {
#ifdef _DLL
    DisableThreadLibraryCalls((HINSTANCE)module);
#endif
  }
  return TRUE;
}


LRESULT WINAPI MessageHandler(HWND window, UINT message, WPARAM wParam, LPARAM lParam) {
  switch (message) {
  case LM_GETREVID:
    return HandleGetRevId(sName, sVersion, lParam);

  case LM_REFRESH:
    return 0;

  case WM_CREATE:
    SendMessage(GetLitestepWnd(), LM_REGISTERMESSAGE, WPARAM(window), LPARAM(sLsMessages));
    return 0;

  case WM_DESTROY:
    SendMessage(GetLitestepWnd(), LM_UNREGISTERMESSAGE, WPARAM(window), LPARAM(sLsMessages));
    return 0;

  case WM_DISPLAYCHANGE:
    gDisplays.Update();
    return 0;

  case WM_SETTINGCHANGE:
    if (wParam == SPI_SETWORKAREA) {
      gDisplays.Update();
      SendCoreMessage(NCORE_DISPLAYS_CHANGED, 0, 0);
    }
    return 0;

  case WM_TIMER:
    switch (wParam) {
    case NCORE_TIMER_WINDOW_MAINTENANCE:
      WindowMonitor::RunWindowMaintenance();
      return 0;
    }
    Timers::Handle(wParam, lParam);
    return 0;

  case LM_FULLSCREENACTIVATED:
    Pane::FullscreenActivated((HMONITOR)wParam, (HWND)lParam);
    return 0;

  case LM_FULLSCREENDEACTIVATED:
    Pane::FullscreenDeactivated((HMONITOR)wParam);
    return 0;

  case LM_MONITORCHANGED:
  case LM_REDRAW:
  case LM_WINDOWCREATED:
  case LM_WINDOWDESTROYED:
  case LM_WINDOWREPLACED:
  case LM_WINDOWREPLACING:
    return WindowMonitor::HandleMessage(message, wParam, lParam);
  }

  if (message >= NCORE_BROADCAST_LOW && message <= NCORE_BROADCAST_HIGH) {
    SendCoreMessage(message, wParam, lParam);
  }

  return DefWindowProc(window, message, wParam, lParam);
}


LRESULT WINAPI ModuleDefWndProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam) {
  if (message == WM_CREATE) {
    WNDPROC moduleProc = (WNDPROC)((LPCREATESTRUCT)lParam)->lpCreateParams;
    SetWindowLongPtr(window, GWLP_WNDPROC, (LONG_PTR)moduleProc);
    return moduleProc(window, message, wParam, lParam);
  }
  return DefWindowProc(window, message, wParam, lParam);
}


static HRESULT RegisterMessageClass(HINSTANCE instance) {
  WNDCLASSEX wc;
  ZeroMemory(&wc, sizeof(WNDCLASSEX));
  wc.cbSize = sizeof(WNDCLASSEX);
  wc.style = CS_NOCLOSE;
  wc.lpfnWndProc = ModuleDefWndProc;
  wc.cbClsExtra = 0;
  wc.cbWndExtra = 0;
  wc.hInstance = instance;
  wc.hIcon = nullptr;
  wc.hCursor = nullptr;
  wc.hbrBackground = nullptr;
  wc.lpszMenuName = nullptr;
  wc.lpszClassName = L"LSnModuleMsgHandler";
  wc.hIconSm = nullptr;
  if (RegisterClassEx(&wc) == NULL) {
    return HRESULT_FROM_WIN32(GetLastError());
  }

  return S_OK;
}


EXPORT_CDECL(int) initModuleW(HWND /* parent */, HINSTANCE instance, LPCWSTR /* path */) {
  gLogger = new Logger(sName);
  gInstance = instance;
  RegisterMessageClass(instance);
  CreateMessageHandler(instance, sName, MessageHandler, gWindow);
  Pane::CreateWindowClasses(instance);
  Factories::Create();
  WindowMonitor::Start();
  return 0;
}


EXPORT_CDECL(void) quitModule(HINSTANCE instance) {
  WindowMonitor::Stop();
  Factories::Destroy();
  Pane::DestroyWindowClasses(instance);
  DestroyWindow(gWindow);
  gWindow = nullptr;
  UnregisterClass(L"LSnModuleMsgHandler", instance);
  gInstance = nullptr;
  delete gLogger;
  gLogger = nullptr;
}


EXPORT_CDECL(VERSION) GetCoreVersion() {
  return sVersion;
}


EXPORT_CDECL(const IDisplays*) GetDisplays() {
  return (const IDisplays*)&gDisplays;
}
