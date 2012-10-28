/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  DrawableSettings.cpp
 *  The nModules Project
 *
 *  Holds all RC settings used by the DrawableWindow class.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "../nShared/LiteStep.h"
#include <strsafe.h>
#include "DrawableSettings.hpp"


/// <summary>
/// Initalizes the class to all default settings.
/// </summary>
DrawableSettings::DrawableSettings() {
    this->alwaysOnTop = false;
    this->blurBehind = false;
    this->evaluateText = false;
    this->height = 100;
    this->hidden = false;
    this->registerWithCore = false;
    StringCchCopyW(this->text, sizeof(this->text)/sizeof(WCHAR), L"");
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
    this->evaluateText = defaults->evaluateText;
    this->height = settings->GetInt("Height", defaults->height);
    this->hidden = settings->GetBool("Hidden", defaults->hidden);
    this->registerWithCore = defaults->registerWithCore;
    settings->GetString("Text", this->text, sizeof(this->text)/sizeof(WCHAR), defaults->text);
    this->width = settings->GetInt("Width", defaults->width);
    this->x = settings->GetInt("X", defaults->x);
    this->y = settings->GetInt("Y", defaults->y);
}
