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
        TopLeft = 0,
        TopRight,
        BottomLeft,
        BottomRight
    };

    // Which direction to go first from the start.
    enum class Direction {
        Horizontal = 0,
        Vertical
    };

    explicit LayoutSettings();
    virtual ~LayoutSettings();

    // Loads the actual settings.
    void Load(Settings* settings, LayoutSettings* defaults);

    // Padding around the items. Default: 0, 0, 0, 0
    RECT padding;

    // Horizontal spacing between the items. Default: 2
    int columnSpacing;

    // Vertical spacing between the items. Default: 2
    int rowSpacing;

    // Where to start adding items. Default: TopLeft
    StartPosition startPosition;

    // Direction to go first from the starting position. Default: Horizontal
    Direction primaryDirection;
};
