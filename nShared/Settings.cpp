/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  Settings.cpp                                                  August, 2012
 *  The nModules Project
 *
 *  Provides functions for 
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include <strsafe.h>
#include "../headers/lsapi.h"
#include "Settings.hpp"
#include "Error.h"
#include "../nCoreCom/Core.h"


using namespace nCore::InputParsing;


Settings::Settings(LPCSTR pszPrefix, LPCSTR pszPrev[]) {
    char szBuf[MAX_LINE_LENGTH];

    m_pszPrefix = _strdup(pszPrefix);
    m_pGroup = NULL;
    
    GetPrefixedRCString(m_pszPrefix, "Group", szBuf, "", sizeof(szBuf));
    
    if (szBuf[0] != '\0') {
        
        // Avoid circular definitions
        int i = 0;
        if (pszPrev != NULL) {
            for (; pszPrev[i] != NULL; i++) {
                if (strcmp(pszPrev[i], szBuf) == 0) {
					char szMsg[MAX_LINE_LENGTH];
					
					StringCchCopy(szMsg, sizeof(szMsg), "Circular group definition!\n");
					for (int j = 0; pszPrev[j] != NULL; j++) {
						StringCchCat(szMsg, sizeof(szMsg), pszPrev[j]);
						StringCchCat(szMsg, sizeof(szMsg), " -> ");
					}
					StringCchCat(szMsg, sizeof(szMsg), szBuf);
					
					ErrorMessage(E_LVL_ERROR, szMsg);
					
					return;
                }
            }
        }
        
        pszPrev = (LPCSTR*)realloc(pszPrev, (i+2)*sizeof(LPCSTR));
        pszPrev[i] = m_pszPrefix;
        pszPrev[i+1] = NULL;
    
        m_pGroup = new Settings(szBuf, pszPrev);
    }
}


Settings::~Settings() {
    free((LPVOID)m_pszPrefix);
    if (m_pGroup) {
        delete m_pGroup;
    }
}


ARGB Settings::Get(LPCSTR pszSetting, ARGB defColor) {
	GetPrefixedRCColor(m_pszPrefix, pszSetting, m_pGroup != NULL ? m_pGroup->Get(pszSetting, defColor) : defColor);
}


void Settings::Set(LPCSTR pszSetting, ARGB colorValue) {
	char szString[32];
	StringCchPrintf(szString, sizeof(szString), "%x", colorValue);
	Set(pszSetting, szString);
}


bool Settings::Get(LPCSTR pszSetting, LPSTR pszDest, UINT cchDest, LPCSTR pszDesfault) {
	return false;
}


void Settings::Set(LPCSTR pszSetting, LPCSTR pszValue) {
    char szOptionName[MAX_LINE_LENGTH];
    StringCchPrintf(szOptionName, MAX_LINE_LENGTH, "%s%s", m_pszPrefix, pszSetting);
	LSSetVariable(szOptionName, pszValue);
}


int Settings::Get(LPCSTR pszSetting, int iDefault) {
    GetPrefixedRCInt(m_pszPrefix, pszSetting, m_pGroup != NULL ? m_pGroup->Get(pszSetting, iDefault) : iDefault);
}


void Settings::Set(LPCSTR pszSetting, int iValue) {
	char szString[10];
	_itoa_s(iValue, szString, sizeof(szString), 10);
	Set(pszSetting, szString);
}


float Settings::Get(LPCSTR pszSetting, float fDefault) {
    GetPrefixedRCFloat(m_pszPrefix, pszSetting, m_pGroup != NULL ? m_pGroup->Get(pszSetting, fDefault) : fDefault);
}


void Settings::Set(LPCSTR pszSetting, float fValue) {
	char szString[32];
	StringCchPrintf(szString, sizeof(szString), "%.30f", fValue);
	Set(pszSetting, szString);
}
