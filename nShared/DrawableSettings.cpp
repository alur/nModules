/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  DrawableSettings.cpp
 *  The nModules Project
 *
 *  Holds all RC settings used by the DrawableWindow class.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "../headers/lsapi.h"
#include <strsafe.h>
#include "DrawableSettings.hpp"


/// <summary>
/// Initalizes the class to all default settings.
/// </summary>
DrawableSettings::DrawableSettings() {
    this->alwaysOnTop = false;
    this->blurBehind = false;
    this->color = 0xFF000000;
    StringCchCopyW(this->font, sizeof(this->font)/sizeof(WCHAR), L"Arial");
    this->fontColor = 0xFFFFFFFF;
    this->fontSize = 12.0f;
    this->height = 100;
    StringCchCopy(this->image, sizeof(this->image), "");
    StringCchCopyW(this->text, sizeof(this->text)/sizeof(WCHAR), L"");
    StringCchCopy(this->textAlign, sizeof(this->textAlign), "Left");
    this->textOffsetBottom = 0.0f;
    this->textOffsetLeft = 0.0f;
    this->textOffsetRight = 0.0f;
    this->textOffsetTop = 0.0f;
    this->textRotation = 0.0f;
    StringCchCopy(this->textVerticalAlign, sizeof(this->textVerticalAlign), "Top");
    this->width = 100;
    this->x = 0;
    this->y = 0;
}


/// <summary>
/// Destructor.
/// </summary>
DrawableSettings::~DrawableSettings() {
}


/// <summary>
/// Loads settings from an RC file using the specified defaults.
/// </summary>
void DrawableSettings::Load(Settings* settings, DrawableSettings* defaults) {
    this->alwaysOnTop = settings->GetBool("AlwaysOnTop", defaults->alwaysOnTop);
    this->blurBehind = settings->GetBool("BlurBehind", defaults->blurBehind);
    this->color = settings->GetColor("Color", defaults->color);
    this->color = this->color & 0xFFFFFF | ((ARGB)settings->GetInt("Alpha", (this->color & 0xFF000000) >> 24) & 0xFF) << 24;
    settings->GetString("Font", this->font, sizeof(this->font)/sizeof(WCHAR), defaults->font);
    this->fontColor = settings->GetColor("FontColor", defaults->fontColor);
    this->fontColor = this->fontColor & 0xFFFFFF | ((ARGB)settings->GetInt("FontAlpha", (this->fontColor & 0xFF000000) >> 24) & 0xFF) << 24;
    this->fontSize = settings->GetFloat("FontSize", defaults->fontSize);
    this->height = settings->GetInt("Height", defaults->height);
    settings->GetString("Text", this->text, sizeof(this->text)/sizeof(WCHAR), defaults->text);
    settings->GetString("TextAlign", this->textAlign, sizeof(this->textAlign), defaults->textAlign);
    this->textOffsetBottom = settings->GetFloat("TextOffsetBottom", defaults->textOffsetBottom);
    this->textOffsetLeft = settings->GetFloat("TextOffsetLeft", defaults->textOffsetLeft);
    this->textOffsetRight = settings->GetFloat("TextOffsetRight", defaults->textOffsetRight);
    this->textOffsetTop = settings->GetFloat("TextOffsetTop", defaults->textOffsetTop);
    this->textRotation = settings->GetFloat("TextRotation", defaults->textRotation);
    settings->GetString("TextVerticalAlign", this->textVerticalAlign, sizeof(this->textVerticalAlign), defaults->textVerticalAlign);
    this->width = settings->GetInt("Width", defaults->width);
    this->x = settings->GetInt("X", defaults->x);
    this->y = settings->GetInt("Y", defaults->y);
}
