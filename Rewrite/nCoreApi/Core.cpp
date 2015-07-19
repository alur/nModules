#include <assert.h>

static void __cdecl UnitializedCoreFunc() {
  assert(false);
  throw "Call to an uninitialized core function!";
}

#define CORE_API_PROC(x, y, ...) \
  x (__cdecl *y)(__VA_ARGS__) = (x (__cdecl*)(__VA_ARGS__))UnitializedCoreFunc
#include "Core.h"

#include "../Headers/hresults.h"

#define CORE_PROC_ITEM(name, version) { (void**)&nCore::name, #name, version }
static const struct CoreProcItem {
  void **proc;
  LPCSTR name;
  VERSION minVersion;
} sCoreProcTable[] = {
  CORE_PROC_ITEM(ClearInterval,             MakeVersion(1, 0, 0, 0)),
  CORE_PROC_ITEM(CreateBackgroundPainter,   MakeVersion(1, 0, 0, 0)),
  CORE_PROC_ITEM(CreateEventHandler,        MakeVersion(1, 0, 0, 0)),
  CORE_PROC_ITEM(CreateImagePainter,        MakeVersion(1, 0, 0, 0)),
  CORE_PROC_ITEM(CreateLogger,              MakeVersion(1, 0, 0, 0)),
  CORE_PROC_ITEM(CreatePane,                MakeVersion(1, 0, 0, 0)),
  CORE_PROC_ITEM(CreateSettingsReader,      MakeVersion(1, 0, 0, 0)),
  CORE_PROC_ITEM(CreateTextPainter,         MakeVersion(1, 0, 0, 0)),
  //CORE_PROC_ITEM(DataChanged,               MakeVersion(1, 0, 0, 0)),
  CORE_PROC_ITEM(EnumRCLines,               MakeVersion(1, 0, 0, 0)),
  CORE_PROC_ITEM(EnumRCLineTokens,          MakeVersion(1, 0, 0, 0)),
  CORE_PROC_ITEM(FindPane,                  MakeVersion(1, 0, 0, 0)),
  CORE_PROC_ITEM(GetChildPainter,           MakeVersion(1, 0, 0, 0)),
  CORE_PROC_ITEM(GetCoreVersion,            MakeVersion(1, 0, 0, 0)),
  CORE_PROC_ITEM(GetD2DFactory,             MakeVersion(1, 0, 0, 0)),
  CORE_PROC_ITEM(GetDisplays,               MakeVersion(1, 0, 0, 0)),
  CORE_PROC_ITEM(GetDWriteFactory,          MakeVersion(1, 0, 0, 0)),
  CORE_PROC_ITEM(GetWICFactory,             MakeVersion(1, 0, 0, 0)),
  CORE_PROC_ITEM(GetWindowIcon,             MakeVersion(1, 0, 0, 0)),
  CORE_PROC_ITEM(IsTaskbarWindow,           MakeVersion(1, 0, 0, 0)),
  CORE_PROC_ITEM(ParseLength,               MakeVersion(1, 0, 0, 0)),
  CORE_PROC_ITEM(ParseMonitor,              MakeVersion(1, 0, 0, 0)),
  CORE_PROC_ITEM(RegisterDataProvider,      MakeVersion(1, 0, 0, 0)),
  CORE_PROC_ITEM(RegisterForMessages,       MakeVersion(1, 0, 0, 0)),
  CORE_PROC_ITEM(SetInterval,               MakeVersion(1, 0, 0, 0)),
  CORE_PROC_ITEM(UnregisterDataProvider,    MakeVersion(1, 0, 0, 0)),
  CORE_PROC_ITEM(UnregisterForMessages,     MakeVersion(1, 0, 0, 0))
};

static HINSTANCE sCoreInstance = nullptr;
static HWND sCoreWindow = nullptr;


HRESULT nCore::Connect(VERSION minVersion) {
  DWORD coreProcessId = 0, moduleProcessId = GetCurrentProcessId();

  do {
    // Looks for LSnCore within the same process
    sCoreWindow = FindWindowEx(nullptr, sCoreWindow, L"LSnModuleMsgHandler", L"nCore");
    if (sCoreWindow != nullptr) {
      GetWindowThreadProcessId(sCoreWindow, &coreProcessId);
    }
  } while (sCoreWindow != nullptr && coreProcessId != moduleProcessId);

  if (sCoreWindow == nullptr) {
    return E_API_CORE_NOT_FOUND;
  }

  sCoreInstance = (HINSTANCE)GetWindowLongPtr(sCoreWindow, GWLP_HINSTANCE);
  if (sCoreInstance == nullptr) {
    return E_FAIL;
  }

  for (auto &item : sCoreProcTable) {
    if (item.minVersion <= minVersion) {
      *item.proc = GetProcAddress(sCoreInstance, item.name);
      if (*item.proc == nullptr) {
        Disconnect();
        return E_API_FUNC_NOT_FOUND;
      }
    }
  }

  return S_OK;
}


void nCore::Disconnect() {
  sCoreInstance = nullptr;
  sCoreWindow = nullptr;
  for (auto &item : sCoreProcTable) {
    *item.proc = UnitializedCoreFunc;
  }
}


HINSTANCE nCore::GetInstance() {
  return sCoreInstance;
}


HWND nCore::GetWindow() {
  return sCoreWindow;
}
