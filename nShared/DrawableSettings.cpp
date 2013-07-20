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
#include "../Utilities/StringUtils.h"


/// <summary>
/// Initalizes the class to all default settings.
/// </summary>
DrawableSettings::DrawableSettings() {
    this->alwaysOnTop = false;
    this->blurBehind = false;
    this->clickThrough = false;
    this->evaluateText = false;
    this->height = 100;
    this->hidden = false;
    this->registerWithCore = false;
    this->text = _wcsdup(L"");
    this->width = 100;
    this->x = 0;
    this->y = 0;
}


/// <summary>
/// Destructor.
/// </summary>
DrawableSettings::~DrawableSettings() {
    free(this->text);
}


/// <summary>
/// Loads settings from an RC file using the specified defaults.
/// </summary>
void DrawableSettings::Load(Settings* settings, DrawableSettings* defaults) {
    WCHAR buf[MAX_LINE_LENGTH];
    if (!defaults) {
        defaults = this;
    }

    this->alwaysOnTop = settings->GetBool("AlwaysOnTop", defaults->alwaysOnTop);
    this->blurBehind = settings->GetBool("BlurBehind", defaults->blurBehind);
    this->clickThrough = settings->GetBool("ClickThrough", defaults->clickThrough);
    this->evaluateText = defaults->evaluateText;
    this->height = settings->GetInt("Height", defaults->height);
    this->hidden = settings->GetBool("Hidden", defaults->hidden);
    this->registerWithCore = defaults->registerWithCore;
    settings->GetString("Text", buf, MAX_LINE_LENGTH, defaults->text);
    this->text = StringUtils::ReallocOverwrite(this->text, buf);
    this->width = settings->GetInt("Width", defaults->width);
    this->x = settings->GetInt("X", defaults->x);
    this->y = settings->GetInt("Y", defaults->y);
}
