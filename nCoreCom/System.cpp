/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  System.cpp
 *  The nModules Project
 *
 *  Description...
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "Core.h"
#include "CoreComHelpers.h"

#include "../Utilities/Common.h"

// Pointers to functions in the core. Initalized by _Init, reset by _DeInit.
namespace nCore {
  DECL_FUNC_VAR(FetchMonitorInfo);

  DECL_FUNC_VAR(LoadFolder);

  namespace System {
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
HRESULT nCore::System::_Init(HMODULE hCoreInstance) {
  INIT_FUNC(FetchMonitorInfo);

  INIT_FUNC(LoadFolder);

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
void nCore::System::_DeInit() {
  FUNC_VAR_NAME(FetchMonitorInfo) = nullptr;

  FUNC_VAR_NAME(LoadFolder) = nullptr;

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

IParsedText* nCore::System::ParseText(LPCWSTR text) {
  ASSERT(nCore::Initialized());
  return FUNC_VAR_NAME(ParseText)(text);
}

BOOL nCore::System::RegisterDynamicTextFunction(LPCWSTR name, UCHAR numArgs,
  FORMATTINGPROC formatter, bool dynamic) {
  ASSERT(nCore::Initialized());
  return FUNC_VAR_NAME(RegisterDynamicTextFunction)(name, numArgs, formatter, dynamic);
}

BOOL nCore::System::UnRegisterDynamicTextFunction(LPCWSTR name, UCHAR numArgs) {
  ASSERT(nCore::Initialized());
  return FUNC_VAR_NAME(UnRegisterDynamicTextFunction)(name, numArgs);
}

BOOL nCore::System::DynamicTextChangeNotification(LPCWSTR name, UCHAR numArgs) {
  ASSERT(nCore::Initialized());
  return FUNC_VAR_NAME(DynamicTextChangeNotification)(name, numArgs);
}

void nCore::System::RegisterWindow(LPCTSTR prefix, Window *window) {
  ASSERT(nCore::Initialized());
  FUNC_VAR_NAME(RegisterWindow)(prefix, window);
}

void nCore::System::UnRegisterWindow(LPCTSTR prefix) {
  ASSERT(nCore::Initialized());
  FUNC_VAR_NAME(UnRegisterWindow)(prefix);
}

class Window *nCore::System::FindRegisteredWindow(LPCTSTR prefix) {
  ASSERT(nCore::Initialized());
  return FUNC_VAR_NAME(FindRegisteredWindow)(prefix);
}

void nCore::System::AddWindowRegistrationListener(LPCTSTR prefix, Window *listener) {
  ASSERT(nCore::Initialized());
  FUNC_VAR_NAME(AddWindowRegistrationListener)(prefix, listener);
}

void nCore::System::RemoveWindowRegistrationListener(LPCTSTR prefix, Window *listener) {
  ASSERT(nCore::Initialized());
  FUNC_VAR_NAME(RemoveWindowRegistrationListener)(prefix, listener);
}

MonitorInfo &nCore::FetchMonitorInfo() {
  ASSERT(nCore::Initialized());
  return FUNC_VAR_NAME(FetchMonitorInfo)();
}

UINT64 nCore::LoadFolder(LoadFolderRequest &request, FileSystemLoaderResponseHandler *handler) {
  ASSERT(nCore::Initialized());
  return FUNC_VAR_NAME(LoadFolder)(request, handler);
}
