/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  System.cpp                                                      July, 2012
 *  The nModules Project
 *
 *  Description...
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include <Windows.h>
#include "Core.h"

BOOL (__cdecl * _pFormatText)(LPCWSTR, size_t, LPWSTR);


/// <summary>
/// Initalizes the core communications.
/// </summary>
/// <returns>True if the core is succefully initalized.</returns>
HRESULT nCore::System::Init(HMODULE hCoreInstance) {
    INIT_FUNC(_pFormatText,BOOL (__cdecl *)(LPCWSTR, size_t, LPWSTR),"FormatText")
    
    return S_OK;
}


BOOL nCore::System::FormatText(LPCWSTR pszSource, size_t cchDest, LPWSTR pszDest) {
    return _pFormatText(pszSource, cchDest, pszDest);
}
