/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  nIcon.cpp
 *  The nModules Project
 *
 *  Main .cpp file for the nIcon module.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "../nShared/LiteStep.h"
#include "nIcon.h"
#include "IconGroup.hpp"
#include "../nShared/LSModule.hpp"

// The messages we want from the core
UINT g_lsMessages[] = { LM_GETREVID, LM_REFRESH, 0 };

// The LiteStep module class
LSModule* g_LSModule;

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
/// <param name="window">The window the message is for.</param>
/// <param name="message">The type of message.</param>
/// <param name="wParam">wParam</param>
/// <param name="lParam">lParam</param>
LRESULT WINAPI LSMessageHandler(HWND window, UINT message, WPARAM wParam, LPARAM lParam) {
    switch(message) {
    case WM_CREATE:
        {
            SendMessage(LiteStep::GetLitestepWnd(), LM_REGISTERMESSAGE, (WPARAM)window, (LPARAM)g_lsMessages);
        }
        return 0;

    case WM_DESTROY:
        {
            SendMessage(LiteStep::GetLitestepWnd(), LM_UNREGISTERMESSAGE, (WPARAM)window, (LPARAM)g_lsMessages);
        }
        return 0;

    case LM_REFRESH:
        {
        }
        return 0;
    }
    return DefWindowProc(window, message, wParam, lParam);
}
