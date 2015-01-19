//-------------------------------------------------------------------------------------------------
// /nShared/LSModule.cpp
// The nModules Project
//
// Deals with all basic LiteStep module functionality.
//-------------------------------------------------------------------------------------------------
#include "ErrorHandler.h"
#include "Factories.h"
#include "LSModule.hpp"
#include "Window.hpp"

#include "../nCoreCom/Core.h"

#include "../nShared/LiteStep.h"

#include <strsafe.h>


/// <summary>
/// The main entry point for this DLL.
/// </summary>
/// <param name="module">A handle to the DLL module.</param>
/// <param name="reasonForCall">The reason code that indicates why the DLL entry-point function is being called.</param>
/// <returns>TRUE on success, FALSE on failure.</returns>
BOOL APIENTRY DllMain(HANDLE module, DWORD reasonForCall, LPVOID /* reserved */) {
  if (reasonForCall == DLL_PROCESS_ATTACH) {
    DisableThreadLibraryCalls((HINSTANCE)module);
  }
  return TRUE;
}


/// <summary>
/// Initializes all variables.
/// </summary>
/// <param name="moduleName">The name of this module, used for handling LM_GETREVID and window classes.</param>
/// <param name="author">Name of the author(s), used for handling LM_GETREVID.</param>
/// <param name="version">The version of the module, used for handling LM_GETREVID.</param>
LSModule::LSModule(LPCTSTR moduleName, LPCTSTR author, VERSION version) {
  this->author = author;
  this->drawableClass = 0;
  this->instance = instance;
  this->messageHandler = nullptr;
  this->messageHandlerClass = 0;
  this->moduleName = moduleName;
  this->parent = parent;
  this->version = version;

  ErrorHandler::Initialize(moduleName);
}


/// <summary>
/// Initalizes the module. Registers window classes and connects with the core.
/// </summary>
/// <param name="parent">The HWND passed to initModuleEx.</param>
/// <param name="instance">The instance of this module.</param>
/// <param name="customMessageClass">Custom window class to use for the message handler.</param>
/// <param name="customDrawableClass">Custom window class to use for the drawable window.</param>
bool LSModule::Initialize(HWND parent, HINSTANCE instance, PWNDCLASSEX customMessageClass,
    PWNDCLASSEX customDrawableClass) {
  WNDCLASSEX wc;
  TCHAR className[MAX_PATH];
  HRESULT hr;

  this->parent = parent;
  this->instance = instance;

  if (FAILED(hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE))) {
    ErrorHandler::ErrorHR(ErrorHandler::Level::Critical, hr, L"Failed to initialize COM.");
    return false;
  }

  // Register the messageHandler window class
  StringCchPrintf(className, _countof(className), L"LS%sMessageHandler", this->moduleName);
  ZeroMemory(&wc, sizeof(WNDCLASSEX));
  wc.cbSize = sizeof(WNDCLASSEX);
  wc.hInstance = this->instance;
  wc.lpszClassName = className;
  wc.style = CS_NOCLOSE;
  MessageHandler::FixWindowClass(&wc);

  // Fix up custom window classes
  if (customMessageClass) {
    MessageHandler::FixWindowClass(customMessageClass);
  }
  if (customDrawableClass) {
    MessageHandler::FixWindowClass(customDrawableClass);
  }

  if ((this->messageHandlerClass = RegisterClassEx(customMessageClass == NULL ? &wc : customMessageClass)) == 0) {
    ErrorHandler::ErrorHR(ErrorHandler::Level::Critical, HRESULT_FROM_WIN32(GetLastError()),
      L"Failed to register the message handler class.");
    return false;
  }

  // Register the drawable window class
  StringCchPrintf(className, _countof(className), L"LS%sDrawable", this->moduleName);
  wc.hCursor = LoadCursor(NULL, IDC_ARROW);
  wc.style |= CS_DBLCLKS;

  if ((this->drawableClass = RegisterClassEx(customDrawableClass == NULL ? &wc : customDrawableClass)) == 0) {
    ErrorHandler::ErrorHR(ErrorHandler::Level::Critical, HRESULT_FROM_WIN32(GetLastError()),
      L"Failed to register the drawable window class.");
    return false;
  }

  // Create the main window.
  if ((this->messageHandler = MessageHandler::CreateMessageWindowEx(WS_EX_TOOLWINDOW,
      (LPCTSTR)this->messageHandlerClass, L"", this->parent ? WS_CHILD : WS_POPUP, 0, 0, 0, 0,
      this->parent, NULL, this->instance, this)) == NULL) {
    ErrorHandler::ErrorHR(ErrorHandler::Level::Critical, HRESULT_FROM_WIN32(GetLastError()),
      L"Failed to register the LiteStep message window.");
    return false;
  }
  SetWindowLongPtr(this->messageHandler, GWLP_USERDATA, MAGIC_DWORD);

  return true;
}


