/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  StrokeSettings.hpp
 *  The nModules Project
 *
 *  Settings used to define a stroke style.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

#include "../Utilities/Common.h"
#include "Settings.hpp"
#include <d2d1.h>
#include <vector>

class StrokeSettings
{
public:
    StrokeSettings();

    void Load(Settings *settings);

public:
    D2D1_CAP_STYLE startCapStyle;
    D2D1_CAP_STYLE endCapStyle;
    D2D1_CAP_STYLE dashCapStyle;
    D2D1_LINE_JOIN lineJoinStyle;
    D2D1_DASH_STYLE dashStyle;
    float miterLimit;
    float dashOffset;
};
