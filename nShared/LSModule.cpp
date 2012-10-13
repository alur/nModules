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
#include "Error.h"
#include "DrawableWindow.hpp"
#include "../nCoreCom/Core.h"


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
/// <param name="parent">The HWND passed to initModuleEx.</param>
/// <param name="moduleName">The name of this module, used for handling LM_GETREVID and window classes.</param>
/// <param name="author">Name of the author(s), used for handling LM_GETREVID.</param>
/// <param name="version">The version of the module, used for handling LM_GETREVID.</param>
/// <param name="instance">The instance of this module.</param>
LSModule::LSModule(HWND parent, LPCSTR moduleName, LPCSTR author, VERSION version, HINSTANCE instance) {
    this->author = _strdup(author);
    this->drawableClass = 0;
    this->instance = instance;
    this->messageHandler = NULL;
    this->messageHandlerClass = 0;
    this->moduleName = _strdup(moduleName);
    this->parent = parent;
    this->version = version;
}


/// <summary>
/// Destructor. Frees allocated resources.
/// </summary>
LSModule::~LSModule() {
    // Destroy all drawables.
    for (map<string, Drawable*>::iterator iter = this->drawables.begin(); iter != this->drawables.end(); ++iter) {
        delete iter->second;
    }
    this->drawables.clear();

    // Destroy the message handler.
    if (this->messageHandler != NULL) {
        DestroyWindow(this->messageHandler);
        this->messageHandler = NULL;
    }

    // Unregister window classes.
    if (this->messageHandlerClass != 0) {
        UnregisterClass((LPCSTR)this->messageHandlerClass, this->instance);
        this->messageHandlerClass = 0;
    }
    if (this->drawableClass != 0) {
        UnregisterClass((LPCSTR)this->drawableClass, this->instance);
        this->drawableClass = 0;
    }

    // Free allocated strings.
    free((LPVOID)this->moduleName);
    free((LPVOID)this->author);

    // Let go of any factories we allocated.
    Factories::Release();
}


/// <summary>
/// Initalizes the module. Registers window classes and connects with the core.
/// </summary>
/// <param name="customMessageClass">Custom window class to use for the message handler.</param>
/// <param name="customDrawableClass">Custom window class to use for the drawable window.</param>
bool LSModule::Initialize(PWNDCLASSEX customMessageClass, PWNDCLASSEX customDrawableClass) {
    WNDCLASSEX wc;
    char className[MAX_PATH];

    // Register the messageHandler window class
    StringCchPrintf(className, sizeof(className), "LS%sMessageHandler", this->moduleName);
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
        return false;
    }

    // Register the drawable window class
    StringCchPrintf(className, sizeof(className), "LS%sDrawable", this->moduleName);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.style |= CS_DBLCLKS;

    if ((this->drawableClass = RegisterClassEx(customDrawableClass == NULL ? &wc : customDrawableClass)) == 0) {
        return false;
    }

    // Create the main window.
    if ((this->messageHandler = MessageHandler::CreateMessageWindowEx(WS_EX_TOOLWINDOW | WS_EX_COMPOSITED,
        (LPCSTR)this->messageHandlerClass, "", WS_POPUP, 0, 0, 0, 0, this->parent, NULL, this->instance, this)) == NULL)
    {
        return false;
    }

    return true;
}


/// <summary>
/// Tries to connect to nCore.
/// </summary>
/// <param name="minimumCoreVersion">The minimum core version which is acceptable.</param>
bool LSModule::ConnectToCore(VERSION minimumCoreVersion) {
    switch (nCore::Init(minimumCoreVersion)) {
    case S_OK:
        break;
    default:
        ErrorMessage(E_LVL_ERROR, "There was a problem connecting to nCore!");
        return false;
    }

    return true;
}


/// <summary>
/// Creates a top-level drawable window.
/// </summary>
/// <param name="settings">The settings structure to use.</param>
DrawableWindow* LSModule::CreateDrawableWindow(Settings* settings, MessageHandler* msgHandler) {
    return new DrawableWindow(this->parent, (LPCSTR)this->drawableClass, this->instance, settings, msgHandler);
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
LRESULT WINAPI LSModule::HandleMessage(HWND window, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case LM_GETREVID:
        {
            size_t length;
            LSModule* lsModule = (LSModule*)GetWindowLongPtr(window, 0);
            StringCchPrintf((LPSTR)lParam, 64, "%s: ", lsModule->moduleName);
			length = strlen((LPSTR)lParam);
            GetVersionString(lsModule->version, (LPSTR)lParam + length, 64 - length, false);
            
            if (SUCCEEDED(StringCchLength((LPSTR)lParam, 64, &length))) {
                return length;
            }

            lParam = NULL;
        }
        return 0;

    default:
        return ::LSMessageHandler(window, message, wParam, lParam);
    }
}
