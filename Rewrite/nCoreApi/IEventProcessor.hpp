#pragma once

#include "ApiDefs.h"

#include "../Headers/Windows.h"

class IEventProcessor {
public:
  /// <summary>
  /// Called by an IEventHandler before it's about to trigger an action.
  /// </summary>
  /// <param name="action">The user-defined action about to be triggered.</param>
  /// <returns>
  /// False if the IEventHandler should execute the action, true if the processor handled it.
  /// </returns>
  virtual bool APICALL ProcessEvent(LPCWSTR action) = 0;
};
