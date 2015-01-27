static void __cdecl UnitializedCoreFunc() {
  throw "Call to an uninitialized core function!";
}

#define CORE_API_PROC(x, y, ...) \
  x (__cdecl *y)(__VA_ARGS__) = (x (__cdecl*)(__VA_ARGS__))UnitializedCoreFunc
#include "Core.h"

#include "../nUtilities/hresults.h"

#define CORE_PROC_ITEM(name, version) { (void**)&nCore::name, #name, version }
static const struct CoreProcItem {
  void **proc;
  LPCSTR name;
  VERSION minVersion;
} sCoreProcTable[] = {
  CORE_PROC_ITEM(CreateEventHandler,        MakeVersion(0, 9, 0, 0)),
  CORE_PROC_ITEM(CreatePane,                MakeVersion(0, 9, 0, 0)),
  CORE_PROC_ITEM(CreateSettingsReader,      MakeVersion(0, 9, 0, 0)),
  CORE_PROC_ITEM(CreateStatePainter,        MakeVersion(0, 9, 0, 0)),
  CORE_PROC_ITEM(EnumRCLines,               MakeVersion(0, 9, 0, 0)),
  CORE_PROC_ITEM(EnumRCLineTokens,          MakeVersion(0, 9, 0, 0)),
  CORE_PROC_ITEM(FindPane,                  MakeVersion(0, 9, 0, 0)),
  CORE_PROC_ITEM(GetCoreVersion,            MakeVersion(0, 9, 0, 0)),
  CORE_PROC_ITEM(GetDisplays,               MakeVersion(0, 9, 0, 0)),
  CORE_PROC_ITEM(GetDWriteFactory,          MakeVersion(0, 9, 0, 0)),
  CORE_PROC_ITEM(GetD2DFactory,             MakeVersion(0, 9, 0, 0)),
  CORE_PROC_ITEM(GetWICFactory,             MakeVersion(0, 9, 0, 0))
};

static HINSTANCE sCoreInstance = nullptr;


HRESULT nCore::Connect(VERSION minVersion) {
  DWORD coreProcessId = 0, moduleProcessId = GetCurrentProcessId();
  HWND coreWindow = nullptr;

  do {
    // Looks for LSnCore within the same process
    coreWindow = FindWindowEx(nullptr, coreWindow, L"LSnModuleMsgHandler", L"nCore");
    if (coreWindow != nullptr) {
      GetWindowThreadProcessId(coreWindow, &coreProcessId);
    }
  } while (coreWindow != nullptr && coreProcessId != moduleProcessId);

  if (coreWindow == nullptr) {
    return E_API_CORE_NOT_FOUND;
  }

  sCoreInstance = (HINSTANCE)GetWindowLongPtr(coreWindow, GWLP_HINSTANCE);
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
  for (auto &item : sCoreProcTable) {
    *item.proc = UnitializedCoreFunc;
  }
}


HINSTANCE nCore::GetInstance() {
  return sCoreInstance;
}
