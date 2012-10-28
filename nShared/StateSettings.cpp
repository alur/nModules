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
    this->cornerRadiusX = 0.0f;
    this->cornerRadiusY = 0.0f;
    StringCchCopyW(this->font, sizeof(this->font)/sizeof(WCHAR), L"Arial");
    this->fontSize = 12.0f;
    StringCchCopy(this->fontStretch, sizeof(this->fontStretch), "Normal");
    StringCchCopy(this->fontStyle, sizeof(this->fontStyle), "Normal");
    StringCchCopy(this->fontWeight, sizeof(this->fontWeight), "Normal");
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

    this->backgroundBrush.color = 0xFF000000;
    this->textBrush.color = 0xFFFFFFFF;
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
    this->cornerRadiusX = settings->GetFloat("CornerRadiusX", defaults->cornerRadiusX);
    this->cornerRadiusY = settings->GetFloat("CornerRadiusY", defaults->cornerRadiusY);
    settings->GetString("Font", this->font, sizeof(this->font)/sizeof(WCHAR), defaults->font);
    this->fontSize = settings->GetFloat("FontSize", defaults->fontSize);
    settings->GetString("FontStretch", this->fontStretch, sizeof(this->fontStretch), defaults->fontStretch);
    settings->GetString("FontStyle", this->fontStyle, sizeof(this->fontStyle), defaults->fontStyle);
    settings->GetString("FontWeight", this->fontWeight, sizeof(this->fontWeight), defaults->fontWeight);
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

    this->backgroundBrush.Load(settings, &defaults->backgroundBrush);
    
    Settings* outlineSettings = settings->CreateChild("Outline");
    this->outlineBrush.Load(outlineSettings, &defaults->outlineBrush);
    delete outlineSettings;

    Settings* textSettings = settings->CreateChild("Font");
    this->textBrush.Load(textSettings, &defaults->textBrush);
    delete textSettings;
}
