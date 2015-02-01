#pragma once

#include "../nCoreApi/IDisplays.hpp"
#include "../nCoreApi/IEventHandler.hpp"
#include "../nCoreApi/ILogger.hpp"
#include "../nCoreApi/IStatePainter.hpp"
#include "../nCoreApi/ApiDefs.h"

#include "../nUtilities/d2d1.h"
#include "../nUtilities/Macros.h"
#include "../nUtilities/Version.h"

EXPORT_CDECL(IEventHandler*) CreateEventHandler(const ISettingsReader *reader);
EXPORT_CDECL(ILogger*) CreateLogger(LPCWSTR name);
EXPORT_CDECL(IPane*) CreatePane(const PaneInitData *initData);
EXPORT_CDECL(HRESULT) CreateSettingsReader(LPCWSTR prefix, ISettingsReader **reader);
EXPORT_CDECL(IStatePainter*) CreateStatePainter(const StatePainterInitData *initData);
EXPORT_CDECL(void) EnumRCLines(LPCWSTR key, void(APICALL *callback)(LPCWSTR line, LPARAM data),
  LPARAM data);
EXPORT_CDECL(void) EnumRCLineTokens(LPCWSTR key,
  void (APICALL *callback)(LPCWSTR token, LPARAM data), LPARAM data);
EXPORT_CDECL(IPane*) FindPane(LPCWSTR name);
EXPORT_CDECL(VERSION) GetCoreVersion();
EXPORT_CDECL(ID2D1Factory*) GetD2DFactory();
EXPORT_CDECL(const IDisplays*) GetDisplays();
EXPORT_CDECL(IDWriteFactory*) GetDWriteFactory();
EXPORT_CDECL(IWICImagingFactory*) GetWICFactory();
EXPORT_CDECL(HICON) GetWindowIcon(HWND window, UINT32 size);
EXPORT_CDECL(bool) IsTaskbarWindow(HWND window);
EXPORT_CDECL(bool) ParseLength(LPCWSTR string, NLENGTH *out);
EXPORT_CDECL(bool) ParseMonitor(LPCWSTR string, LPUINT out);
EXPORT_CDECL(void) RegisterForMessages(HWND window, const UINT messages[]);
EXPORT_CDECL(void) UnregisterForMessages(HWND window, const UINT messages[]);
