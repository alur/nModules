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
    enum STARTPOSITION {
        TOPLEFT = 0,
        TOPRIGHT,
        BOTTOMLEFT,
        BOTTOMRIGHT
    };

    // Which direction to go first from the start.
    enum DIRECTION {
        HORIZONTAL = 0,
        VERTICAL
    };

    explicit LayoutSettings();
    virtual ~LayoutSettings();

    // Loads the actual settings.
    void Load(Settings* settings, LayoutSettings* defaults);

    // Padding around the items. Default: 0,0,0,0
    RECT padding;

    // Horizontal spacing between the items. Default: 2
    int columnSpacing;

    // Vertical spacing between the items. Default: 2
    int rowSpacing;

    // Where to start adding items. Default: TopLeft
    STARTPOSITION startPosition;

    // Direction to go first from the starting position. Default: Horizontal
    DIRECTION primaryDirection;
};
