/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  System.cpp
 *  The nModules Project
 *
 *  Description...
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include <Windows.h>
#include "Core.h"

IParsedText* (__cdecl * _pParseText)(LPCWSTR);


/// <summary>
/// Initalizes the core communications.
/// </summary>
/// <returns>True if the core is succefully initalized.</returns>
HRESULT nCore::System::Init(HMODULE hCoreInstance) {
    INIT_FUNC(_pParseText,IParsedText* (__cdecl *)(LPCWSTR),"ParseText");
    
    return S_OK;
}


IParsedText* nCore::System::ParseText(LPCWSTR text) {
    return _pParseText(text);
}
