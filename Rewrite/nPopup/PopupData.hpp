#pragma once

#include "../Headers/d2d1.h"

/// <summary>
/// All the settings required to
/// </summary>
class PopupData {
public:
  explicit PopupData(LPCWSTR prefix);

public:
  /// <summary>
  /// True if the popup should be confined to the monitor it was created on.
  /// </summary>
  bool ConfineToMonitor() const;

  /// <summary>
  /// True if the popup should be confined to the workarea of the monitor it's created on.
  /// </summary>
  bool ConfineToWorkArea() const;

  int GetMaxWidth() const;

  int GetItemSpacing() const;

  bool ExpandLeft() const;

  D2D1_SIZE_F GetChildOffset() const;

private:
  bool mConfineToMonitor;
  bool mConfineToWorkArea;
  int mMaxWidth;
  int mItemSpacing;
  bool mExpandLeft;
  D2D1_SIZE_F mChildOffset;
};
