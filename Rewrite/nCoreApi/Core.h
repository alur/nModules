#pragma once

#include "IDataProvider.hpp"
#include "IDiscardablePainter.hpp"
#include "IDisplays.hpp"
#include "IEventHandler.hpp"
#include "IEventProcessor.hpp"
#include "IImagePainter.hpp"
#include "ILogger.hpp"
#include "IPane.hpp"
#include "ISettingsReader.hpp"
#include "IStringMap.hpp"
#include "TaskWindow.h"
#include "Version.h"

#include "../Headers/Windows.h"

#ifndef CORE_API_PROC
#define CORE_API_PROC(x, y, ...) extern x (__cdecl *y)(__VA_ARGS__)
#endif

namespace nCore {
  /// <summary>
  /// Clears a timer.
  /// </summary>
  CORE_API_PROC(void, ClearInterval, UINT_PTR timer);

  /// <summary>
  /// Connects to the core.
  /// </summary>
  /// <param name="minVersion">The minimum supported version of nCore.</param>
  HRESULT Connect(VERSION minVersion);

  /// <summary>
  /// Creates a background painter.
  /// </summary>
  CORE_API_PROC(IDiscardablePainter*, CreateBackgroundPainter, const ISettingsReader*,
    const StateDefinition *states, BYTE numStates);

  /// <summary>
  /// Creates a new EventHandler. The caller must eventually call ->Destroy().
  /// </summary>
  /// <param name="reader">The settings reader from which to get event actions.</param>
  CORE_API_PROC(IEventHandler*, CreateEventHandler, const ISettingsReader *reader);

  /// <summary>
  /// Creates an IImagePainter.
  /// </summary>
  CORE_API_PROC(IImagePainter*, CreateImagePainter);

  /// <summary>
  /// Creates a new logger.
  /// </summary>
  /// <param name="name">
  /// Name to identify the source of the log message, generally the module name.
  /// </param>
  CORE_API_PROC(ILogger*, CreateLogger, LPCWSTR name);

  /// <summary>
  /// Creates a new IPane. The caller must eventually call ->Destroy().
  /// </summary>
  /// <param name="initData">Initialization data for the pane.</param>
  CORE_API_PROC(IPane*, CreatePane, const PaneInitData *initData);

  /// <summary>
  /// Creates a settings reader for the given prefix.
  /// </summary>
  /// <param name="prefix">The prefix to use when reading settings.</param>
  /// <param name="defaults">Default values to use, may be null.</param>
  CORE_API_PROC(ISettingsReader*, CreateSettingsReader, LPCWSTR prefix,
    const IStringMap *defaults);

  /// <summary>
  /// Creates a text painter.
  /// </summary>
  CORE_API_PROC(IDiscardablePainter*, CreateTextPainter, const ISettingsReader*,
    const StateDefinition *states, BYTE numStates);

  /// <summary>
  /// Notifies the core that some number of data sources have changed.
  /// </summary>
  //void DataChanged(int num, LPCWSTR name, ...);

  /// <summary>
  /// Disconnects from the core.
  /// </summary>
  void Disconnect();

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
  CORE_API_PROC(void, EnumRCLineTokens, LPCWSTR key,
    void (APICALL *callback)(LPCWSTR token, LPARAM data), LPARAM data);

  /// <summary>
  /// Finds the pane with the given name.
  /// </summary>
  /// <param name="name">The name of the pane to find.</param>
  CORE_API_PROC(IPane*, FindPane, LPCWSTR name);

  /// <summary>
  /// Returns a pointer to the child painter singleton. This painter simply paints the children of
  /// a pane.
  /// </summary>
  CORE_API_PROC(IPainter*, GetChildPainter);

  /// <summary>
  /// Returns the version of the core that is running.
  /// </summary>
  CORE_API_PROC(VERSION, GetCoreVersion);

  /// <summary>
  /// Returns the global Direct2D factory.
  /// </summary>
  CORE_API_PROC(ID2D1Factory*, GetD2DFactory);

  /// <summary>
  /// Returns a pointer to the global IDisplays object.
  /// </summary>
  CORE_API_PROC(const IDisplays*, GetDisplays);

  /// <summary>
  /// Retrieves the core's instance.
  /// </summary>
  HINSTANCE GetInstance();

  /// <summary>
  /// Returns the global DirectWrite factory.
  /// </summary>
  CORE_API_PROC(IDWriteFactory*, GetDWriteFactory);

  /// <summary>
  /// Returns the global WIC factory.
  /// </summary>
  CORE_API_PROC(IWICImagingFactory*, GetWICFactory);

  /// <summary>
  /// Retrieves the core's message window.
  /// </summary>
  HWND GetWindow();

  /// <summary>
  /// Returns the HICON for the given taskbar Window.
  /// </summary>
  /// <param name="window">The window to get the icon of.</param>
  /// <param name="size">The target size of icon to get.</param>
  /// <param>Returns the icon, or nullptr.</param>
  CORE_API_PROC(HICON, GetWindowIcon, HWND window, UINT32 size);

  /// <summary>
  /// Returns true if the given window should be shown in the taskbar.
  /// </summary>
  CORE_API_PROC(bool, IsTaskbarWindow, HWND window);

  /// <summary>
  /// Parses an NLENGTH from a string.
  /// </summary>
  /// <param name="string">The string to parse.</param>
  /// <param name="out">Pointer to an NLENGTH which the parse result will be written to.</param>
  /// <returns>True if the string is a valid NLENGTH.</returns>
  /// <remarks>Out will be untouched if this functionr returns false.</remarks>
  CORE_API_PROC(bool, ParseLength, LPCWSTR string, NLENGTH *out);

  /// <summary>
  /// Parses a string into a monitor ID.
  /// </summary>
  /// <param name="string">The string to parse.</param>
  /// <param name="out">Pointer to an UINT which the parse result will be written to.</param>
  /// <returns>True if the string is a valid monitor.</returns>
  /// <remarks>Out will be untouched if this functionr returns false.</remarks>
  CORE_API_PROC(bool, ParseMonitor, LPCWSTR string, LPUINT out);

  /// <summary>
  /// Registers a data provider.
  /// </summary>
  CORE_API_PROC(bool, RegisterDataProvider, LPCWSTR name, IDataProvider *provider);

  /// <summary>
  /// Registers a window as a handler for the specified messages.
  /// </summary>
  CORE_API_PROC(void, RegisterForMessages, HWND window, const UINT messages[]);

  /// <summary>
  /// Sets a timer.
  /// </summary>
  CORE_API_PROC(UINT_PTR, SetInterval, UINT delay, IMessageHandler *handler);

  /// <summary>
  /// Unregisters a data provider.
  /// </summary>
  CORE_API_PROC(void, UnregisterDataProvider, LPCWSTR name);

  /// <summary>
  /// Unregisters a window as a handler for the specified messages.
  /// </summary>
  CORE_API_PROC(void, UnregisterForMessages, HWND window, const UINT messages[]);
}
