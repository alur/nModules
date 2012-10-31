/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  BrushSettings.cpp
 *  The nModules Project
 *
 *  Holds all RC settings used for a brush.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "../nShared/LiteStep.h"
#include <strsafe.h>
#include "BrushSettings.hpp"
#include "Strings.h"


/// <summary>
/// Initalizes the class to all default settings.
/// </summary>
BrushSettings::BrushSettings() {
    StringCchCopy(this->brushType, sizeof(this->brushType), "SolidColor");
    this->color = 0xFF000000;
    this->gradientColors = _strdup("Black");
    this->gradientCenterX = 0.0f;
    this->gradientCenterY = 0.0f;
    this->gradientEndX = 0.0f;
    this->gradientEndY = 0.0f;
    this->gradientOriginOffsetX = 0.0f;
    this->gradientOriginOffsetY = 0.0f;
    this->gradientRadiusX = 0.0f;
    this->gradientRadiusY = 0.0f;
    this->gradientStartX = 0.0f;
    this->gradientStartY = 0.0f;
    this->gradientStops = _strdup("0.0");
    this->image = _strdup("");
    this->imageOpacity = 1.0f;
    this->imageRotation = 0.0f;
    StringCchCopy(this->imageScalingMode, sizeof(this->imageScalingMode), "Tile");
    StringCchCopy(this->tilingModeX, sizeof(this->tilingModeX), "Tile");
    StringCchCopy(this->tilingModeY, sizeof(this->tilingModeY), "Tile");
}


BrushSettings::BrushSettings(const BrushSettings &source) {
    StringCchCopy(this->brushType, sizeof(this->brushType), source.brushType);
    this->color = source.color;
    this->gradientColors = _strdup(source.gradientColors);
    this->gradientCenterX = source.gradientCenterX;
    this->gradientCenterY = source.gradientCenterY;
    this->gradientEndX = source.gradientEndX;
    this->gradientEndY = source.gradientEndY;
    this->gradientOriginOffsetX = source.gradientOriginOffsetX;
    this->gradientOriginOffsetY = source.gradientOriginOffsetY;
    this->gradientRadiusX = source.gradientRadiusX;
    this->gradientRadiusY = source.gradientRadiusY;
    this->gradientStartX = source.gradientStartX;
    this->gradientStartY = source.gradientStartY;
    this->gradientStops = _strdup(source.gradientStops);
    this->image = _strdup(source.image);
    this->imageOpacity = source.imageOpacity;
    this->imageRotation = source.imageRotation;
    StringCchCopy(this->imageScalingMode, sizeof(this->imageScalingMode), source.imageScalingMode);
    StringCchCopy(this->tilingModeX, sizeof(this->tilingModeX), source.tilingModeX);
    StringCchCopy(this->tilingModeY, sizeof(this->tilingModeY), source.tilingModeY);
}


/// <summary>
/// Destructor.
/// </summary>
BrushSettings::~BrushSettings() {
    free(this->gradientColors);
    free(this->gradientStops);
    free(this->image);
}


/// <summary>
/// Loads settings from an RC file using the specified defaults.
/// </summary>
void BrushSettings::Load(Settings* settings, BrushSettings* defaults) {
    char buf[4096];

    settings->GetString("BrushType", this->brushType, sizeof(this->brushType), defaults->brushType);
    this->color = settings->GetColor("Color", defaults->color);
    this->color = this->color & 0xFFFFFF | ((ARGB)settings->GetInt("Alpha", (this->color & 0xFF000000) >> 24) & 0xFF) << 24;
    this->gradientEndX = settings->GetFloat("GradientEndX", defaults->gradientEndX);
    this->gradientEndY = settings->GetFloat("GradientEndY", defaults->gradientEndY);
    settings->GetString("GradientColors", buf, sizeof(buf), defaults->gradientColors);
    this->gradientColors = Strings::ReallocOverwrite(this->gradientColors, buf);
    this->gradientCenterX = settings->GetFloat("GradientCenterX", defaults->gradientCenterX);
    this->gradientCenterY = settings->GetFloat("GradientCenterY", defaults->gradientCenterY);
    this->gradientOriginOffsetX = settings->GetFloat("GradientOriginOffsetX", defaults->gradientOriginOffsetX);
    this->gradientOriginOffsetY = settings->GetFloat("GradientOriginOffsetY", defaults->gradientOriginOffsetY);
    this->gradientRadiusX = settings->GetFloat("GradientRadiusX", defaults->gradientRadiusX);
    this->gradientRadiusY = settings->GetFloat("GradientRadiusY", defaults->gradientRadiusY);
    this->gradientStartX = settings->GetFloat("GradientStartX", defaults->gradientStartX);
    this->gradientStartY = settings->GetFloat("GradientStartY", defaults->gradientStartY);
    settings->GetString("GradientStops", buf, sizeof(buf), defaults->gradientStops);
    this->gradientStops = Strings::ReallocOverwrite(this->gradientStops, buf);
    settings->GetString("Image", buf, sizeof(buf), defaults->image);
    this->image = Strings::ReallocOverwrite(this->image, buf);
    this->imageOpacity = settings->GetInt("Alpha", int(defaults->imageOpacity*255))/255.0f;
    this->imageRotation = settings->GetFloat("ImageRotation", defaults->imageRotation);
    settings->GetString("ImageScalingMode", this->imageScalingMode, sizeof(this->imageScalingMode), defaults->imageScalingMode);
    settings->GetString("TilingModeX", this->tilingModeX, sizeof(this->tilingModeX), defaults->tilingModeX);
    settings->GetString("TilingModeY", this->tilingModeY, sizeof(this->tilingModeY), defaults->tilingModeY);
}
