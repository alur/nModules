#pragma once

#include "ApiDefs.h"

#include "../Headers/d2d1.h"

/// <summary>
/// A monitor.
/// </summary>
struct Display {
  RECT rect;
  RECT workArea;
  HMONITOR monitor;
  D2D1_POINT_2F dpi;
  int width;
  int height;
  int workAreaWidth;
  int workAreaHeight;
};

/// <summary>
/// Contains information about the current display configuration.
/// </summary>
class IDisplays {
public:
  /// <summary>
  /// Returns the ID of the display containing the largest fraction of the given window.
  /// </summary>
  /// <param name="hwnd">Handle to the window.</param>
  virtual UINT APICALL DisplayFromHWND(HWND hwnd) const = 0;

  /// <summary>
  /// Returns the ID of the display containing the largest fraction of the given rectangle.
  /// </summary>
  /// <param name="rect">The rectangle.</param>
  virtual UINT APICALL DisplayFromRECT(RECT rect) const = 0;

  /// <summary>
  /// Returns the number of displays.
  /// </summary>
  virtual UINT APICALL Count() const = 0;

  /// <summary>
  /// Returns the virtual desktop, which covers all other displays.
  /// </summary>
  virtual const Display& APICALL GetDesktop() const = 0;

  /// <summary>
  /// Returns the display with the given ID.
  /// </summary>
  /// <param name="id">The ID of the display to return.</param>
  virtual const Display& APICALL GetDisplay(UINT id) const = 0;
};
