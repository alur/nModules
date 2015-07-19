#pragma once

#include "../nCoreApi/ApiDefs.h"
#include "../nCoreApi/IDataProvider.hpp"
#include "../nCoreApi/IDisplays.hpp"
#include "../nCoreApi/IEventHandler.hpp"
#include "../nCoreApi/IImagePainter.hpp"
#include "../nCoreApi/ILogger.hpp"
#include "../nCoreApi/IStringMAp.hpp"
#include "../nCoreApi/Version.h"

#include "../Headers/d2d1.h"
#include "../Headers/Macros.h"

EXPORT_CDECL(void) ClearInterval(UINT_PTR timer);
EXPORT_CDECL(IDiscardablePainter*) CreateBackgroundPainter(const ISettingsReader *reader,
  const StateDefinition *states, BYTE numStates);
EXPORT_CDECL(IEventHandler*) CreateEventHandler(const ISettingsReader *reader);
EXPORT_CDECL(IImagePainter*) CreateImagePainter();
EXPORT_CDECL(ILogger*) CreateLogger(LPCWSTR name);
EXPORT_CDECL(IPane*) CreatePane(const PaneInitData *initData);
EXPORT_CDECL(ISettingsReader*) CreateSettingsReader(LPCWSTR prefix, const IStringMap *defaults);
EXPORT_CDECL(IDiscardablePainter*) CreateTextPainter(const ISettingsReader *reader,
  const StateDefinition *states, BYTE numStates);
//EXPORT_CDECL(void) DataChanged(int num, LPCWSTR name, ...);
EXPORT_CDECL(void) EnumRCLines(LPCWSTR key, void(APICALL *callback)(LPCWSTR line, LPARAM data),
  LPARAM data);
EXPORT_CDECL(void) EnumRCLineTokens(LPCWSTR key,
  void (APICALL *callback)(LPCWSTR token, LPARAM data), LPARAM data);
EXPORT_CDECL(IPane*) FindPane(LPCWSTR name);
EXPORT_CDECL(IPainter*) GetChildPainter();
EXPORT_CDECL(VERSION) GetCoreVersion();
EXPORT_CDECL(ID2D1Factory*) GetD2DFactory();
EXPORT_CDECL(const IDisplays*) GetDisplays();
EXPORT_CDECL(IDWriteFactory*) GetDWriteFactory();
EXPORT_CDECL(IWICImagingFactory*) GetWICFactory();
EXPORT_CDECL(HICON) GetWindowIcon(HWND window, UINT32 size);
EXPORT_CDECL(bool) IsTaskbarWindow(HWND window);
EXPORT_CDECL(bool) ParseLength(LPCWSTR string, NLENGTH *out);
EXPORT_CDECL(bool) ParseMonitor(LPCWSTR string, LPUINT out);
EXPORT_CDECL(bool) RegisterDataProvider(LPCWSTR name, IDataProvider *provider);
EXPORT_CDECL(void) RegisterForMessages(HWND window, const UINT messages[]);

EXPORT_CDECL(UINT_PTR) SetInterval(UINT delay, IMessageHandler *handler);
EXPORT_CDECL(void) UnregisterDataProvider(LPCWSTR name);
EXPORT_CDECL(void) UnregisterForMessages(HWND window, const UINT messages[]);
