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
BOOL (__cdecl * _pRegisterDynamicTextFunction)(LPCWSTR, UCHAR, FORMATTINGPROC, bool);
BOOL (__cdecl * _pUnRegisterDynamicTextFunction)(LPCWSTR, UCHAR);


/// <summary>
/// Initalizes the core communications.
/// </summary>
/// <returns>True if the core is succefully initalized.</returns>
HRESULT nCore::System::Init(HMODULE hCoreInstance) {
    INIT_FUNC(_pParseText,IParsedText* (__cdecl *)(LPCWSTR),"ParseText");
    INIT_FUNC(_pRegisterDynamicTextFunction, BOOL (__cdecl *)(LPCWSTR, UCHAR, FORMATTINGPROC, bool), "RegisterDynamicTextFunction");
    INIT_FUNC(_pUnRegisterDynamicTextFunction, BOOL (__cdecl *)(LPCWSTR, UCHAR), "UnRegisterDynamicTextFunction");
    
    return S_OK;
}


IParsedText* nCore::System::ParseText(LPCWSTR text) {
    return _pParseText(text);
}


BOOL nCore::System::RegisterDynamicTextFunction(LPCWSTR name, UCHAR numArgs, FORMATTINGPROC formatter, bool dynamic) {
    return _pRegisterDynamicTextFunction(name, numArgs, formatter, dynamic);
}


BOOL nCore::System::UnRegisterDynamicTextFunction(LPCWSTR name, UCHAR numArgs) {
    return _pUnRegisterDynamicTextFunction(name, numArgs);
}
