#pragma once

#include "ApiDefs.h"
#include "IDiscardable.hpp"
#include "IEventProcessor.hpp"

#include "../Headers/Windows.h"

class IEventHandler : public IDiscardable {
public:
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
