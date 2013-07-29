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
#include <map>


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
    this->textAntiAliasMode = D2D1_TEXT_ANTIALIAS_MODE_CLEARTYPE;
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


static std::map<D2D1_TEXT_ANTIALIAS_MODE, LPCSTR> textAntiAliasModeMap = {
    { D2D1_TEXT_ANTIALIAS_MODE_CLEARTYPE, "ClearType" },
    { D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE, "GrayScale" },
    { D2D1_TEXT_ANTIALIAS_MODE_ALIASED,   "Aliased"   }
};


template <typename T>
static T GetValue(LPCSTR string, std::map<T, LPCSTR> map, T defValue) {
    for (auto &x : map) {
        if (_stricmp(string, x.second) == 0) {
            return x.first;
        }
    }
    return defValue;
}


template <typename T>
static LPCSTR GetName(T value, std::map<T, LPCSTR> map) {
    for (auto &x : map) {
        if (x.first == value) {
            return x.second;
        }
    }
    return "";
}


/// <summary>
/// Loads settings from an RC file using the specified defaults.
/// </summary>
void DrawableSettings::Load(Settings* settings, DrawableSettings* defaults) {
    WCHAR buf[MAX_LINE_LENGTH];
    CHAR buf2[MAX_LINE_LENGTH];
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
    settings->GetString("Text", buf, _countof(buf), defaults->text);
    this->text = StringUtils::ReallocOverwrite(this->text, buf);
    settings->GetString("TextAntiAliasMode", buf2, _countof(buf2), GetName(defaults->textAntiAliasMode, textAntiAliasModeMap));
    this->textAntiAliasMode = ParseAntiAliasMode(buf2);
    this->width = settings->GetInt("Width", defaults->width);
    this->x = settings->GetInt("X", defaults->x);
    this->y = settings->GetInt("Y", defaults->y);
}


D2D1_TEXT_ANTIALIAS_MODE DrawableSettings::ParseAntiAliasMode(LPCSTR str) {
    return GetValue(str, textAntiAliasModeMap, D2D1_TEXT_ANTIALIAS_MODE_CLEARTYPE);
}
