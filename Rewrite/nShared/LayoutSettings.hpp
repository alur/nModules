#pragma once

#include "../nCoreApi/ISettingsReader.hpp"
#include "../nCoreApi/Lengths.h"

class LayoutSettings {
public:
  // Possible start positions.
  enum class StartPosition {
    TopLeft,
    TopRight,
    BottomLeft,
    BottomRight
  };

  // Which direction to go first from the start.
  enum class Direction {
    Horizontal,
    Vertical
  };

public:
  LayoutSettings();

public:
  // Loads the actual settings.
  void Load(const ISettingsReader *reader);

public:
  // Padding around the items. Default: 0, 0, 0, 0
  NRECT mPadding;

  // Horizontal spacing between the items. Default: 2
  NLENGTH mColumnSpacing;

  // Vertical spacing between the items. Default: 2
  NLENGTH mRowSpacing;

  // Where to start adding items. Default: TopLeft
  StartPosition mStartPosition;

  // Direction to go first from the starting position. Default: Horizontal
  Direction mPrimaryDirection;
};
