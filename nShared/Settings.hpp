/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  Settings.hpp
 *  The nModules Project
 *
 *  Declaration of the Settings class.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

#include <d2d1.h>


typedef DWORD ARGB;


class Settings {
public:
    explicit Settings(LPCSTR prefix);

    // Makes a deep copy of the specified settings.
    explicit Settings(Settings* settings);

    virtual ~Settings();

    Settings* CreateChild(LPCSTR prefix);

    void AppendGroup(Settings* group);

    ARGB GetColor(LPCSTR key, ARGB defValue);
    D2D_COLOR_F GetColor(LPCSTR key, D2D_COLOR_F defValue);
    void SetColor(LPCSTR key, ARGB colorValue);
    void SetColor(LPCSTR key, D2D_COLOR_F colorValue);
    
    bool GetString(LPCSTR key, LPSTR pszDest, UINT cchDest, LPCSTR pszDefault);
    bool GetString(LPCSTR key, LPWSTR pszwDest, UINT cchDest, LPCSTR pszDefault);
    bool GetString(LPCSTR key, LPWSTR pszwDest, UINT cchDest, LPCWSTR pszDefault);
    void SetString(LPCSTR key, LPCSTR pszValue);
    
    int GetInt(LPCSTR key, int iDefault);
    void SetInt(LPCSTR key, int iValue);
    
    float GetFloat(LPCSTR key, float fDefault);
    void SetFloat(LPCSTR key, float fValue);
    
    double GetDouble(LPCSTR key, double dDefault);
    void SetDouble(LPCSTR key, double dValue);

    bool GetBool(LPCSTR key, bool defaultValue);
    void SetBool(LPCSTR key, bool value);
    
    UINT GetMonitor(LPCSTR key, UINT defaultValue);
    void SetMonitor(LPCSTR key, UINT value);
    
    void GetRectFromXYWH(LPCSTR pszX, LPCSTR pszY, LPCSTR pszW, LPCSTR pszH, LPRECT pDest, LPRECT pDefault);
    void GetRectFromXYWH(LPCSTR pszX, LPCSTR pszY, LPCSTR pszW, LPCSTR pszH, LPRECT pDest, int defX, int defY, int defW, int defH);
    void SetXYWHFromRect(LPCSTR pszX, LPCSTR pszY, LPCSTR pszW, LPCSTR pszH, LPRECT pValue);
    
    void GetOffsetRect(LPCSTR pszLeft, LPCSTR pszTop, LPCSTR pszRight, LPCSTR pszBottom, LPRECT pDest, LPRECT pDefault);
    void GetOffsetRect(LPCSTR pszLeft, LPCSTR pszTop, LPCSTR pszRight, LPCSTR pszBottom, LPRECT pDest, int defLeft, int defTop, int defRight, int defBottom);
    void SetOfsetRect(LPCSTR pszLeft, LPCSTR pszTop, LPCSTR pszRight, LPCSTR pszBottom, LPRECT pValue);

    // The fully specified prefix to read settings from the RC files with.
    LPCSTR prefix;
    
private:
	explicit Settings(LPCSTR pszPrefix, LPCSTR pszPrev[]);

    // Creates the Settings* for this settings group.
	Settings* GreateGroup(LPCSTR pszPrev[]);

    //
    Settings* group;
};
