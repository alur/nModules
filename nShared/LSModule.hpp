/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  LSModule.cpp
 *  The nModules Project
 *
 *  Deals with all basic LiteStep module functionality.
 *   
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

#include "Macros.h"
#include "Versioning.h"
#include <vector>

using std::vector;

LRESULT WINAPI LSMessageHandler(HWND window, UINT message, WPARAM wParam, LPARAM lParam);

EXPORT_CDECL(int) initModuleEx(HWND, HINSTANCE, LPCSTR);
EXPORT_CDECL(void) quitModule(HINSTANCE);

class LSModule {
public:
    explicit LSModule(LPCSTR moduleName, LPCSTR author, VERSION version, HINSTANCE instance, UINT lsMessages[]);
    virtual ~LSModule();

    bool Initialize(LPCSTR drawableWindowClasses[]);
    bool ConnectToCore(VERSION minimumCoreVersion);

    ATOM GetWindowClass(UINT id);
    HINSTANCE GetInstance();
    HWND GetMessageWindow();

private:
    // Intercepts LiteStep messages before the module can get to them.
    static LRESULT WINAPI MessageHandler(HWND, UINT, WPARAM, LPARAM);

    bool RegisterLSWindowClass();
    bool RegisterDrawableWindowClass(LPCSTR name);
    bool CreateLSWindow();
    void HandleError(DWORD errorCode);

    LPCSTR moduleName;
    LPCSTR author;
    VERSION version;
    HWND LSWindow;
    HINSTANCE instance;
    UINT* lsMessages;

    vector<ATOM> registeredWindowClasses;
};
