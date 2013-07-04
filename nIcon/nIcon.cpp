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
#include "Version.h"


// The messages we want from the core
const UINT gLSMessages[] = { LM_GETREVID, LM_REFRESH, 0 };

// All current icon groups
map<string, IconGroup*> gIconGroups;

// The LiteStep module class
LSModule gLSModule(MODULE_NAME, MODULE_AUTHOR, MakeVersion(MODULE_VERSION));


/// <summary>
/// Called by the LiteStep core when this module is loaded.
/// </summary>
int initModuleEx(HWND parent, HINSTANCE instance, LPCSTR /* path */) {
    if (!gLSModule.Initialize(parent, instance)) {
        return 1;
    }

    if (!gLSModule.ConnectToCore(MakeVersion(CORE_VERSION))) {
        return 1;
    }

    OleInitialize(nullptr);

    LoadSettings();

    return 0;
}


/// <summary>
/// Called by the LiteStep core when this module is about to be unloaded.
/// </summary>
void quitModule(HINSTANCE /* instance */) {
    // Remove all groups
    for (auto group : gIconGroups) {
        delete group.second;
    }
    gIconGroups.clear();

    gLSModule.DeInitalize();

    OleUninitialize();
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
            SendMessage(LiteStep::GetLitestepWnd(), LM_REGISTERMESSAGE, (WPARAM)window, (LPARAM)gLSMessages);
        }
        return 0;

    case WM_DESTROY:
        {
            SendMessage(LiteStep::GetLitestepWnd(), LM_UNREGISTERMESSAGE, (WPARAM)window, (LPARAM)gLSMessages);
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
/// Reads through the .rc files and creates labels.
/// </summary>
void LoadSettings() {
    char szLine[MAX_LINE_LENGTH], szLabel[256];
    LPSTR szTokens[] = { szLabel };
    LPVOID f = LiteStep::LCOpen(nullptr);

    while (LiteStep::LCReadNextConfig(f, "*nIcon", szLine, sizeof(szLine))) {
        LiteStep::LCTokenize(szLine+strlen("*nIcon")+1, szTokens, 1, nullptr);

        gIconGroups.insert(gIconGroups.begin(), std::pair<string, IconGroup*>(string(szLabel), new IconGroup(szLabel)));
    }
    LiteStep::LCClose(f);
}
