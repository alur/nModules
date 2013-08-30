/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  WindowSettings.cpp
 *  The nModules Project
 *
 *  Holds all RC settings used by the Window class.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "../nShared/LiteStep.h"
#include <strsafe.h>
#include "WindowSettings.hpp"
#include "../Utilities/StringUtils.h"
#include <map>


/// <summary>
/// Initalizes the class to all default settings.
/// </summary>
WindowSettings::WindowSettings()
{
    this->alwaysOnTop = false;
    this->blurBehind = false;
    this->clickThrough = false;
    this->evaluateText = false;
    this->height = RelatedNumber(100);
    this->hidden = false;
    this->registerWithCore = false;
    this->text = _wcsdup(L"");
    this->textAntiAliasMode = D2D1_TEXT_ANTIALIAS_MODE_CLEARTYPE;
    this->width = RelatedNumber(100);
    this->x = RelatedNumber(0);
    this->y = RelatedNumber(0);
}


/// <summary>
/// Copy constructor
/// </summary>
WindowSettings::WindowSettings(const WindowSettings &wndSettings)
{
    memcpy(this, &wndSettings, sizeof(wndSettings));
    this->text = _wcsdup(this->text);
}


/// <summary>
/// Assignment operator
/// </summary>
WindowSettings &WindowSettings::operator=(const WindowSettings &wndSettings)
{
    memcpy(this, &wndSettings, sizeof(wndSettings));
    this->text = _wcsdup(this->text);
    return *this;
}


/// <summary>
/// Destructor.
/// </summary>
WindowSettings::~WindowSettings()
{
    free(this->text);
}


static std::map<D2D1_TEXT_ANTIALIAS_MODE, LPCTSTR> textAntiAliasModeMap = {
    { D2D1_TEXT_ANTIALIAS_MODE_CLEARTYPE, _T("ClearType") },
    { D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE, _T("GrayScale") },
    { D2D1_TEXT_ANTIALIAS_MODE_ALIASED,   _T("Aliased")   }
};


template <typename T>
static T GetValue(LPCTSTR string, std::map<T, LPCTSTR> map, T defValue)
{
    for (auto &x : map)
    {
        if (_tcsicmp(string, x.second) == 0)
        {
            return x.first;
        }
    }
    return defValue;
}


template <typename T>
static LPCTSTR GetName(T value, std::map<T, LPCTSTR> map)
{
    for (auto &x : map)
    {
        if (x.first == value)
        {
            return x.second;
        }
    }
    return _T("");
}


/// <summary>
/// Loads settings from an RC file using the specified defaults.
/// </summary>
void WindowSettings::Load(Settings* settings, WindowSettings* defaults)
{
    WCHAR buf[MAX_LINE_LENGTH];
    if (!defaults)
    {
        defaults = this;
    }

    this->alwaysOnTop = settings->GetBool(_T("AlwaysOnTop"), defaults->alwaysOnTop);
    this->blurBehind = settings->GetBool(_T("BlurBehind"), defaults->blurBehind);
    this->clickThrough = settings->GetBool(_T("ClickThrough"), defaults->clickThrough);
    this->evaluateText = defaults->evaluateText;
    this->height = settings->GetRelatedNumber(_T("Height"), defaults->height);
    this->hidden = settings->GetBool(_T("Hidden"), defaults->hidden);
    this->registerWithCore = defaults->registerWithCore;
    settings->GetString(_T("Text"), buf, _countof(buf), defaults->text);
    this->text = StringUtils::ReallocOverwrite(this->text, buf);
    settings->GetString(_T("TextAntiAliasMode"), buf, _countof(buf), GetName(defaults->textAntiAliasMode, textAntiAliasModeMap));
    this->textAntiAliasMode = ParseAntiAliasMode(buf);
    this->width = settings->GetRelatedNumber(_T("Width"), defaults->width);
    this->x = settings->GetRelatedNumber(_T("X"), defaults->x);
    this->y = settings->GetRelatedNumber(_T("Y"), defaults->y);
}


D2D1_TEXT_ANTIALIAS_MODE WindowSettings::ParseAntiAliasMode(LPCTSTR str)
{
    return GetValue(str, textAntiAliasModeMap, D2D1_TEXT_ANTIALIAS_MODE_CLEARTYPE);
}
