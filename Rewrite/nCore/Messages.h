#pragma once

#include "../nCoreApi/Messages.h"

#define NCORE_BROADCAST_LOW               0x9800
#define NCORE_BROADCAST_HIGH              0x9FFF

// nCore internal messages [0x0500-0x2000)

// Timers
enum {
  NCORE_TIMER_WINDOW_MAINTENANCE = 1,

  NCORE_LEASEABLE_TIMERS_START // Keep at the end
};
