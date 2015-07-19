#pragma once

#include "../Headers/Windows.h"

#include <Shobjidl.h>

/// <summary>
///
/// </summary>
struct TaskWindow {
  UINT monitor;
  HICON icon;
  HICON overlayIcon;
  TBPFLAG progressState;
  USHORT progress;
};
