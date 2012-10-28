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
    StringCchCopy(this->image, sizeof(this->image), "");
    StringCchCopy(this->imageScalingMode, sizeof(this->imageScalingMode), "Tile");
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
    settings->GetString("Image", this->image, sizeof(this->image), defaults->image);
    settings->GetString("ImageScalingMode", this->imageScalingMode, sizeof(this->imageScalingMode), defaults->imageScalingMode);
}
