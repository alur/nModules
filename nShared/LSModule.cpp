/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  LSModule.cpp
 *  The nModules Project
 *
 *  Deals with all basic LiteStep module functionality.
 *   
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "../nShared/LiteStep.h"
#include <strsafe.h>
#include "LSModule.hpp"
#include "Factories.h"
#include "ErrorHandler.h"
#include "DrawableWindow.hpp"
#include "../nCoreCom/Core.h"


/// <summary>
/// The main entry point for this DLL.
/// </summary>
/// <param name="module">A handle to the DLL module.</param>
/// <param name="reasonForCall">The reason code that indicates why the DLL entry-point function is being called.</param>
/// <returns>TRUE on success, FALSE on failure.</returns>
BOOL APIENTRY DllMain(HANDLE module, DWORD reasonForCall, LPVOID /* reserved */)
{
    if (reasonForCall == DLL_PROCESS_ATTACH)
    {
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
LSModule::LSModule(LPCTSTR moduleName, LPCTSTR author, VERSION version)
{
    this->author = _tcsdup(author);
    this->drawableClass = 0;
    this->instance = instance;
    this->messageHandler = nullptr;
    this->messageHandlerClass = 0;
    this->moduleName = _tcsdup(moduleName);
    this->parent = parent;
    this->version = version;

    ErrorHandler::Initialize(moduleName);
}


/// <summary>
/// Destructor. Frees allocated resources.
/// </summary>
LSModule::~LSModule()
{
    // Destroy the message handler.
    if (this->messageHandler != nullptr)
    {
        DestroyWindow(this->messageHandler);
        this->messageHandler = nullptr;
    }

    // Unregister window classes.
    if (this->messageHandlerClass != 0)
    {
        UnregisterClass((LPCTSTR)this->messageHandlerClass, this->instance);
        this->messageHandlerClass = 0;
    }
    if (this->drawableClass != 0)
    {
        UnregisterClass((LPCTSTR)this->drawableClass, this->instance);
        this->drawableClass = 0;
    }

    // Free allocated strings.
    free((LPVOID)this->moduleName);
    free((LPVOID)this->author);

    // Disconnect from the core
    nCore::Disconnect();
}


/// <summary>
/// Initalizes the module. Registers window classes and connects with the core.
/// </summary>
/// <param name="parent">The HWND passed to initModuleEx.</param>
/// <param name="instance">The instance of this module.</param>
/// <param name="customMessageClass">Custom window class to use for the message handler.</param>
/// <param name="customDrawableClass">Custom window class to use for the drawable window.</param>
bool LSModule::Initialize(HWND parent, HINSTANCE instance, PWNDCLASSEX customMessageClass, PWNDCLASSEX customDrawableClass)
{
    WNDCLASSEX wc;
    TCHAR className[MAX_PATH];

    this->parent = parent;
    this->instance = instance;

    CoInitializeEx(nullptr, COINIT_MULTITHREADED);

    // Register the messageHandler window class
    StringCchPrintf(className, _countof(className), _T("LS%sMessageHandler"), this->moduleName);
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

    if ((this->messageHandlerClass = RegisterClassEx(customMessageClass == NULL ? &wc : customMessageClass)) == 0)
    {
        return false;
    }

    // Register the drawable window class
    StringCchPrintf(className, _countof(className), _T("LS%sDrawable"), this->moduleName);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.style |= CS_DBLCLKS;

    if ((this->drawableClass = RegisterClassEx(customDrawableClass == NULL ? &wc : customDrawableClass)) == 0)
    {
        return false;
    }

    // Create the main window.
    if ((this->messageHandler = MessageHandler::CreateMessageWindowEx(WS_EX_TOOLWINDOW | WS_EX_COMPOSITED,
        (LPCTSTR)this->messageHandlerClass, _T(""), WS_POPUP, 0, 0, 0, 0, this->parent, NULL, this->instance, this)) == NULL)
    {
        return false;
    }
    SetWindowLongPtr(this->messageHandler, GWLP_USERDATA, MAGIC_DWORD);

    return true;
}


/// <summary>
/// Deinitalizes
/// </summary>
void LSModule::DeInitalize()
{
    // Let go of any factories we allocated.
    Factories::Release();

    //
    CoUninitialize();
}


/// <summary>
/// Tries to connect to nCore.
/// </summary>
/// <param name="minimumCoreVersion">The minimum core version which is acceptable.</param>
bool LSModule::ConnectToCore(VERSION minimumCoreVersion)
{
    switch (nCore::Connect(minimumCoreVersion))
    {
    case S_OK:
        break;
    default:
        ErrorHandler::Error(ErrorHandler::Level::Critical, TEXT("There was a problem connecting to nCore!"));
        return false;
    }

    return true;
}


/// <summary>
/// Creates a top-level drawable window.
/// </summary>
/// <param name="settings">The settings structure to use.</param>
DrawableWindow* LSModule::CreateDrawableWindow(Settings* settings, MessageHandler* msgHandler)
{
    return new DrawableWindow(this->parent, (LPCTSTR)this->drawableClass, this->instance, settings, msgHandler);
}


HWND LSModule::GetMessageWindow()
{
    return this->messageHandler;
}


HINSTANCE LSModule::GetInstance()
{
    return this->instance;
}


/// <summary>
/// Intercepts LiteStep messages before the module can get to them.
/// </summary>
LRESULT WINAPI LSModule::HandleMessage(HWND window, UINT message, WPARAM wParam, LPARAM lParam, LPVOID)
{
    switch (message)
    {
    case LM_GETREVID:
        {
            size_t length;
            LSModule* lsModule = (LSModule*)GetWindowLongPtr(window, 0);
            StringCchPrintf((LPTSTR)lParam, 64, _T("%s: "), lsModule->moduleName);
			length = _tcslen((LPTSTR)lParam);
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
