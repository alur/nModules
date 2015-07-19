#pragma once

#include "ApiDefs.h"

#include "../Headers/Windows.h"

typedef LONG_PTR NPARAM;

/// <summary>
/// A class which can process window messages for a pane.
/// </summary>
class IMessageHandler {
public:
  /// <summary>
  /// Handles a window message sent to this pane.
  /// </summary>
  /// <param name="window">Handle to the window the message was sent to.</param>
  /// <param name="message">The message type.</param>
  /// <param name="wParam">Message dependent data.</param>
  /// <param name="lParam">Message dependent data.</param>
  /// <param name="nParam">Message dependent data.</param>
  /// <remarks>
  /// We pass around an extra param, the NPARAM, in order to allow forwarding a message in its
  /// original form with some extra data.
  /// </remarks>
  virtual LRESULT APICALL HandleMessage(
    HWND window, UINT message, WPARAM wParam, LPARAM lParam, NPARAM nParam) = 0;
};
