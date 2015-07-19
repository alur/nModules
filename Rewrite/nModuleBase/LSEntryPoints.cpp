#include "nModule.hpp"

#include "../nShared/LiteStep.h"

#include "../nCoreApi/Core.h"

#include "../Headers/Macros.h"
#include "../Headers/Windows.h"

// All modules are expected to provide these globals.
extern NModule gModule;
extern LRESULT WINAPI MessageHandlerProc(HWND, UINT, WPARAM, LPARAM);


static void Cleanup() {
  gModule.log->Discard();
  gModule.log = nullptr;
  DestroyWindow(gModule.window);
  gModule.window = nullptr;
  gModule.instance = nullptr;
  nCore::Disconnect();
}


EXPORT_CDECL(void) quitModule(HINSTANCE /* instance */) {
  Cleanup();
}


EXPORT_CDECL(int) initModuleW(HWND /* parent */, HINSTANCE instance, LPCWSTR /* path */) {
  HRESULT hr = nCore::Connect(gModule.coreVersion);
  if (FAILED(hr)) {
    HandleCoreConnectionError(gModule.name, gModule.version, hr);
    return 1;
  }

  gModule.log = nCore::CreateLogger(gModule.name);
  gModule.instance = instance;

  hr = CreateMessageHandler(
    nCore::GetInstance(), gModule.name, MessageHandlerProc, gModule.window);
  if (FAILED(hr)) {
    gModule.log->ErrorHR(hr, L"Failed to create message handler!");
    Cleanup();
    return 1;
  }

  return 0;
}
