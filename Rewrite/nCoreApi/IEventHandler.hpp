#pragma once

#include "ApiDefs.h"
#include "IEventProcessor.hpp"

#include "../nUtilities/Windows.h"

class IEventHandler {
public:
  /// <summary>
  /// Destroys the event handler.
  /// </summary>
  virtual void APICALL Destroy() = 0;

  /// <summary>
  /// Handles a window message.
  /// </summary>
  /// <param name="window">Handle to the window the message was sent to.</param>
  /// <param name="message">The message type.</param>
  /// <param name="wParam">Message dependent data.</param>
  /// <param name="lParam">Message dependent data.</param>
  /// <param name="processor"></param>
  virtual LRESULT APICALL HandleMessage(
    HWND window, UINT message, WPARAM wParam, LPARAM lParam, IEventProcessor *processor) = 0;
};