/// <summary>
/// Deinitalizes
/// </summary>
void LSModule::DeInitalize() {
  // Let go of any factories we allocated.
  Factories::Release();
  CoUninitialize();

  // Destroy the message handler.
  if (this->messageHandler != nullptr) {
    DestroyWindow(this->messageHandler);
    this->messageHandler = nullptr;
  }

  // Unregister window classes.
  if (this->messageHandlerClass != 0) {
    if (UnregisterClass((LPCTSTR)this->messageHandlerClass, this->instance) == FALSE) {
      ErrorHandler::ErrorHR(ErrorHandler::Level::Critical, HRESULT_FROM_WIN32(GetLastError()),
        L"Failed to unregister the message handler class.");
    }
    this->messageHandlerClass = 0;
  }
  if (this->drawableClass != 0) {
    if (UnregisterClass((LPCTSTR)this->drawableClass, this->instance) == FALSE) {
      ErrorHandler::ErrorHR(ErrorHandler::Level::Critical, HRESULT_FROM_WIN32(GetLastError()),
        L"Failed to unregister the drawable window class.");
    }
    this->drawableClass = 0;
  }

  // Disconnect from the core
  nCore::Disconnect();
}


/// <summary>
/// Tries to connect to nCore.
/// </summary>
/// <param name="minimumCoreVersion">The minimum core version which is acceptable.</param>
bool LSModule::ConnectToCore(VERSION minimumCoreVersion) {
  switch (nCore::Connect(minimumCoreVersion)) {
  case S_OK:
    break;
  default:
    ErrorHandler::Error(ErrorHandler::Level::Critical,
      L"There was a problem connecting to nCore!");
    return false;
  }

  return true;
}


/// <summary>
/// Creates a top-level drawable window.
/// </summary>
/// <param name="settings">The settings structure to use.</param>
Window *LSModule::CreateDrawableWindow(Settings *settings, MessageHandler *msgHandler) {
  return new Window(this->parent, (LPCTSTR)this->drawableClass, this->instance, settings,
    msgHandler);
}


HWND LSModule::GetMessageWindow() {
  return this->messageHandler;
}


HINSTANCE LSModule::GetInstance() {
  return this->instance;
}


/// <summary>
/// Intercepts LiteStep messages before the module can get to them.
/// </summary>
LRESULT WINAPI LSModule::HandleMessage(HWND window, UINT message, WPARAM wParam, LPARAM lParam,
  LPVOID) {
  switch (message) {
  case LM_GETREVID:
    {
      size_t length;
      LSModule* lsModule = (LSModule*)GetWindowLongPtr(window, 0);
      StringCchPrintf((LPTSTR)lParam, 64, L"%s: ", lsModule->moduleName);
      length = wcslen((LPTSTR)lParam);
      GetVersionString(lsModule->version, (LPTSTR)lParam + length, 64 - length, false);

      if (SUCCEEDED(StringCchLength((LPTSTR)lParam, 64, &length))) {
        return length;
      }

      lParam = NULL;
    }
    return 0;

  default:
    return ::LSMessageHandler(window, message, wParam, lParam);
  }
}
