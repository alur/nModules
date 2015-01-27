#pragma once

#include "ApiDefs.h"
#include "IMessageHandler.hpp"
#include "IPanePainter.hpp"
#include "ISettingsReader.hpp"

#include "../nUtilities/NRect.hpp"
#include "../nUtilities/Windows.h"

/// <summary>
///
/// </summary>
struct PaneInitData {
  size_t cbSize;
  LPCWSTR name;
  DWORD64 flags;
  IMessageHandler *messageHandler;
  class IPanePainter *painter;
  // The settings reader to use. This may be null, if you would like the pane not to read
  // configuration settings.
  const ISettingsReader *settingsReader;

  enum Flag : DWORD64 {
    // Set for the desktop window.
    DesktopWindow = 1,
  };
};

/// <summary>
/// A part of a window.
/// </summary>
class IPane : public IMessageHandler {
public:
  /// <summary>
  /// Creates a child of this pane.
  /// </summary>
  /// <param name="initData"></param>
  virtual IPane* APICALL CreateChild(const PaneInitData *initData) = 0;

  /// <summary>
  /// Destroys the pane.
  /// </summary>
  virtual void APICALL Destroy() = 0;

  /// <summary>
  /// Gets the current HWND of the window containing the pane. Note that this may change.
  /// </summary>
  virtual HWND APICALL GetWindow() const = 0;
};
