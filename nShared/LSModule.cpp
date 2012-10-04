/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  LSModule.cpp
 *  The nModules Project
 *
 *  Deals with all basic LiteStep module functionality.
 *   
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "../headers/lsapi.h"
#include <strsafe.h>
#include "LSModule.hpp"
#include "Factories.h"
#include "Error.h"
#include "DrawableWindow.hpp"
#include "../nCoreCom/Core.h"


/// <summary>
/// The main entry point for this DLL.
/// </summary>
BOOL APIENTRY DllMain(HANDLE module, DWORD reasonForCall, LPVOID) {
    if (reasonForCall == DLL_PROCESS_ATTACH)
        DisableThreadLibraryCalls((HINSTANCE)module);
    return TRUE;
}


/// <summary>
/// Constructor.
/// </summary>
LSModule::LSModule(LPCSTR moduleName, LPCSTR author, VERSION version, HINSTANCE instance, UINT lsMessages[]) {
    this->moduleName = _strdup(moduleName);
    this->author = _strdup(author);
    this->version = version;
    this->instance = instance;
    this->lsMessages = lsMessages;
    this->LSWindow = NULL;
}


/// <summary>
/// Destructor. Frees allocated resources.
/// </summary>
LSModule::~LSModule() {
    // Unregister for messages
    SendMessage(GetLitestepWnd(), LM_UNREGISTERMESSAGE, (WPARAM)this->LSWindow, (LPARAM)lsMessages);

    free((LPVOID)this->moduleName);
    free((LPVOID)this->author);

    if (this->LSWindow) {
        DestroyWindow(this->LSWindow);
        this->LSWindow = NULL;
    }

    for (vector<ATOM>::iterator iter = this->registeredWindowClasses.begin(); iter != this->registeredWindowClasses.end(); iter++) {
        UnregisterClass((LPCSTR)*iter, this->instance);
    }

    Factories::Release();
}


/// <summary>
/// Initalizes the module. Registers window classes and connects with the core.
/// </summary>
bool LSModule::Initialize(LPCSTR drawableWindowClasses[], PWNDCLASSEX customClass) {
    // Register window classes.
    if (!RegisterLSWindowClass(customClass)) {
        return false;
    }
    if (drawableWindowClasses != NULL) {
        while (*drawableWindowClasses != NULL) {
            if (!RegisterDrawableWindowClass(*drawableWindowClasses++)) {
                return false;
            }
        }
    }

    // Create the main window.
    if (!CreateLSWindow()) {
        return false;
    }

    // Register for messages
    SendMessage(GetLitestepWnd(), LM_REGISTERMESSAGE, (WPARAM)this->LSWindow, (LPARAM)this->lsMessages);
    SetWindowLongPtr(this->LSWindow, 0, (LONG_PTR)this);

    return true;
}


/// <summary>
/// Tries to connect to nCore.
/// </summary>
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
/// Registers the LiteStep message handling class.
/// </summary>
bool LSModule::RegisterLSWindowClass(PWNDCLASSEX customClass) {
    WNDCLASSEX wc;
    char className[MAX_PATH];

    StringCchPrintf(className, sizeof(className), "LS%sMessageHandler", this->moduleName);

    ZeroMemory(&wc, sizeof(WNDCLASSEX));
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.cbWndExtra = sizeof(LSModule*);
    wc.lpfnWndProc = MessageHandler;
    wc.hInstance = this->instance;
    wc.lpszClassName = className;
    wc.style = CS_NOCLOSE;

    ATOM value = RegisterClassEx(customClass == NULL ? &wc : customClass);

    if (value == 0) {
        HandleError(GetLastError());
        return false;
    }
    
    this->registeredWindowClasses.push_back(value);

    return true;
}


/// <summary>
/// Registers a drawablewindow class.
/// </summary>
bool LSModule::RegisterDrawableWindowClass(LPCSTR name) {
    char className[MAX_PATH];

    StringCchPrintf(className, sizeof(className), "LS%s%sHandler", this->moduleName, name);

    ATOM value = DrawableWindow::RegisterWindowClass(className, this->instance);

    if (value == 0) {
        HandleError(GetLastError());
        return false;
    }
    
    this->registeredWindowClasses.push_back(value);

    return true;
}


/// <summary>
/// Creates the LiteStep message handler window.
/// </summary>
bool LSModule::CreateLSWindow() {
    this->LSWindow = CreateWindowEx(
        WS_EX_TOOLWINDOW | WS_EX_COMPOSITED,
        (LPCSTR)this->registeredWindowClasses[0],
        "", WS_POPUP,
        0, 0, 0, 0, NULL, NULL, this->instance, NULL);

    return (this->LSWindow != NULL);
}


/// <summary>
/// Intercepts LiteStep messages before the module can get to them.
/// </summary>
LRESULT WINAPI LSModule::MessageHandler(HWND window, UINT message, WPARAM wParam, LPARAM lParam) {
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


/// <summary>
/// Returns the window class as the specified id. 0 is the LiteStep message handler. 1+ is the drawable classes, in order.
/// </summary>
ATOM LSModule::GetWindowClass(UINT id) {
    return this->registeredWindowClasses[id];
}


/// <summary>
/// Returns this modules instance.
/// </summary>
HINSTANCE LSModule::GetInstance() {
    return this->instance;
}


/// <summary>
/// Displays a message box with information about the error that occured.
/// </summary>
void LSModule::HandleError(DWORD errorCode) {

}


/// <summary>
/// Returns the LiteStep messaging window.
/// </summary>
HWND LSModule::GetMessageWindow() {
    return this->LSWindow;
}
