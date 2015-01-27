#pragma once

#include "ApiDefs.h"

class IEventProcessor {
public:
  virtual bool APICALL ProcessEvent() = 0;
};
