/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  DrawableSettings.cpp
 *  The nModules Project
 *
 *  Holds all RC settings used by the DrawableWindow class.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "../nShared/LiteStep.h"
#include <strsafe.h>
#include "StateSettings.hpp"


/// <summary>
/// Initalizes the class to all default settings.
/// </summary>
StateSettings::StateSettings() {
    this->color = 0xFF000000;
    this->cornerRadiusX = 0.0f;
    this->cornerRadiusY = 0.0f;
    StringCchCopyW(this->font, sizeof(this->font)/sizeof(WCHAR), L"Arial");
    this->fontColor = 0xFFFFFFFF;
    this->fontSize = 12.0f;
    StringCchCopy(this->fontStretch, sizeof(this->fontStretch), "Normal");
    StringCchCopy(this->fontStyle, sizeof(this->fontStyle), "Normal");
    StringCchCopy(this->fontWeight, sizeof(this->fontWeight), "Normal");
    StringCchCopy(this->image, sizeof(this->image), "");
    StringCchCopy(this->imageScalingMode, sizeof(this->imageScalingMode), "Tile");
    this->outlineColor = 0x00000000;
    this->outlineWidth = 0.0f;
    this->rightToLeft = false;
    StringCchCopy(this->textAlign, sizeof(this->textAlign), "Left");
    this->textOffsetBottom = 0.0f;
    this->textOffsetLeft = 0.0f;
    this->textOffsetRight = 0.0f;
    this->textOffsetTop = 0.0f;
    this->textRotation = 0.0f;
    StringCchCopy(this->textTrimmingGranularity, sizeof(this->textTrimmingGranularity), "Character");
    StringCchCopy(this->textVerticalAlign, sizeof(this->textVerticalAlign), "Top");
    this->wordWrap = false;
}


/// <summary>
/// Destructor.
/// </summary>
StateSettings::~StateSettings() {
}


/// <summary>
/// Loads settings from an RC file using the specified defaults.
/// </summary>
void StateSettings::Load(Settings* settings, StateSettings* defaults) {
    this->color = settings->GetColor("Color", defaults->color);
    this->color = this->color & 0xFFFFFF | ((ARGB)settings->GetInt("Alpha", (this->color & 0xFF000000) >> 24) & 0xFF) << 24;
    this->cornerRadiusX = settings->GetFloat("CornerRadiusX", defaults->cornerRadiusX);
    this->cornerRadiusY = settings->GetFloat("CornerRadiusY", defaults->cornerRadiusY);
    settings->GetString("Font", this->font, sizeof(this->font)/sizeof(WCHAR), defaults->font);
    this->fontColor = settings->GetColor("FontColor", defaults->fontColor);
    this->fontColor = this->fontColor & 0xFFFFFF | ((ARGB)settings->GetInt("FontAlpha", (this->fontColor & 0xFF000000) >> 24) & 0xFF) << 24;
    this->fontSize = settings->GetFloat("FontSize", defaults->fontSize);
    settings->GetString("FontStretch", this->fontStretch, sizeof(this->fontStretch), defaults->fontStretch);
    settings->GetString("FontStyle", this->fontStyle, sizeof(this->fontStyle), defaults->fontStyle);
    settings->GetString("FontWeight", this->fontWeight, sizeof(this->fontWeight), defaults->fontWeight);
    settings->GetString("Image", this->image, sizeof(this->image), defaults->image);
    settings->GetString("ImageScalingMode", this->imageScalingMode, sizeof(this->imageScalingMode), defaults->imageScalingMode);
    this->outlineColor = settings->GetColor("OutlineColor", defaults->outlineColor);
    this->outlineWidth = settings->GetFloat("OutlineWidth", defaults->outlineWidth);
    this->rightToLeft = settings->GetBool("RightToLeft", defaults->rightToLeft);
    settings->GetString("TextAlign", this->textAlign, sizeof(this->textAlign), defaults->textAlign);
    this->textOffsetBottom = settings->GetFloat("TextOffsetBottom", defaults->textOffsetBottom);
    this->textOffsetLeft = settings->GetFloat("TextOffsetLeft", defaults->textOffsetLeft);
    this->textOffsetRight = settings->GetFloat("TextOffsetRight", defaults->textOffsetRight);
    this->textOffsetTop = settings->GetFloat("TextOffsetTop", defaults->textOffsetTop);
    this->textRotation = settings->GetFloat("TextRotation", defaults->textRotation);
    settings->GetString("TextTrimmingGranularity", this->textTrimmingGranularity, sizeof(this->textTrimmingGranularity), defaults->textTrimmingGranularity);
    settings->GetString("TextVerticalAlign", this->textVerticalAlign, sizeof(this->textVerticalAlign), defaults->textVerticalAlign);
    this->wordWrap = settings->GetBool("WordWrap", defaults->wordWrap);
}
