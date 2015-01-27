#pragma once

#include "../nCoreApi/IDisplays.hpp"

#include <vector>

class Displays : public IDisplays {
public:
  Displays();

  // IDisplays
public:
  UINT APICALL DisplayFromHWND(HWND hwnd) const override;
  UINT APICALL DisplayFromRECT(RECT rect) const override;
  UINT APICALL Count() const override;
  const Display& APICALL GetDisplay(UINT id) const override;
  const Display& APICALL GetDesktop() const override;

public:
  void Update();

private:
  /// <summary>
  /// Callback for EnumDisplayMonitors. Adds a display to the list of display.
  /// </summary>
  /// <param name="hMonitor">Handle to the monitor to add.</param>
  /// <param name="lParam">A pointer to the Displays class to add this monitor to.</param>
  static BOOL CALLBACK EnumMonitorsCallback(HMONITOR, HDC, LPRECT, LPARAM);

private:
  std::vector<Display> mDisplays;
  Display mDesktop;
};
