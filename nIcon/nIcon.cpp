/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  nIcon.cpp
 *  The nModules Project
 *
 *  Main .cpp file for the nIcon module.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "../headers/lsapi.h"
#include "nIcon.h"
#include "IconGroup.hpp"
#include "../nShared/LSModule.hpp"

// The messages we want from the core
UINT g_lsMessages[] = { LM_GETREVID, LM_REFRESH, 0 };

// The LiteStep module class
LSModule* g_LSModule;

// The window classes we want to register
LPCSTR g_windowClasses[] = {"Tray", "Icon", NULL};

//
IconGroup* pGroup;


/// <summary>
/// Called by the LiteStep core when this module is loaded.
/// </summary>
int initModuleEx(HWND parent, HINSTANCE instance, LPCSTR /* szPath */) {
    g_LSModule = new LSModule(parent, "nIcon", "Alurcard2", MAKE_VERSION(0,2,0,0), instance);
    
    if (!g_LSModule->Initialize()) {
        delete g_LSModule;
        return 1;
    }

    if (!g_LSModule->ConnectToCore(MAKE_VERSION(0,2,0,0))) {
        delete g_LSModule;
        return 1;
    }

    pGroup = new IconGroup("DesktopIcons");

    return 0;
}


/// <summary>
/// Called by the LiteStep core when this module is about to be unloaded.
/// </summary>
void quitModule(HINSTANCE /* instance */) {
    if (pGroup) {
        delete pGroup;
    }

    // Deinitalize
    if (g_LSModule) {
        delete g_LSModule;
    }
}


/// <summary>
/// Handles the main window's messages.
/// </summary>
/// <param name="hWnd">The window the message is for.</param>
/// <param name="uMsg">The type of message.</param>
/// <param name="wParam">wParam</param>
/// <param name="lParam">lParam</param>
LRESULT WINAPI LSMessageHandler(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch(uMsg) {
        case LM_REFRESH: {
            return 0;
        }
    }
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}
