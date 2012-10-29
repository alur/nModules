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

class BrushSettings {
public:
    explicit BrushSettings();
    virtual ~BrushSettings();

    // Loads the actual settings.
    void Load(Settings* settings, BrushSettings* defaults);

    // The type of brush to use. SolidColor, LinearGradient, RadialGradient, Image. Default: SolidColor
    char brushType[32];

    // The color of a SolidColor brush. Default: 0xFF000000 (black)
    ARGB color;

    // The colors to use in a gradient. Default: black
    char gradientColors[4096];

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
    char gradientStops[4096];

    // The image to use for this window. Default: "" (blank)
    char image[MAX_PATH];

    // The maximum opacity of the image. 0 to 1. Default: 1
    float imageOpacity;

    // The rotation of the image, in degrees. Default: 0
    float imageRotation;

    // How to position the image. Fit, Fill, Center, Stretch, or Tile. Default: Tile
    char imageScalingMode[32];

    // Horizontal tiling mode. Clamp, Tile, or Mirror. Default: Tile
    char tilingModeX[32];

    // Vertical tiling mode. Clamp, Tile, or Mirror. Default: Tile
    char tilingModeY[32];
};
