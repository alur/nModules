/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  nPopup.cpp
 *  The nModules Project
 *
 *  Main .cpp file for the nTask module.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "../headers/lsapi.h"
#include "../nShared/LSModule.hpp"
#include "../nShared/Debugging.h"
#include "nPopup.h"

// The window classes we want to register
LPCSTR g_windowClasses[] = {"Popup", "Item", NULL};

// The LSModule class
LSModule* g_LSModule;

// The messages we want from the core
UINT g_lsMessages[] = { LM_GETREVID, LM_REFRESH, NULL };


/// <summary>
/// Called by the LiteStep core when this module is loaded.
/// </summary>
int initModuleEx(HWND /* hWndParent */, HINSTANCE instance, LPCSTR /* szPath */) {
    g_LSModule = new LSModule("nPopup", "Alurcard2", MAKE_VERSION(0,2,0,0), instance, g_lsMessages);
    
    if (!g_LSModule->Initialize(g_windowClasses)) {
        delete g_LSModule;
        return 1;
    }

    if (!g_LSModule->ConnectToCore(MAKE_VERSION(0,2,0,0))) {
        delete g_LSModule;
        return 1;
    }

    // Load settings
    LoadSettings();

    return 0;
}


/// <summary>
/// Called by the core when this module is about to be unloaded.
/// </summary>
void quitModule(HINSTANCE /* hDllInstance */) {
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
        case LM_REFRESH:
            return 0;
    }
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}


/// <summary>
/// 
/// </summary>
void LoadSettings() {
    LoadPopups();
}


/// <summary>
/// Reads through the .rc files and creates Taskbars.
/// </summary>
void LoadPopups() {
    char szLine[MAX_LINE_LENGTH], szToken[MAX_LINE_LENGTH];
    LPVOID f = LCOpen(NULL);

    LPSTR icon = NULL;

    while (LCReadNextConfig(f, "*Popup", szLine, sizeof(szLine))) {
        //while (GetToken(
        //LCTokenize(szLine+strlen("*nTaskbar")+1, szTokens, 1, NULL);
        //name = _strdup(szLabel);
        //g_Taskbars.insert(g_Taskbars.begin(), std::pair<LPCSTR, Taskbar*>(name, new Taskbar(name)));
        TRACE(szLine);
    }
    LCClose(f);
}
