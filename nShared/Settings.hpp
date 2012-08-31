/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  Settings.hpp                                                  August, 2012
 *  The nModules Project
 *
 *  Declaration of the Settings class.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#ifndef SETTINGS_HPP
#define SETTINGS_HPP

typedef DWORD ARGB;
#include <d2d1.h>

class Settings {
public:
    explicit Settings(LPCSTR pszPrefix);
    virtual ~Settings();

    ARGB GetColor(LPCSTR pszSetting, ARGB defValue);
    D2D_COLOR_F GetColor(LPCSTR pszSetting, D2D_COLOR_F defValue);
    void SetColor(LPCSTR pszSettingm, ARGB colorValue);
    void SetColor(LPCSTR pszSetting, D2D_COLOR_F colorValue);
    
    bool GetString(LPCSTR pszSetting, LPSTR pszDest, UINT cchDest, LPCSTR pszDefault);
    bool GetString(LPCSTR pszSetting, LPWSTR pszwDest, UINT cchDest, LPCSTR pszDefault);
    void SetString(LPCSTR pszSetting, LPCSTR pszValue);
    
    int GetInt(LPCSTR pszSetting, int iDefault);
    void SetInt(LPCSTR pszSetting, int iValue);
    
    float GetFloat(LPCSTR pszSetting, float fDefault);
    void SetFloat(LPCSTR pszSetting, float fValue);
    
    double GetDouble(LPCSTR pszSetting, double dDefault);
    void SetDouble(LPCSTR pszSetting, double dValue);

    bool GetBool(LPCSTR pszSetting, bool bDefault);
    void SetBool(LPCSTR pszSetting, bool bValue);
    
    UINT GetMonitor(LPCSTR pszSetting, UINT uDefault);
    void SetMonitor(LPCSTR pszSetting, UINT uDefault);
    
    void GetRectFromXYWH(LPCSTR pszX, LPCSTR pszY, LPCSTR pszW, LPCSTR pszH, LPRECT pDest, LPRECT pDefault);
    void GetRectFromXYWH(LPCSTR pszX, LPCSTR pszY, LPCSTR pszW, LPCSTR pszH, LPRECT pDest, int defX, int defY, int defW, int defH);
    void SetXYWHFromRect(LPCSTR pszX, LPCSTR pszY, LPCSTR pszW, LPCSTR pszH, LPRECT pValue);
    
    void GetOffsetRect();
    void SetOfsetRect();
    
private:
	explicit Settings(LPCSTR pszPrefix, LPCSTR pszPrev[]);
	
	Settings* GetGroup(LPCSTR pszPrev[]);

    LPCSTR m_pszPrefix;

    Settings* m_pGroup;
};

#endif /* SETTINGS_HPP */
