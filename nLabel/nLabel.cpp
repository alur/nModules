/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  nLabel.cpp
 *  The nModules Project
 *
 *  Main .cpp file for the nLabel module.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "../headers/lsapi.h"
#include "nLabel.h"
#include "../nShared/LSModule.hpp"
#include "Label.hpp"
#include <map>

using std::map;

// The window classes we want to register
LPCSTR g_windowClasses[] = {"Label", NULL};

// The LSModule class
LSModule* g_LSModule;

// The messages we want from the core
UINT g_lsMessages[] = { LM_GETREVID, LM_REFRESH, NULL };

// All the labels we currently have loaded
map<LPCSTR, Label*> g_Labels;


/// <summary>
/// Called by the LiteStep core when this module is loaded.
/// </summary>
int initModuleEx(HWND /* hWndParent */, HINSTANCE instance, LPCSTR /* szPath */) {
    g_LSModule = new LSModule("nLabel", "Alurcard2", MAKE_VERSION(0,2,0,0), instance, g_lsMessages);
    
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
/// Called by the LiteStep core when this module is about to be unloaded.
/// </summary>
void quitModule(HINSTANCE /* instance */) {
    // Remove all labels
    for (map<LPCSTR, Label*>::const_iterator iter = g_Labels.begin(); iter != g_Labels.end(); iter++) {
        delete iter->second;
    }
    g_Labels.clear();

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


/// <summary>
/// Reads through the .rc files and creates labels.
/// </summary>
void LoadSettings() {
    char szLine[MAX_LINE_LENGTH], szLabel[256];
    LPSTR szTokens[] = { szLabel };
    LPVOID f = LCOpen(NULL);
    LPSTR name;

    while (LCReadNextConfig(f, "*nLabel", szLine, sizeof(szLine))) {
        LCTokenize(szLine+strlen("*nLabel")+1, szTokens, 1, NULL);
        name = _strdup(szLabel);
        g_Labels.insert(g_Labels.begin(), std::pair<LPCSTR, Label*>(name, new Label(name)));
    }
    LCClose(f);
}
