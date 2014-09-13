/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  LayoutSettings.hpp
 *  The nModules Project
 *
 *  Settings used for laying out items in a rectangle.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

#include "Settings.hpp"

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
  explicit LayoutSettings(void init(LayoutSettings&));

public:
  // Loads the actual settings.
  void Load(Settings *settings, const LayoutSettings *defaults = nullptr);
    
  // Calculates the number of items that can fit in a column
  int ItemsPerColumn(int itemHeight, int containerHeight);

  // Calculates the number of items that can fit in a row
  int ItemsPerRow(int itemWidth, int containerWidth);

  // Calculates the positioning of an item based on its position ID.
  int ItemLimit(int itemWidth, int itemHeight, int containerWidth, int containerHeight);

  // Calculates the positioning of an item based on its position ID.
  RECT RectFromID(int id, int itemWidth, int itemHeight, int containerWidth, int containerHeight);

  // Returns the length needed to contain the specified number of items.
  int LengthFromNumberOfItems(int numItems, int itemWidth, int itemHeight);

public:
  // Padding around the items. Default: 0, 0, 0, 0
  RECT mPadding;

  // Horizontal spacing between the items. Default: 2
  int mColumnSpacing;

  // Vertical spacing between the items. Default: 2
  int mRowSpacing;

  // Where to start adding items. Default: TopLeft
  StartPosition mStartPosition;

  // Direction to go first from the starting position. Default: Horizontal
  Direction mPrimaryDirection;
};
