#pragma once

#include "../Headers/Windows.h"

#include <ShObjIdl.h>

struct TaskData {
  UINT monitor;
  TBPFLAG progressState;
  USHORT progress;
  bool flashing;
  bool minimized;
  HICON overlayIcon;
};
