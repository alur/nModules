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
#include "../Utilities/Unordered1To1Map.hpp"

template <typename SettingType>
using SettingMap = const Unordered1To1Map<
    SettingType,
    LPCTSTR,
    std::hash<SettingType>,
    CaseInsensitive::Hash,
    std::equal_to<SettingType>,
    CaseInsensitive::Equal
>;

// String < -- > D2D1_TEXT_ANTIALIAS_MODE
static SettingMap<D2D1_TEXT_ANTIALIAS_MODE> textAntiAliasModeMap(
{
    { D2D1_TEXT_ANTIALIAS_MODE_CLEARTYPE, _T("ClearType") },
    { D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE, _T("GrayScale") },
    { D2D1_TEXT_ANTIALIAS_MODE_ALIASED,   _T("Aliased")   }
});


/// <summary>
/// Initalizes the class to all default settings.
/// </summary>
WindowSettings::WindowSettings()
    : alwaysOnTop(false)
    , blurBehind(false)
    , clickThrough(false)
    , evaluateText(false)
    , height(100)
    , hidden(false)
    , registerWithCore(false)
    , textAntiAliasMode(D2D1_TEXT_ANTIALIAS_MODE_CLEARTYPE)
    , width(100)
    , x(0)
    , y(0)
{
    *text = _T('\0');
}


/// <summary>
/// Initalizes the class to all default settings.
/// </summary>
WindowSettings::WindowSettings(void init(WindowSettings &))
    : WindowSettings()
{
    init(*this);
}


/// <summary>
/// Loads settings from an RC file using the specified defaults.
/// </summary>
void WindowSettings::Load(const Settings *settings, const WindowSettings *defaults) {
  WCHAR buf[MAX_LINE_LENGTH];
  if (!defaults) {
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
  StringCchCopy(this->text, _countof(this->text), buf);
  settings->GetString(_T("TextAntiAliasMode"), buf, _countof(buf), textAntiAliasModeMap.GetByA(defaults->textAntiAliasMode, _T("ClearType")));
  this->textAntiAliasMode = ParseAntiAliasMode(buf);
  this->width = settings->GetRelatedNumber(_T("Width"), defaults->width);
  this->x = settings->GetRelatedNumber(_T("X"), defaults->x);
  this->y = settings->GetRelatedNumber(_T("Y"), defaults->y);
}


/// <summary>
/// Parses 
/// </summary>
D2D1_TEXT_ANTIALIAS_MODE WindowSettings::ParseAntiAliasMode(LPCTSTR str)
{
    return textAntiAliasModeMap.GetByB(str, D2D1_TEXT_ANTIALIAS_MODE_CLEARTYPE);
}
