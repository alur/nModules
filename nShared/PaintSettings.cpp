/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  PaintSettings.cpp                                               July, 2012
 *  The nModules Project
 *
 *  Implementation of the PaintSettings class. The PaintSettings class loads
 *  and maintains all settings for a certain paintable object.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "../headers/lsapi.h"
#include "PaintSettings.hpp"
#include "../nCoreCom/Core.h"


/// <summary>
/// Constructor
/// </summary>
PaintSettings::PaintSettings(LPCSTR pszPrefix) {
    m_pszPrefix = _strdup(pszPrefix);
    Load();
}


/// <summary>
/// Destructor
/// </summary>
PaintSettings::~PaintSettings() {
    free((LPVOID)m_pszPrefix);
    free((LPVOID)text);
    free((LPVOID)font);
}


/// <summary>
/// Loads/Reloads all settings
/// </summary>
void PaintSettings::Load() {
    using namespace nCore::InputParsing;

    char szBuf[MAX_LINE_LENGTH];
    wchar_t wszBuf[MAX_LINE_LENGTH];
    DWORD argb;

    position.left = GetPrefixedRCInt(m_pszPrefix, "X", 0);
    position.top = GetPrefixedRCInt(m_pszPrefix, "Y", 0);
    position.right = position.left + GetPrefixedRCInt(m_pszPrefix, "Width", 100);
    position.bottom = position.top + GetPrefixedRCInt(m_pszPrefix, "Height", 100);

    argb = GetPrefixedRCColor(m_pszPrefix, "Color", 0xFF000000);
    backColor = ARGBToD2DColor(argb);
    backColor.a = GetPrefixedRCInt(m_pszPrefix, "Alpha", argb >> 24)/255.0f; // Alpha overrides #argb

    argb = GetPrefixedRCColor(m_pszPrefix, "FontColor", 0xFFFFFFFF);
    fontColor = ARGBToD2DColor(argb);
    fontColor.a = GetPrefixedRCInt(m_pszPrefix, "FontAlpha", argb >> 24)/255.0f; // Alpha overrides #argb
    
    DWMBlur = GetPrefixedRCBool(m_pszPrefix, "DWMBlur", false);

    GetPrefixedRCWString(m_pszPrefix, "Text", wszBuf, " ", sizeof(wszBuf)/sizeof(wchar_t));
    text = _wcsdup(wszBuf);
    GetPrefixedRCWString(m_pszPrefix, "Font", wszBuf, "Arial", sizeof(wszBuf)/sizeof(wchar_t));
    font = _wcsdup(wszBuf);

    fontSize = GetPrefixedRCFloat(m_pszPrefix, "FontSize", 12.0f);

    textOffset.bottom = GetPrefixedRCFloat(m_pszPrefix, "TextOffsetBottom", 0.0f);
    textOffset.top = GetPrefixedRCFloat(m_pszPrefix, "TextOffsetTop", 0.0f);
    textOffset.left = GetPrefixedRCFloat(m_pszPrefix, "TextOffsetLeft", 0.0f);
    textOffset.right = GetPrefixedRCFloat(m_pszPrefix, "TextOffsetRight", 0.0f);

    GetPrefixedRCString(m_pszPrefix, "TextAlign", szBuf, "Left", sizeof(szBuf));
    if (_stricmp(szBuf, "Center") == 0)
        textAlignment = DWRITE_TEXT_ALIGNMENT_CENTER;
    else if (_stricmp(szBuf, "Right") == 0)
        textAlignment = DWRITE_TEXT_ALIGNMENT_TRAILING;
    else
        textAlignment = DWRITE_TEXT_ALIGNMENT_LEADING;

    GetPrefixedRCString(m_pszPrefix, "TextVerticalAlign", szBuf, "Top", sizeof(szBuf));
    if (_stricmp(szBuf, "Middle") == 0)
        textVerticalAlignment = DWRITE_PARAGRAPH_ALIGNMENT_CENTER;
    else if (_stricmp(szBuf, "Bottom") == 0)
        textVerticalAlignment = DWRITE_PARAGRAPH_ALIGNMENT_FAR;
    else
        textVerticalAlignment = DWRITE_PARAGRAPH_ALIGNMENT_NEAR;
}


void PaintSettings::OverLoad(LPCSTR pszPrefix) {
    using namespace nCore::InputParsing;
    DWORD argb;

    argb = GetPrefixedRCColor(pszPrefix, "Color", GetPrefixedRCColor(m_pszPrefix, "Color", 0xFF000000));
    backColor = ARGBToD2DColor(argb);
    backColor.a = GetPrefixedRCInt(pszPrefix, "Alpha", GetPrefixedRCInt(m_pszPrefix, "Alpha", argb >> 24))/255.0f; // Alpha overrides #argb
}


/// <summary>
/// Converts GDI+'s ARGB format to a D2D color
/// </summary>
D2D_COLOR_F PaintSettings::ARGBToD2DColor(DWORD argb) {
    D2D_COLOR_F ret;
    ret.a = (argb >> 24)/255.0f;
    ret.r = (argb >> 16 & 0xFF)/255.0f;
    ret.g = (argb >> 8 & 0xFF)/255.0f;
    ret.b = (argb & 0xFF)/255.0f;
    return ret;
}


void PaintSettings::setText(LPCWSTR pszwText) {
    free((LPVOID)text);
    text = _wcsdup(pszwText);
}
