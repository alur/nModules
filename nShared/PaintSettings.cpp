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


/// <summary>
/// Constructor
/// </summary>
PaintSettings::PaintSettings(LPCSTR pszPrefix) {
    m_pszPrefix = _strdup(pszPrefix);
    m_pSettings = new Settings(pszPrefix);
    Load();
}


/// <summary>
/// Destructor
/// </summary>
PaintSettings::~PaintSettings() {
	delete m_pSettings;
    free((LPVOID)m_pszPrefix);
    free((LPVOID)text);
    free((LPVOID)font);
}


/// <summary>
/// Loads/Reloads all settings
/// </summary>
void PaintSettings::Load() {
    char szBuf[MAX_LINE_LENGTH];
    wchar_t wszBuf[MAX_LINE_LENGTH];
    DWORD argb;

	m_pSettings->GetRectFromXYWH("X", "Y", "Width", "Height", &position, 0, 0, 100, 100);

    m_bAlwaysOnTop = m_pSettings->GetBool("AlwaysOnTop", false);

	argb = m_pSettings->GetColor("Color", 0xFF000000);
    backColor = ARGBToD2DColor(argb);
    backColor.a = m_pSettings->GetInt("Alpha", argb >> 24)/255.0f; // Alpha overrides #argb

    argb = m_pSettings->GetColor("FontColor", 0xFFFFFFFF);
    fontColor = ARGBToD2DColor(argb);
    fontColor.a = m_pSettings->GetInt("FontAlpha", argb >> 24)/255.0f; // Alpha overrides #argb
    
    DWMBlur = m_pSettings->GetBool("DWMBlur", false);

    m_pSettings->GetString("Text", wszBuf, sizeof(wszBuf)/sizeof(wchar_t), "");
    text = _wcsdup(wszBuf);
    m_pSettings->GetString("Font", wszBuf, sizeof(wszBuf)/sizeof(wchar_t), "Arial");
    font = _wcsdup(wszBuf);

    fontSize = m_pSettings->GetFloat("FontSize", 12.0f);

    textOffset.bottom = m_pSettings->GetFloat("TextOffsetBottom", 0.0f);
    textOffset.top = m_pSettings->GetFloat("TextOffsetTop", 0.0f);
    textOffset.left = m_pSettings->GetFloat("TextOffsetLeft", 0.0f);
    textOffset.right = m_pSettings->GetFloat("TextOffsetRight", 0.0f);

    m_pSettings->GetString("TextAlign", szBuf, sizeof(szBuf), "Left");
    if (_stricmp(szBuf, "Center") == 0)
        textAlignment = DWRITE_TEXT_ALIGNMENT_CENTER;
    else if (_stricmp(szBuf, "Right") == 0)
        textAlignment = DWRITE_TEXT_ALIGNMENT_TRAILING;
    else
        textAlignment = DWRITE_TEXT_ALIGNMENT_LEADING;

    m_pSettings->GetString("TextVerticalAlign", szBuf, sizeof(szBuf), "Top");
    if (_stricmp(szBuf, "Middle") == 0)
        textVerticalAlignment = DWRITE_PARAGRAPH_ALIGNMENT_CENTER;
    else if (_stricmp(szBuf, "Bottom") == 0)
        textVerticalAlignment = DWRITE_PARAGRAPH_ALIGNMENT_FAR;
    else
        textVerticalAlignment = DWRITE_PARAGRAPH_ALIGNMENT_NEAR;
}


void PaintSettings::OverLoad(LPCSTR pszPrefix) {
    DWORD argb;
    Settings* settings = new Settings(pszPrefix);

    argb = settings->GetColor("Color", m_pSettings->GetColor("Color", 0xFF000000));
    backColor = ARGBToD2DColor(argb);
    backColor.a = settings->GetInt("Alpha", m_pSettings->GetInt("Alpha", argb >> 24))/255.0f; // Alpha overrides #argb

    delete settings;
}


/// <summary>
/// Converts GDI+'s ARGB format to a D2D color.
/// </summary>
D2D_COLOR_F PaintSettings::ARGBToD2DColor(DWORD argb) {
    D2D_COLOR_F ret;
    ret.a = (argb >> 24)/255.0f;
    ret.r = (argb >> 16 & 0xFF)/255.0f;
    ret.g = (argb >> 8 & 0xFF)/255.0f;
    ret.b = (argb & 0xFF)/255.0f;
    return ret;
}


/// <summary>
/// Sets the text value.
/// </summary>
void PaintSettings::setText(LPCWSTR pszwText) {
    free((LPVOID)text);
    text = _wcsdup(pszwText);
}


/// <summary>
/// Returns the settings class used by this paintsettings.
/// </summary>
Settings* PaintSettings::GetSettings() {
    return m_pSettings;
}