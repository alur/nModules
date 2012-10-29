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


/// <summary>
/// Initalizes the class to all default settings.
/// </summary>
BrushSettings::BrushSettings() {
    StringCchCopy(this->brushType, sizeof(this->brushType), "SolidColor");
    this->color = 0xFF000000;
    StringCchCopy(this->gradientColors, sizeof(this->gradientColors), "black");
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
    StringCchCopy(this->gradientStops, sizeof(this->gradientStops), "0.0");
    StringCchCopy(this->image, sizeof(this->image), "");
    this->imageOpacity = 1.0f;
    this->imageRotation = 0.0f;
    StringCchCopy(this->imageScalingMode, sizeof(this->imageScalingMode), "Tile");
    StringCchCopy(this->tilingModeX, sizeof(this->tilingModeX), "Tile");
    StringCchCopy(this->tilingModeY, sizeof(this->tilingModeY), "Tile");
}


/// <summary>
/// Destructor.
/// </summary>
BrushSettings::~BrushSettings() {
}


/// <summary>
/// Loads settings from an RC file using the specified defaults.
/// </summary>
void BrushSettings::Load(Settings* settings, BrushSettings* defaults) {
    settings->GetString("BrushType", this->brushType, sizeof(this->brushType), defaults->brushType);
    this->color = settings->GetColor("Color", defaults->color);
    this->color = this->color & 0xFFFFFF | ((ARGB)settings->GetInt("Alpha", (this->color & 0xFF000000) >> 24) & 0xFF) << 24;
    this->gradientEndX = settings->GetFloat("GradientEndX", defaults->gradientEndX);
    this->gradientEndY = settings->GetFloat("GradientEndY", defaults->gradientEndY);
    settings->GetString("GradientColors", this->gradientColors, sizeof(this->gradientColors), defaults->gradientColors);
    this->gradientCenterX = settings->GetFloat("GradientCenterX", defaults->gradientCenterX);
    this->gradientCenterY = settings->GetFloat("GradientCenterY", defaults->gradientCenterY);
    this->gradientOriginOffsetX = settings->GetFloat("GradientOriginOffsetX", defaults->gradientOriginOffsetX);
    this->gradientOriginOffsetY = settings->GetFloat("GradientOriginOffsetY", defaults->gradientOriginOffsetY);
    this->gradientRadiusX = settings->GetFloat("GradientRadiusX", defaults->gradientRadiusX);
    this->gradientRadiusY = settings->GetFloat("GradientRadiusY", defaults->gradientRadiusY);
    this->gradientStartX = settings->GetFloat("GradientStartX", defaults->gradientStartX);
    this->gradientStartY = settings->GetFloat("GradientStartY", defaults->gradientStartY);
    settings->GetString("GradientStops", this->gradientStops, sizeof(this->gradientStops), defaults->gradientStops);
    settings->GetString("Image", this->image, sizeof(this->image), defaults->image);
    this->imageOpacity = settings->GetInt("Alpha", int(defaults->imageOpacity*255))/255.0f;
    this->imageRotation = settings->GetFloat("ImageRotation", defaults->imageRotation);
    settings->GetString("ImageScalingMode", this->imageScalingMode, sizeof(this->imageScalingMode), defaults->imageScalingMode);
    settings->GetString("TilingModeX", this->tilingModeX, sizeof(this->tilingModeX), defaults->tilingModeX);
    settings->GetString("TilingModeY", this->tilingModeY, sizeof(this->tilingModeY), defaults->tilingModeY);
}
