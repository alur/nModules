/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  System.cpp
 *  The nModules Project
 *
 *  Description...
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "../Utilities/Common.h"
#include "Core.h"
#include "CoreComHelpers.h"

// Pointers to function in the core. Initalized by _Init, reset by _
namespace nCore
{
    namespace System
    {
        DECL_FUNC_VAR(ParseText);
        DECL_FUNC_VAR(RegisterDynamicTextFunction);
        DECL_FUNC_VAR(UnRegisterDynamicTextFunction);
        DECL_FUNC_VAR(DynamicTextChangeNotification);

        DECL_FUNC_VAR(RegisterWindow);
        DECL_FUNC_VAR(UnRegisterWindow);
        DECL_FUNC_VAR(FindRegisteredWindow);
        DECL_FUNC_VAR(AddWindowRegistrationListener);
        DECL_FUNC_VAR(RemoveWindowRegistrationListener);
    }
}


/// <summary>
/// Initalizes the core communications.
/// </summary>
/// <returns>True if the core is succefully initalized.</returns>
HRESULT nCore::System::_Init(HMODULE hCoreInstance)
{
    INIT_FUNC(ParseText);
    INIT_FUNC(RegisterDynamicTextFunction);
    INIT_FUNC(UnRegisterDynamicTextFunction);
    INIT_FUNC(DynamicTextChangeNotification);

    INIT_FUNC(RegisterWindow);
    INIT_FUNC(UnRegisterWindow);
    INIT_FUNC(FindRegisteredWindow);
    INIT_FUNC(AddWindowRegistrationListener);
    INIT_FUNC(RemoveWindowRegistrationListener);

    return S_OK;
}


/// <summary>
/// Disconnects from the core
/// </summary>
void nCore::System::_DeInit()
{
    FUNC_VAR_NAME(ParseText) = nullptr;
    FUNC_VAR_NAME(RegisterDynamicTextFunction) = nullptr;
    FUNC_VAR_NAME(UnRegisterDynamicTextFunction) = nullptr;
    FUNC_VAR_NAME(DynamicTextChangeNotification) = nullptr;

    FUNC_VAR_NAME(RegisterWindow) = nullptr;
    FUNC_VAR_NAME(UnRegisterWindow) = nullptr;
    FUNC_VAR_NAME(FindRegisteredWindow) = nullptr;
    FUNC_VAR_NAME(AddWindowRegistrationListener) = nullptr;
    FUNC_VAR_NAME(RemoveWindowRegistrationListener) = nullptr;
}


IParsedText* nCore::System::ParseText(LPCWSTR text)
{
    assert(nCore::Initialized());
    return FUNC_VAR_NAME(ParseText)(text);
}


BOOL nCore::System::RegisterDynamicTextFunction(LPCWSTR name, UCHAR numArgs, FORMATTINGPROC formatter, bool dynamic)
{
    assert(nCore::Initialized());
    return FUNC_VAR_NAME(RegisterDynamicTextFunction)(name, numArgs, formatter, dynamic);
}


BOOL nCore::System::UnRegisterDynamicTextFunction(LPCWSTR name, UCHAR numArgs)
{
    assert(nCore::Initialized());
    return FUNC_VAR_NAME(UnRegisterDynamicTextFunction)(name, numArgs);
}


BOOL nCore::System::DynamicTextChangeNotification(LPCWSTR name, UCHAR numArgs)
{
    assert(nCore::Initialized());
    return FUNC_VAR_NAME(DynamicTextChangeNotification)(name, numArgs);
}


void nCore::System::RegisterWindow(LPCTSTR prefix, LPVOID window)
{
    assert(nCore::Initialized());
    FUNC_VAR_NAME(RegisterWindow)(prefix, window);
}


void nCore::System::UnRegisterWindow(LPCTSTR prefix)
{
    assert(nCore::Initialized());
    FUNC_VAR_NAME(UnRegisterWindow)(prefix);
}


Window *nCore::System::FindRegisteredWindow(LPCTSTR prefix)
{
    assert(nCore::Initialized());
    return FUNC_VAR_NAME(FindRegisteredWindow)(prefix);
}


void nCore::System::AddWindowRegistrationListener(LPCTSTR prefix, Window *listener)
{
    assert(nCore::Initialized());
    FUNC_VAR_NAME(AddWindowRegistrationListener)(prefix, listener);
}


void nCore::System::RemoveWindowRegistrationListener(LPCTSTR prefix, Window *listener)
{
    assert(nCore::Initialized());
    FUNC_VAR_NAME(RemoveWindowRegistrationListener)(prefix, listener);
}
