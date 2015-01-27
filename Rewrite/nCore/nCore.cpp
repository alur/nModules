#include "Displays.hpp"
#include "Factories.h"
#include "Pane.hpp"

#include "../nShared/LsModule.hpp"

#include "../nUtilities/lsapi.h"
#include "../nUtilities/Macros.h"
#include "../nUtilities/Windows.h"

static const UINT sLsMessages[] = { LM_GETREVID, LM_REFRESH, 0 };
static const VERSION sVersion = MakeVersion(0, 9, 0, 0);
LsModule gLsModule(L"nCore", MakeVersion(0, 9, 0, 0));
Displays gDisplays;
HINSTANCE gInstance = nullptr;


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
    return gLsModule.HandleGetRevId(lParam);

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
    }
    return 0;
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
  gInstance = instance;
  RegisterMessageClass(instance);
  gLsModule.CreateMessageHandler(instance, MessageHandler);
  Pane::CreateWindowClasses(instance);
  Factories::Create();
  return 0;
}


EXPORT_CDECL(void) quitModule(HINSTANCE instance) {
  Factories::Destroy();
  Pane::DestroyWindowClasses(instance);
  gLsModule.DestroyMessageHandler();
  UnregisterClass(L"LSnModuleMsgHandler", instance);
  gInstance = nullptr;
}


EXPORT_CDECL(VERSION) GetCoreVersion() {
  return sVersion;
}


EXPORT_CDECL(const IDisplays*) GetDisplays() {
  return (const IDisplays*)&gDisplays;
}
