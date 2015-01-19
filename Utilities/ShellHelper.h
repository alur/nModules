//-------------------------------------------------------------------------------------------------
// /Utilities/ShellHelper.h
// The nModules Project
//
// Provides wrappers around functions which are not available on all systems.
//-------------------------------------------------------------------------------------------------
#pragma once

#include "Common.h"

namespace ShellHelper {
  enum MONITOR_DPI_TYPE {
    MDT_Effective_DPI = 0,
    MDT_Angular_DPI = 1,
    MDT_Raw_DPI = 2,
    MDT_Default = MDT_Effective_DPI
  };

  HRESULT WINAPI GetDPIForMonitor(HMONITOR, MONITOR_DPI_TYPE, LPUINT dpiX, LPUINT dpiY);
}
