/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  BrushSettings.hpp
 *  The nModules Project
 *
 *  Settings used by brushes.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

#include <d2d1.h>
#include <dwrite.h>
#include "Settings.hpp"

class BrushSettings
{
public:
    explicit BrushSettings();
    explicit BrushSettings(const BrushSettings &source);
    virtual ~BrushSettings();

    // Loads the actual settings.
    void Load(Settings* settings, BrushSettings* defaults);

    // The type of brush to use. SolidColor, LinearGradient, RadialGradient, Image. Default: SolidColor
    TCHAR brushType[32];

    // The color of a SolidColor brush. Default: 0xFF000000 (black)
    ARGB color;

    // The colors to use in a gradient. Default: black
    LPTSTR gradientColors;

    // The x coordinate of the center of a gradient. Default: 0
    float gradientCenterX;

    // The y coordinate of the center of a gradient. Default: 0
    float gradientCenterY;

    // The x coordinate of the origin of a gradient. Default: 0
    float gradientEndX;

    // The y coordinate of the origin of a gradient. Default: 0
    float gradientEndY;

    // The x coordinate of the origin of a gradient. Default: 0
    float gradientOriginOffsetX;

    // The y coordinate of the origin of a gradient. Default: 0
    float gradientOriginOffsetY;

    // The x coordinate of the origin of a gradient. Default: 0
    float gradientRadiusX;

    // The y coordinate of the origin of a gradient. Default: 0
    float gradientRadiusY;

    // The x coordinate of the origin of a gradient. Default: 0
    float gradientStartX;

    // The y coordinate of the origin of a gradient. Default: 0
    float gradientStartY;

    // The locations along the gradient where the color changes. Default: "0.0"
    LPTSTR gradientStops;

    // The image to use for this window. Default: "" (blank)
    LPTSTR image;

    // The maximum opacity of the image. 0 to 1. Default: 1
    float imageOpacity;

    // The rotation of the image, in degrees. Default: 0
    float imageRotation;

    // How to position the image. Fit, Fill, Center, Stretch, or Tile. Default: Tile
    TCHAR imageScalingMode[32];

    // Horizontal tiling mode. Clamp, Tile, or Mirror. Default: Tile
    TCHAR tilingModeX[32];

    // Vertical tiling mode. Clamp, Tile, or Mirror. Default: Tile
    TCHAR tilingModeY[32];
};
