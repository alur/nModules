/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  LayoutSettings.cpp
 *  The nModules Project
 *
 *  Settings used for laying out items in a rectangle.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

#include "LayoutSettings.hpp"


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


/// <summary>
/// Destructor.
/// </summary>
LayoutSettings::~LayoutSettings() {
}


/// <summary>
/// Loads settings from an RC file using the specified defaults.
/// </summary>
void LayoutSettings::Load(Settings* settings, LayoutSettings* defaults) {
    char buffer[32];

    mColumnSpacing = settings->GetInt("ColumnSpacing", defaults->mColumnSpacing);
    settings->GetOffsetRect("PaddingLeft", "PaddingTop", "PaddingRight", "PaddingBottom", &mPadding, &defaults->mPadding);

    settings->GetString("Start", buffer, sizeof(buffer), "TopLeft");
    if (_stricmp(buffer, "TopRight") == 0) {
        mStartPosition = StartPosition::TopRight;
    }
    else if (_stricmp(buffer, "BottomLeft") == 0) {
        mStartPosition = StartPosition::BottomLeft;
    }
    else if (_stricmp(buffer, "BottomRight") == 0) {
        mStartPosition = StartPosition::BottomRight;
    }
    else {
        mStartPosition = StartPosition::TopLeft;
    }

    this->mRowSpacing = settings->GetInt("RowSpacing", defaults->mRowSpacing);

    settings->GetString("PrimaryDirection", buffer, sizeof(buffer), "Horizontal");
    if (_stricmp(buffer, "Vertical") == 0) {
        mPrimaryDirection = Direction::Vertical;
    }
    else {
        mPrimaryDirection = Direction::Horizontal;
    }
}


/// <summary>
/// Calculates the positioning of an item based on its position ID.
/// </summary>
RECT LayoutSettings::RectFromID(int id, int itemWidth, int itemHeight, int containerWidth, int containerHeight) {
    RECT rect = {0};
    int row = 0, column = 0;
    
    // The required space to fit n items in a row is n*itemWidth + (n-1)*columnSpacing
    // Thus, the number of items you can fit in a row is (width + columnSpacing)/(itemWidth + columnSpacing)
    switch (mPrimaryDirection) {
    case Direction::Vertical:
        {
            int itemsPerColumn = max(1, (containerHeight - mPadding.top - mPadding.bottom + mRowSpacing)/(itemHeight + mRowSpacing));
            column = id / itemsPerColumn;
            row = id % itemsPerColumn;
        }
        break;

    case Direction::Horizontal:
        {
            int itemsPerRow = max(1, (containerWidth - mPadding.left - mPadding.right + mColumnSpacing)/(itemWidth + mColumnSpacing));
            row = id / itemsPerRow;
            column = id % itemsPerRow;
        }
        break;
    }

    switch (mStartPosition) {
    case StartPosition::BottomLeft:
        {
            rect.left = mPadding.left + column * (itemWidth + mColumnSpacing);
            rect.right = rect.left + itemWidth;

            rect.bottom = containerHeight - mPadding.bottom - row * (itemHeight + mRowSpacing);
            rect.top = rect.bottom - itemHeight;
        }
        break;

    case StartPosition::TopLeft:
        {
            rect.left = mPadding.left + column * (itemWidth + mColumnSpacing);
            rect.right = rect.left + itemWidth;

            rect.top = mPadding.top + row * (itemHeight + mRowSpacing);
            rect.bottom = rect.top + itemHeight;
        }
        break;

    case StartPosition::BottomRight:
        {
            rect.right = containerWidth - mPadding.right - column * (itemWidth + mColumnSpacing);
            rect.left = rect.right - itemWidth;

            rect.bottom = containerHeight - mPadding.bottom - row * (itemHeight + mRowSpacing);
            rect.top = rect.bottom - itemHeight;
        }
        break;

    case StartPosition::TopRight:
        {
            rect.right = containerWidth - mPadding.right - column * (itemWidth + mColumnSpacing);
            rect.left = rect.right - itemWidth;

            rect.top = mPadding.top + row * (itemHeight + mRowSpacing);
            rect.bottom = rect.top + itemHeight;
        }
        break;
    }

    return rect;
}
