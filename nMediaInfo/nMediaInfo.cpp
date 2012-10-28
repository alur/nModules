/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  nMediaInfo.cpp
 *  The nModules Project
 *
 *  Main .cpp file for the nMediaInfo module.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "../nShared/LiteStep.h"
#include "nMediaInfo.h"
#include "../nShared/LSModule.hpp"
#include <map>
#include <strsafe.h>
#include "TextFunctions.h"
#include "CoverArt.hpp"
#include "Bangs.h"

using std::map;

// The LSModule class
LSModule* g_LSModule;

// The messages we want from the core
UINT g_lsMessages[] = { LM_GETREVID, LM_REFRESH, NULL };

//
UINT WinampSongChangeMsg = 0;

//
map<string, CoverArt*> g_CoverArt;


/// <summary>
/// Called by the LiteStep core when this module is loaded.
/// </summary>
int initModuleEx(HWND parent, HINSTANCE instance, LPCSTR /* szPath */) {
    g_LSModule = new LSModule(parent, "nMediaInfo", "Alurcard2", MAKE_VERSION(0,2,0,0), instance);
    
    if (!g_LSModule->Initialize()) {
        delete g_LSModule;
        return 1;
    }

    if (!g_LSModule->ConnectToCore(MAKE_VERSION(0,2,0,0))) {
        delete g_LSModule;
        return 1;
    }

    WinampSongChangeMsg = RegisterWindowMessageW(L"WinampSongChange");

    // Load settings
    LoadSettings();

    TextFunctions::_Register();
    TextFunctions::_Update();
    Bangs::_Register();

    return 0;
}


/// <summary>
/// Called by the LiteStep core when this module is about to be unloaded.
/// </summary>
void quitModule(HINSTANCE /* instance */) {
    TextFunctions::_UnRegister();
    Bangs::_Unregister();

    for (map<string, CoverArt*>::const_iterator coverArt = g_CoverArt.begin(); coverArt != g_CoverArt.end(); ++coverArt) {
        delete coverArt->second;
    }
    g_CoverArt.clear();

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
    if (message == WinampSongChangeMsg) {
        Update();
    }
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


/// <summary>
/// Loads settings.
/// </summary>
void LoadSettings() {
    char szLine[MAX_LINE_LENGTH], szLabel[256];
    LPSTR szTokens[] = { szLabel };
    LPVOID f = LiteStep::LCOpen(NULL);

    while (LiteStep::LCReadNextConfig(f, "*nCoverArt", szLine, sizeof(szLine))) {
        LiteStep::LCTokenize(szLine+strlen("*nCoverArt")+1, szTokens, 1, NULL);
        g_CoverArt.insert(g_CoverArt.begin(), std::pair<string, CoverArt*>(string(szLabel), new CoverArt(szLabel)));
    }
    LiteStep::LCClose(f);
}


void Update() {
    static bool open = true;

    if (open) {
        // Winamp will spam a lot of Update messages on every track change, and they go through asynchronously.
        // I was unable to solve this with either mutexes or critical sections for some reason. (maybe it's not a threading problem?).
        // This solution seems to work good though, and comes with added benefit of usually ignoring the extra messages.
        open = false;
        TextFunctions::_Update();
        for (map<string, CoverArt*>::const_iterator coverArt = g_CoverArt.begin(); coverArt != g_CoverArt.end(); ++coverArt) {
            coverArt->second->Update();
        }
        open = true;
    }
}
