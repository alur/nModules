/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  LayoutSettings.cpp
 *  The nModules Project
 *
 *  Settings used for laying out items in a rectangle.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

#include "LayoutSettings.hpp"
#include <algorithm>

using std::max;
using std::min;


/// <summary>
/// Initalizes the class to all default settings.
/// </summary>
LayoutSettings::LayoutSettings() {
  mColumnSpacing = 2;
  mPadding.left = 0;
  mPadding.top = 0;
  mPadding.right = 0;
  mPadding.bottom = 0;
  mPrimaryDirection = Direction::Horizontal;
  mRowSpacing = 2;
  mStartPosition = StartPosition::TopLeft;
}


LayoutSettings::LayoutSettings(void init(LayoutSettings&)) : LayoutSettings() {
  init(*this);
}


/// <summary>
/// Loads settings from an RC file using the specified defaults.
/// </summary>
void LayoutSettings::Load(Settings *settings, const LayoutSettings *defaults) {
  TCHAR buffer[32];
  if (defaults == nullptr) {
    defaults = this;
  }

  mColumnSpacing = settings->GetInt(L"ColumnSpacing", defaults->mColumnSpacing);
  mPadding = settings->GetOffsetRect(L"Padding", &defaults->mPadding);

  settings->GetString(L"Start", buffer, sizeof(buffer), L"TopLeft");
  if (_wcsicmp(buffer, L"TopRight") == 0) {
    mStartPosition = StartPosition::TopRight;
  } else if (_wcsicmp(buffer, L"BottomLeft") == 0) {
    mStartPosition = StartPosition::BottomLeft;
  } else if (_wcsicmp(buffer, L"BottomRight") == 0) {
    mStartPosition = StartPosition::BottomRight;
  } else {
    mStartPosition = StartPosition::TopLeft;
  }

  this->mRowSpacing = settings->GetInt(L"RowSpacing", defaults->mRowSpacing);

  settings->GetString(L"PrimaryDirection", buffer, sizeof(buffer), L"Horizontal");
  if (_wcsicmp(buffer, L"Vertical") == 0) {
    mPrimaryDirection = Direction::Vertical;
  } else {
    mPrimaryDirection = Direction::Horizontal;
  }
}


/// <summary>
/// Calculates the number of items that can fit in a column
/// </summary>
int LayoutSettings::ItemsPerColumn(int itemHeight, int containerHeight) {
  return max(1L, (containerHeight - mPadding.top - mPadding.bottom + mRowSpacing)/(itemHeight + mRowSpacing));
}


/// <summary>
/// Calculates the number of items that can fit in a row
/// </summary>
int LayoutSettings::ItemsPerRow(int itemWidth, int containerWidth) {
  return max(1L, (containerWidth - mPadding.left - mPadding.right + mColumnSpacing)/(itemWidth + mColumnSpacing));
}


/// <summary>
/// Calculates how many items can fit in a container of the given size.
/// </summary>
int LayoutSettings::ItemLimit(int itemWidth, int itemHeight, int containerWidth, int containerHeight) {
  return ItemsPerColumn(itemHeight, containerHeight) * ItemsPerRow(itemWidth, containerWidth);
}


/// <summary>
/// Calculates the positioning of an item based on its position ID.
/// </summary>
RECT LayoutSettings::RectFromID(int id, int itemWidth, int itemHeight, int containerWidth, int containerHeight) {
  RECT rect = { 0 };
  int row = 0, column = 0;
    
  // The required space to fit n items in a row is n*itemWidth + (n - 1)*columnSpacing
  // Thus, the number of items you can fit in a row is
  // (width + columnSpacing)/(itemWidth + columnSpacing)
  switch (mPrimaryDirection) {
  case Direction::Vertical: {
      int itemsPerColumn = ItemsPerColumn(itemHeight, containerHeight);
      column = id / itemsPerColumn;
      row = id % itemsPerColumn;
    }
    break;

  case Direction::Horizontal: {
      int itemsPerRow = ItemsPerRow(itemWidth, containerWidth);
      row = id / itemsPerRow;
      column = id % itemsPerRow;
    }
    break;
  }

  switch (mStartPosition) {
  case StartPosition::BottomLeft:
    rect.left = mPadding.left + column * (itemWidth + mColumnSpacing);
    rect.right = rect.left + itemWidth;
    rect.bottom = containerHeight - mPadding.bottom - row * (itemHeight + mRowSpacing);
    rect.top = rect.bottom - itemHeight;
    break;

  case StartPosition::TopLeft:
    rect.left = mPadding.left + column * (itemWidth + mColumnSpacing);
    rect.right = rect.left + itemWidth;
    rect.top = mPadding.top + row * (itemHeight + mRowSpacing);
    rect.bottom = rect.top + itemHeight;
    break;

  case StartPosition::BottomRight:
    rect.right = containerWidth - mPadding.right - column * (itemWidth + mColumnSpacing);
    rect.left = rect.right - itemWidth;
    rect.bottom = containerHeight - mPadding.bottom - row * (itemHeight + mRowSpacing);
    rect.top = rect.bottom - itemHeight;
    break;

  case StartPosition::TopRight:
    rect.right = containerWidth - mPadding.right - column * (itemWidth + mColumnSpacing);
    rect.left = rect.right - itemWidth;
    rect.top = mPadding.top + row * (itemHeight + mRowSpacing);
    rect.bottom = rect.top + itemHeight;
    break;
  }

  return rect;
}
