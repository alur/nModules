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

class Settings {
public:
    explicit Settings(LPCSTR pszPrefix, LPCSTR pszPrev[] = NULL);
    virtual ~Settings();

    ARGB Get(LPCSTR pszSetting, ARGB defValue);
    void Set(LPCSTR pszSettingm, ARGB colorValue);
    
    bool Get(LPCSTR pszSetting, LPSTR pszDest, UINT cchDest, LPCSTR pszDesfault);
    void Set(LPCSTR pszSetting, LPCSTR pszValue);

    int Get(LPCSTR pszSetting, int iDefault);
    void Set(LPCSTR pszSetting, int iValue);
    
    float Get(LPCSTR pszSetting, float fDefault);
    void Set(LPCSTR pszSetting, float fValue);
    
private:
    LPCSTR m_pszPrefix;

    Settings* m_pGroup;
};

#endif /* SETTINGS_HPP */
