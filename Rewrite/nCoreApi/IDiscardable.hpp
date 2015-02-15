#pragma once

#include "ApiDefs.h"

class IDiscardable {
public:
  /// <summary>
  /// Discards this object, freeing and memory allocated to it.
  /// </summary>
  virtual void APICALL Discard() = 0;
};
