/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  Settings.hpp
 *  The nModules Project
 *
 *  Declaration of the Settings class.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

#include "LiteStep.h"
#include <d2d1.h>
#include <memory>

class Settings;
typedef Settings *LPSettings;
typedef const Settings *LPCSettings;

class Settings
{
public:
    explicit Settings(LPCTSTR prefix);
    explicit Settings(LPSettings settings);
    
private:
	explicit Settings(LPCTSTR prefix, LPCTSTR prefixTrail[]);

public:
    LPSettings CreateChild(LPCTSTR prefix) const;
    void AppendGroup(LPSettings group);
    LPCTSTR GetPrefix() const;
    
private:
    // Creates the Settings* for this settings group.
	LPSettings GreateGroup(LPCTSTR prefixTrail[]);

    // Basic getters and setters
public:
    bool GetBool(LPCTSTR key, bool defaultValue) const;
    void SetBool(LPCTSTR key, bool value) const;
    ARGB GetColor(LPCTSTR key, ARGB defaultValue) const;
    void SetColor(LPCTSTR key, ARGB value) const;
    double GetDouble(LPCTSTR key, double defaultValue) const;
    void SetDouble(LPCTSTR key, double value) const;
    float GetFloat(LPCTSTR key, float defaultValue) const;
    void SetFloat(LPCTSTR key, float value) const;
    int GetInt(LPCTSTR key, int defaultValue) const;
    void SetInt(LPCTSTR key, int value) const;
    __int64 GetInt64(LPCTSTR key, __int64 defaultValue) const;
    void SetInt64(LPCTSTR key, __int64 value) const;
    bool GetLine(LPCTSTR key, LPTSTR buffer, UINT cchBuffer, LPCTSTR defaultValue) const;
    UINT GetMonitor(LPCTSTR key, UINT defaultValue) const;
    void SetMonitor(LPCTSTR key, UINT value) const;
    bool GetString(LPCTSTR key, LPTSTR buffer, UINT cchBuffer, LPCTSTR defaultValue) const;
    void SetString(LPCTSTR key, LPCTSTR value) const;
    
    // More advanced getters and setters
public:
    RECT GetRect(LPCTSTR key, LPRECT defaultValue) const;
    RECT GetRect(LPCTSTR key, LONG defX, LONG defY, LONG defWidth, LONG defHeight) const;
    void SetRect(LPCTSTR key, LPRECT value) const;
    
    RECT GetOffsetRect(LPCTSTR key, LPRECT defaultValue) const;
    RECT GetOffsetRect(LPCTSTR key, LONG defLeft, LONG defTop, LONG defRight, LONG defBottom) const;
    void SetOffsetRect(LPCTSTR key, LPRECT value) const;

    void IterateOverStars(LPCTSTR key, std::function<void (LPCTSTR token)> callback) const;
    
private:
    // The fully specified prefix to read settings from the RC files with.
    TCHAR mPrefix[MAX_RCCOMMAND];

    // Where to get settings from if they are not specified for our own prefix.
    std::unique_ptr<Settings> mGroup;
};
