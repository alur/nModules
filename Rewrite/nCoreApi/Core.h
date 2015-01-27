#pragma once

#include "IDisplays.hpp"
#include "IEventHandler.hpp"
#include "IEventProcessor.hpp"
#include "IPane.hpp"
#include "ISettingsReader.hpp"
#include "IStatePainter.hpp"

#include "../nUtilities/Version.h"
#include "../nUtilities/Windows.h"

#ifndef CORE_API_PROC
#define CORE_API_PROC(x, y, ...) extern x (__cdecl *y)(__VA_ARGS__)
#endif

namespace nCore {
  /// <summary>
  /// Connects to the core.
  /// </summary>
  /// <param name="context"></param>
  HRESULT Connect(VERSION minVersion);

  /// <summary>
  /// Disconnects from the core.
  /// </summary>
  void Disconnect();

  /// <summary>
  /// Retrieves the core's instance.
  /// </summary>
  HINSTANCE GetInstance();

  /// <summary>
  /// Creates a new EventHandler. The caller must eventually call ->Destroy().
  /// </summary>
  /// <param name="reader"></param>
  /// <param name="processor"></param>
  CORE_API_PROC(IEventHandler*, CreateEventHandler, const ISettingsReader *reader);

  /// <summary>
  /// Creates a new IPane. The caller must eventually call ->Destroy().
  /// </summary>
  /// <param name="initData">Initialization data for the pane.</param>
  CORE_API_PROC(IPane*, CreatePane, const PaneInitData *initData);

  /// <summary>
  /// Creates a settings reader.
  /// </summary>
  /// <param name="prefix">The prefix to use when reading settings.</param>
  /// <param name="reader">out</param>
  CORE_API_PROC(HRESULT, CreateSettingsReader, LPCWSTR prefix, ISettingsReader **reader);

  /// <summary>
  /// Creates a new EventHandler. The caller must eventually call ->Destroy().
  /// </summary>
  /// <param name="reader"></param>
  CORE_API_PROC(IStatePainter*, CreateStatePainter, const StatePainterInitData *initData,
    const ISettingsReader *reader);

  /// <summary>
  /// Enumerates all lines with the given key.
  /// </summary>
  /// <param name="key">The key of the lines to enumerate.</param>
  /// <param name="callback">Callback function to call for each line.</param>
  CORE_API_PROC(void, EnumRCLines, LPCWSTR key, void(APICALL *callback)(LPCWSTR line, LPARAM data),
    LPARAM data);

  /// <summary>
  /// Enumerates all the tokens on every line with the given key.
  /// </summary>
  /// <param name="key">The key of the lines to enumerate.</param>
  /// <param name="callback">Callback function to call for each token.</param>
  CORE_API_PROC(void, EnumRCLineTokens, LPCWSTR key, void (APICALL *callback)(LPCWSTR token, LPARAM data),
    LPARAM data);

  /// <summary>
  /// Finds the pane with the given name.
  /// </summary>
  /// <param name="name">The name of the pane to find.</param>
  CORE_API_PROC(IPane*, FindPane, LPCWSTR name);

  /// <summary>
  /// Returns the version of the core that is running.
  /// </summary>
  CORE_API_PROC(VERSION, GetCoreVersion);

  /// <summary>
  /// Returns a pointer to the global IDisplays object.
  /// </summary>
  CORE_API_PROC(const IDisplays*, GetDisplays);

  /// <summary>
  /// Returns the global DirectWrite factory.
  /// </summary>
  CORE_API_PROC(IDWriteFactory*, GetDWriteFactory);

  /// <summary>
  /// Returns the global Direct2D factory.
  /// </summary>
  CORE_API_PROC(ID2D1Factory*, GetD2DFactory);

  /// <summary>
  /// Returns the global WIC factory.
  /// </summary>
  CORE_API_PROC(IWICImagingFactory*, GetWICFactory);
}
