/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  System.cpp
 *  The nModules Project
 *
 *  Description...
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include <Windows.h>
#include "Core.h"

static IParsedText* (__cdecl * _pParseText)(LPCWSTR);
static BOOL (__cdecl * _pRegisterDynamicTextFunction)(LPCWSTR, UCHAR, FORMATTINGPROC, bool);
static BOOL (__cdecl * _pUnRegisterDynamicTextFunction)(LPCWSTR, UCHAR);
static BOOL (__cdecl * _pDynamicTextChangeNotification)(LPCWSTR, UCHAR);
static void (__cdecl * _pRegisterWindow)(LPCSTR, LPVOID);
static void (__cdecl * _pUnRegisterWindow)(LPCSTR);
static DrawableWindow* (__cdecl * _pFindRegisteredWindow)(LPCSTR);
static void (__cdecl * _pAddWindowRegistrationListener)(LPCSTR, DrawableWindow*);
static void (__cdecl * _pRemoveWindowRegistrationListener)(LPCSTR, DrawableWindow*);


/// <summary>
/// Initalizes the core communications.
/// </summary>
/// <returns>True if the core is succefully initalized.</returns>
HRESULT nCore::System::_Init(HMODULE hCoreInstance) {
    INIT_FUNC(_pParseText,IParsedText* (__cdecl *)(LPCWSTR), "ParseText");
    INIT_FUNC(_pRegisterDynamicTextFunction, BOOL (__cdecl *)(LPCWSTR, UCHAR, FORMATTINGPROC, bool), "RegisterDynamicTextFunction");
    INIT_FUNC(_pUnRegisterDynamicTextFunction, BOOL (__cdecl *)(LPCWSTR, UCHAR), "UnRegisterDynamicTextFunction");
    INIT_FUNC(_pDynamicTextChangeNotification, BOOL (__cdecl *)(LPCWSTR, UCHAR), "DynamicTextChangeNotification");

    INIT_FUNC(_pRegisterWindow, void (__cdecl *)(LPCSTR, LPVOID), "RegisterWindow");
    INIT_FUNC(_pUnRegisterWindow, void (__cdecl *)(LPCSTR), "UnRegisterWindow");
    INIT_FUNC(_pFindRegisteredWindow, DrawableWindow* (__cdecl *)(LPCSTR), "FindRegisteredWindow");
    INIT_FUNC(_pAddWindowRegistrationListener, void (__cdecl *)(LPCSTR, DrawableWindow*), "AddWindowRegistrationListener");
    INIT_FUNC(_pRemoveWindowRegistrationListener, void (__cdecl *)(LPCSTR, DrawableWindow*), "RemoveWindowRegistrationListener");

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


BOOL nCore::System::DynamicTextChangeNotification(LPCWSTR name, UCHAR numArgs) {
    return _pDynamicTextChangeNotification(name, numArgs);
}


void nCore::System::RegisterWindow(LPCSTR prefix, LPVOID window) {
    _pRegisterWindow(prefix, window);
}


void nCore::System::UnRegisterWindow(LPCSTR prefix) {
    _pUnRegisterWindow(prefix);
}


DrawableWindow *nCore::System::FindRegisteredWindow(LPCSTR prefix) {
    return _pFindRegisteredWindow(prefix);
}


void nCore::System::AddWindowRegistrationListener(LPCSTR prefix, DrawableWindow *listener) {
    _pAddWindowRegistrationListener(prefix, listener);
}


void nCore::System::RemoveWindowRegistrationListener(LPCSTR prefix, DrawableWindow *listener) {
    _pRemoveWindowRegistrationListener(prefix, listener);
}
